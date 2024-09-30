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

    std::cout << "Command parse test: " << std::endl << *this << std::endl;
}

void    Command::parseCMD(std::string input, Client& client)
{
    parseStr(input);

    if (getCommand() == "JOIN")
        joinChannel(client);
/*
    else if (getCommand() == "TOPIC")
        setChannelName();
    else if (getCommand() == "KICK")
        executeKick();
    else if (getCommand() == "INVITE")
        executeInvite();
*/

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

void    Command::addPrivileges(Client& client)
{
    //Weird issue happened here. Check the issues board in ft_irc
//    std::cout << "MODEtest 0" << std::endl;
  //  std::cout << "_server.findChannel(getChannelName()) = " << _server.findChannel(getChannelName()) << std::endl;
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (targetChannel == NULL)
        return;
    if (_arguments[0] == "+o")
    {
        Client* targetClient = _server.getClientByName(_arguments[1]);
        targetChannel->giveOperatorStatus(client, targetClient);
    }
    else if (_arguments[0] == "+i")
        targetChannel->setInviteOnly(1, client);
    else if (_arguments[0] == "+t")
        targetChannel->setTopicPrivileges(1, client);
    else if (_arguments[0] == "+k")
        targetChannel->setChannelPassword(1, client, _arguments[1]);

}

void    Command::removePrivileges(Client& client)
{
    Channel* targetChannel = _server.findChannel(getChannelName());
    if (targetChannel == NULL)
        return;
    Client* targetClient = _server.getClientByName(_arguments[1]);//could maybe do error check here. Instead of looking for client in server, we look for client in channel
    if (_arguments[0] == "-o")
        targetChannel->removeOperatorStatus(client, targetClient);
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
        newChannel.setUp(getChannelName()); //setUp function needs some more body and finetuning
        _server.getChannels().push_back(newChannel);
       
        std::cout << "Succesfully added the channel -" << _server.getChannels().back().getTopic() << "- to vector channel in server class" << std::endl; //Errors here: after joining different channel, output remained inconsistent about channelname
       
        existingChannel = &_server.getChannels().back();
    }
    //step 2
    if (existingChannel->getInviteOnly() == false) //Needs more logic to implement it. solve this at INVITE
    {
        if (existingChannel->getChannelPassword() == true && _arguments[0] == existingChannel->getPassword())
        {

        }
        else
        {
            existingChannel->addUser(client);
            std::cout << "Succesfully added user -" << existingChannel->getUsers().back()->getNickName() << "- to -" << existingChannel->getTopic() << std::endl << std::endl; //
        }
    }
    else
    {
        std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + existingChannel->getTopic() + " Channel is for invite only";
        std::cout << message << std::endl;
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