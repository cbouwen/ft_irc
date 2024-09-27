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

const std::string   Command::getArguments() const
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
    
    this->_command = *words.begin();
    words.erase(words.begin());
    if (getCommand() == "INVITE") //only when command is invite does irssi switch order in arguments. Check reference below
    {
        this->_arguments = *words.begin(); 
        words.erase(words.begin());
        this->_channelName = *words.begin();
    }
    else
    {
        this->_channelName = *words.begin();
        words.erase(words.begin());
        for (size_t i = 0; i < words.size(); ++i)
        {
            if (i > 0) this->_arguments += " ";
                this->_arguments += words[i];   //This starts the argument list with ':'. We can keep this and see in the future or work around it
        }
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
    else if (getCommand() == "MODE")
        executeMode();
    else if (getCommand() == "INVITE")
        executeInvite();
    else if (getCommand() == "PRIVMSG") //if no command is parsed, command is set to PRIVMSG meaning sending msgs
        sendMsg();
*/
}

void    Command::joinChannel(Client& client)
{
    Channel* existingChannel = NULL;

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
        newChannel.setUp(getChannelName()); //let us set this up when all of the above works
        _server.getChannels().push_back(newChannel);
        existingChannel = &_server.getChannels().back();
    }
    
    existingChannel->addUser(client); //write addUser, find a way to add client

    std::cout << "Succesfully added the channel -" << _server.getChannels().back().getTopic() << "- to vector channel in server class" << std::endl; //Errors here: after joining different channel, output remained inconsistent about channelname
    std::cout << "Succesfully added user -" << client.getNickName() << "- to -" << existingChannel->getTopic() << std::endl; //
}

std::ostream& operator << (std::ostream &os,const Command& command)
{

	os << "Channel name: " << command.getChannelName() << std::endl;
	os << "Command: " << command.getCommand() << std::endl;
	os << "Arguments: " << command.getArguments() << std::endl;
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