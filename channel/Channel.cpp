#include "Channel.hpp"

Channel::Channel()
{

}

Channel::~Channel()
{

}

const std::vector<Client*>   Channel::getUsers() const
{
    return _users;
}

const std::string   Channel::getTopic() const
{
    return _topic;
}

void    Channel::setUp(std::string channelName)
{
    _topic = channelName;
}

void    Channel::addUser(Client& client)
{
    _users.push_back(&client);
    std::cout << "Client " << client.getNickName() << " added to channel: " << getTopic() <<  std::endl;
}

void    Channel::broadcastMessage(const std::string& message, const Client& sender)
{
    std::string channelMsg = ":" + sender.getNickName() + "!" + sender.getUserName() + "@" + sender.getHostName() + " PRIVMSG " + this->_topic + " " + message;

    for (unsigned int i = 0; i < _users.size(); ++i)
    {
        if (_users[i]->getNickName() != sender.getNickName())
            _users[i]->sendMessageToClient(channelMsg);
    }
}