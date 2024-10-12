#include "Command.hpp"

Command::~Command()
{

}

const std::string   Command::getChannelName() const
{
    return _channelName;
}

const std::string   Command::getCommand() const
{
    return _command;
}

const std::vector<std::string>   Command::getArguments() const
{
    return _arguments;
}

void    Command::getAuthenticated(std::string input, Client& client)
{
//    static bool     passwordMatch;
    static bool     nicknameSet;
//    static bool     usernameSet;

    parseStr(input);//does this come in the same way as irssi?

    if (getCommand() == "NICK")
    {
		if (_channelName.empty())
			std::cerr << "Not enough parameters" << std::endl; //where does this message end up? serverside? delete if yes
        else if (!nicknameUnique(_channelName)) //is this where the nickname is??
			std::cerr << "Choose a unique nickname" << std::endl;
        else
        {
            client.setNickname(_channelName);
            nicknameSet = true;
        }
    }
    /*
    else if (getCommand() == "USER")
        setUsername(client, _arguments[0]); 
    else if (getCommand() == "PASS")
        checkPassword(client, _arguments[0]); 
*/
    client.sendMessageToClient("Please use either NICK, PASS or USER to authorize");
}

void    Command::parseStr(std::string str)
{
    std::vector<std::string>    words;
    std::string                 word;
    std::stringstream           stream(str);

    while (stream >> word)
        words.push_back(word);
    
    this->_command = words.front();
    words.erase(words.begin());
    if (getCommand() == "INVITE") //only when command is invite does irssi switch order in arguments. Check reference below
    {
        this->_arguments.push_back(words.front());
        words.erase(words.begin());
        this->_channelName = words.front();
    }
    else
    {
        this->_channelName = words.front();
        words.erase(words.begin());
        this->_arguments.insert(_arguments.end(), words.begin(), words.end());
    }

    std::cout << std::endl << std::endl << *this << std::endl;
}

void    Command::handleTopic(Client& client)
{
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (!targetChannel)
        return ;
    if (_arguments.empty()) //view the topic
    {
        if (targetChannel->getTopicName().empty()) // no topic is set
        {
            std::string clientMessage = ":server 331 " + client.getNickName() + " " + targetChannel->getTopic() + " : No topic is set";
            std::cout << clientMessage << std::endl;
            client.sendMessageToClient(clientMessage);
        }
        else //show the topic
        {
            std::string clientMessage = ":server 332 " + client.getNickName() + " " + targetChannel->getTopic() + " :" + targetChannel->getTopicName();
            std::cout << clientMessage << std::endl;
            client.sendMessageToClient(clientMessage);
        }
    }
    else //extra argument to set the topic. Might have to add extra step to check if they added more than 1 argument
    {
        targetChannel->setTopic(client, _arguments[0]);
        std::string clientMessage = ":server 332 " + client.getNickName() + " " + targetChannel->getTopic() + " :" + targetChannel->getTopicName();
        std::cout << clientMessage << std::endl;
        client.sendMessageToClient(clientMessage);
    }
}

void    Command::parseCMD(std::string input, Client& client)
{
    parseStr(input);

    if (getCommand() == "JOIN")
        joinChannel(client);
    else if (getCommand() == "TOPIC")
        handleTopic(client);
    else if (getCommand() == "KICK")
        executeKick(client, _arguments[0]);
    else if (getCommand() == "INVITE")
        executeInvite(client, _arguments[0]);
    else if (getCommand() == "MODE")
    {
        if (_arguments[0][0] == '+')
            addPrivileges(client);
        else if (_arguments[0][0] == '-')
            removePrivileges(client); 
    }
    else if (getCommand() == "PRIVMSG")
    {
        std::string message;
        for (size_t i = 0; i < getArguments().size(); ++i)
        {
            message += getArguments()[i];
            if (i < getArguments().size() - 1)  // Add a space between words
                message += " ";
        }
        if (!targetIsUser())
        {
            Client* recipient = _server.getClientByName(getChannelName());
            std::string privMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + recipient->getNickName() + " " + message;
            recipient->sendMessageToClient(privMsg);        
        }
        else
        {
            Channel* targetChannel = _server.findChannel(getChannelName());
         //   std::cout << std::endl <<std::endl << "Broadcast to: " << targetChannel->getTopic() << std::endl <<std::endl;
            //if (targetChannel) //I believe this is unncessary. If we came here the channel name HAS to exist. This definitely needs some testing though
                targetChannel->broadcastMessage(message, client);
        }
    }
}

void    Command::executeKick(Client& client, std::string targetClientName)
{
    Client* targetClient = _server.getClientByName(targetClientName);
    if (!targetClient)
        return;
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (!targetChannel)
        return;
    targetChannel->kickClient(client, targetClient);    
}

void    Command::executeInvite(Client& client, std::string targetClientName)
{
    Client* targetClient = _server.getClientByName(targetClientName);
    if (!targetClient)
        return;
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (!targetChannel)
        return;
    targetChannel->inviteClient(client, targetClient);    
}

void    Command::addPrivileges(Client& client)
{
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (targetChannel == NULL)
        return;
    if (_arguments[0] == "+o")
    {
        if (_arguments.size() > 1 && !_arguments[1].empty())
        {
            Client* targetClient = _server.getClientByName(_arguments[1]);
            targetChannel->giveOperatorStatus(client, targetClient);
        }
    }
    else if (_arguments[0] == "+i")
        targetChannel->setInviteOnly(1, client);
    else if (_arguments[0] == "+t")
        targetChannel->setTopicPrivileges(1, client);
    else if (_arguments[0] == "+k")
    {
        if (_arguments.size() <= 1) //no password passed
            targetChannel->setChannelPassword(1, client, NULL);
        else
            targetChannel->setChannelPassword(1, client, &_arguments[1]);
    }
}

void    Command::removePrivileges(Client& client)
{
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (targetChannel == NULL)
        return;
    if (_arguments[0] == "-o")
    {
        if (_arguments.size() > 1 && !_arguments[1].empty())
        {
            Client* targetClient = _server.getClientByName(_arguments[1]);//could maybe do error check here. Instead of looking for client in server, we look for client in channel
            targetChannel->removeOperatorStatus(client, targetClient);
        }
    }
    else if (_arguments[0] == "-i")
        targetChannel->setInviteOnly(0, client);
    else if (_arguments[0] == "-t")
        targetChannel->setTopicPrivileges(0, client);
    else if (_arguments[0] == "-k")
        targetChannel->setChannelPassword(0, client, NULL);
}

bool    Command::targetIsUser()
{
    if (_channelName[0] == '#')
    {
        return true;
    }
    return false;
}

void    Command::joinChannel(Client& client) //2 steps: 1 = creating the channel and adding it the vector 'channels' on server | 2 = adding client to vector 'users' on channel
{
    Channel* existingChannel = NULL;

    //step 1
    for (std::vector<Channel>::const_iterator it = _server.getChannels().begin(); it != _server.getChannels().end(); ++it)
    {
        if (it->getTopic() == getChannelName())
        {
            existingChannel = const_cast<Channel*>(&(*it));
            break;
        }
    }
    if (existingChannel == NULL)
    {
        Channel newChannel;
        newChannel.setUp(getChannelName());
        _server.getChannels().push_back(newChannel);
       
        std::cout << "Succesfully added the channel -" << _server.getChannels().back().getTopic() << "- to vector channel in server class" << std::endl;
       
        existingChannel = &_server.getChannels().back();
    }

    //step 2
    if (existingChannel->checkIsInvited(client)) //CLient is on invited list == auto accept
    {
        existingChannel->addUser(client);
        std::string joinMessage = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " JOIN :" + existingChannel->getTopic();
        existingChannel->broadcastMessage(joinMessage, client);
        std::cout << "Succesfully added user -" << existingChannel->getUsers().back()->getNickName() << "- to -" << existingChannel->getTopic() << std::endl << std::endl;
        return ;
    }

    if (existingChannel->getInviteOnly() == false) //Needs more logic to implement it. solve this at INVITE
    {
        if (existingChannel->getChannelPassword() == true)
        {
            if (_arguments.size() > 0)
            {
                if (_arguments[0] != existingChannel->getPassword())
                {
                    std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + existingChannel->getTopic() + "Incorrect password";
                    std::cout << message << std::endl; //or send msg to client?
                }
              else
                {
                    existingChannel->addUser(client);
                    std::string joinMessage = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " JOIN :" + existingChannel->getTopic();
                    existingChannel->broadcastMessage(joinMessage, client);
                    std::cout << "Succesfully added user -" << existingChannel->getUsers().back()->getNickName() << "- to -" << existingChannel->getTopic() << std::endl << std::endl;
                }
            }
            else
            {
                std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + existingChannel->getTopic() + "You have to enter a password";
                std::cout << message << std::endl; //or send msg to client?
            }
        }
        else
        {
            existingChannel->addUser(client);
            std::cout << "Succesfully added user -" << existingChannel->getUsers().back()->getNickName() << "- to -" << existingChannel->getTopic() << std::endl << std::endl;
        }
    }
    else
    {
        std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + existingChannel->getTopic() + " Channel is for invite only";
        std::cout << message << std::endl; //or send msg to client?
    }
}

std::ostream& operator<<(std::ostream &os, const Command& command)
{
    os << "Channel name: " << command.getChannelName() << std::endl;
    os << "Command: " << command.getCommand() << std::endl;
    os << "Arguments: ";
    for (size_t i = 0; i < command.getArguments().size(); ++i)
    {
        os << command.getArguments()[i];
        if (i != command.getArguments().size() - 1)
            os << " ";  // Add a space between arguments, but not after the last one
    }
    os << std::endl;
    return os;
}


//This is a reference guide as to how the input and commands are given as input and how irssi sends them to the server.
//"Client <4> Data:", "Input = ", and "Rest of stream = " is my print message. Ignore this
//@Matisse, @yannick: Leave this in till end of project for easier troubleshooting

/*
/join general:
""
Client <4> Data: JOIN #general
Input = JOIN
Rest of stream =  #general
""

//FOLLOWING IS AFTER WE JOINED THE "GENERAL" CHANNEL

/topic kaas:
"
Client <4> Data: TOPIC #general :kaas
Input = TOPIC
Rest of stream =  #general :kaas
"

/kick matisse:
"
Client <4> Data: KICK #general matisse :
Input = KICK
Rest of stream =  #general matisse :
"

/mode +i
"
Client <4> Data: MODE #general +i
Input = MODE
Rest of stream =  #general +i
"

/invite matisse:
"
Client <4> Data: INVITE matisse #general
Input = INVITE
Rest of stream =  matisse #general
"

/hi there:
"
Client <4> Data: PRIVMSG #general :hi there
Input = PRIVMSG
Rest of stream =  #general :hi there
"

*/