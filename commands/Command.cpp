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
    try
    {
        parseStr(input);//does this come in the same way as irssi?

    if (getCommand() == "NICK")
    {
		if (_arguments.size() > 0)
            throw std::runtime_error("Nickname can only accept 1 parameter");
        else
            client.setNickname(_channelName, _server);
    }
    else if (getCommand() == "USER")
        client.setUsername(_channelName, _arguments); //check parse first
    else if (getCommand() == "PASS")
    {
        if (client.getPasswordMatch() == true)
            throw std::runtime_error("Password is already correct!");
        else if (_arguments.size() > 0) //this statement could be wrong. need tests
            throw std::runtime_error("Please use 1 parameter for the password");
        else
            checkPassword(client, _channelName, _server.getPassword()); 
    }

    if (client.checkAuthorized() == true)
    {
        client.setAuthorized();
        client.sendMessageToClient("You are now authorized to use the irc server");
        std::string welcomeMessage = ":serverhostname 001 " + client.getNickName() + " :Welcome to the IRC network, " + client.getNickName() + "!\r\n";
	    client.sendMessageToClient(welcomeMessage);
        std::cout << client << std::endl;
    }
    else
        client.sendMessageToClient("Please set NICK, PASS and USER to authorize");
    }
    catch (const std::exception& e) // Catch error
    {
        client.sendMessageToClient(e.what()); // Send error message to client
        std::cerr << "Error: " << e.what() << std::endl; // Log the error
    }
}

bool	findAlpha(std::string str)
{
	if (isalpha(str[0]))
		return true;
	return false;
}

void    Command::parseStr(std::string str) //need to add in a throw here that will handle an empty input + empty arguments
{
    std::vector<std::string>    words;
    std::string                 word;
    std::stringstream           stream(str);

	if (!findAlpha(str))
		throw std::runtime_error("Command has to start with character, no spaces allowed.");

    while (stream >> word)
	{
        words.push_back(word);
	}

    this->_command = words.front();
    words.erase(words.begin());
	if (words.empty())
		throw std::runtime_error("No arguments given.");
    if (getCommand() == "INVITE") //only when command is invite does irssi switch order in arguments. does nc do this too?
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
	try
	{
		parseStr(input);

		if (getCommand() == "NICK")
			client.setNickname(_channelName, _server);
		else if (getCommand() == "JOIN")
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
				if (targetChannel->findUser(client))
					targetChannel->broadcastMessage(message, client);
				else
					client.sendMessageToClient("You are not part of this channel");
			}
		}
	}
	catch (const std::exception& e) // Catch error
    {
        client.sendMessageToClient(e.what()); // Send error message to client
        std::cerr << "Error: " << e.what() << std::endl; // Log the error
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