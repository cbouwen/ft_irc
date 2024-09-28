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

    std::cout <<  "Channel is " << this->getTopic() << std::endl;
    std::cout << std::endl << "Sender is " << sender.getNickName() << std::endl;

    int i = 0;
    for (std::vector<Client*>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        ++i;   
        std::cout << "Client " << i << " is " << (*it)->getNickName() << std::endl;
        if (*it != &sender)
            (*it)->sendMessageToClient(message);
    }
}