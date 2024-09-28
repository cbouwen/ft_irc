#include "Channel.hpp"

Channel::Channel()
{

}

Channel::~Channel()
{

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