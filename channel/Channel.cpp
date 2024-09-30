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

bool   Channel::getInviteOnly() const
{
    return _inviteOnly;
}

bool   Channel::getChannelPassword() const
{
    return _channelPassword;
}

bool   Channel::getTopicPrivileges() const
{
    return _changeTopic;
}

const std::string   Channel::getPassword() const
{
    return _password;
}

const std::string   Channel::getTopicName() const
{
    return _topicName;
}

const std::string   Channel::getTopic() const
{
    return _topic;
}

void    Channel::setUp(std::string channelName)
{
    _topic = channelName;
    _topicName = channelName;
    _inviteOnly = false;
    _changeTopic = false;
    _channelPassword = false;
}

void    Channel::setTopic(Client& client, std::string topicName)
{
    std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + this->_topic + " ";
    if (getTopicPrivileges())
    {
        if (!checkOperatorStatus(client))
        {
            message += "You don't have operator privileges";
            client.sendMessageToClient(message);
            return ;
        }
    } 
    _topicName = topicName;
    message = "The topic for " + getTopic() + " has been set to " + topicName;
    broadcastMessage(message, client);
}


bool    Channel::checkOperatorStatus(Client& client) const
{
    for (size_t i = 0; i < _operators.size(); i++)
    {
        if (_operators[i]->getFD() == client.getFD())
            return true;
    }
    return false;
}

void    Channel::removeOperatorStatus(Client& operatorClient, Client* targetClient) //What if none of clients are members of channel?
{
    std::string message = ":" + operatorClient.getNickName() + "!" + operatorClient.getUserName() + "@" + operatorClient.getHostName() + " PRIVMSG " + this->_topic + " ";
    if (targetClient == NULL)
    {
        message += "Target client is invalid or not a member of the channel";
        operatorClient.sendMessageToClient(message);
        return ;
    }
    if (!checkOperatorStatus(operatorClient))
    {
        message += "You don't have operator privileges";
        operatorClient.sendMessageToClient(message);
    }
    else if (!checkOperatorStatus(*targetClient))
    {
        message += "Target does not have operator privileges";
        operatorClient.sendMessageToClient(message);
    }
    else
    {
        std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), targetClient);
        if (it != _operators.end())
        {
            _operators.erase(it);
            std::cout << "Client " << targetClient->getNickName() << " no longer has operator privileges on: " << getTopic() << std::endl;
        }
    }
}

void    Channel::giveOperatorStatus(Client& operatorClient, Client* targetClient) //What if none of clients are members of channel?
{
    std::string message = ":" + operatorClient.getNickName() + "!" + operatorClient.getUserName() + "@" + operatorClient.getHostName() + " PRIVMSG " + this->_topic + " ";
    if (targetClient == NULL)
    {
        message += "Target client is invalid or not a member of the channel";
        operatorClient.sendMessageToClient(message);
        return ;
    }
    if (!checkOperatorStatus(operatorClient))
    {
        message += "You don't have operator privileges";
        operatorClient.sendMessageToClient(message);
    }
    else if (checkOperatorStatus(*targetClient))
    {
        message += "Target is already an operator";
        operatorClient.sendMessageToClient(message);
    }
    else
    {
        _operators.push_back(targetClient);
        std::cout << "Client " << _operators.back()->getNickName() << " is now operator of channel: " << getTopic() <<  std::endl;
    }
}

void    Channel::setTopicPrivileges(int a, Client& client)
{
    std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + this->_topic + " ";
    if (!checkOperatorStatus(client))
    {
        message += "You don't have operator privileges";
        client.sendMessageToClient(message);
    }
    else
    {
        if (a == 0)
        {
            _changeTopic = false;
            message += " Every user can now set the topic ";
        client.sendMessageToClient(message);
        }
        else if (a == 1)
        {
            _changeTopic = true;
            message += " Only operators can now set the topic";
            client.sendMessageToClient(message);
        }
    }
}

void    Channel::setInviteOnly(int a, Client& client)
{
    std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + this->_topic + " ";
    if (!checkOperatorStatus(client))
    {
        message += "You don't have operator privileges";
        client.sendMessageToClient(message);
    }
    else
    {
    if (a == 0)
    {
        _inviteOnly = false;
        message += this->getTopic() + " is open for everyone to join";
        client.sendMessageToClient(message);
    }
    else if (a == 1)
    {
        _inviteOnly = true;
        message += this->getTopic() + " is now invite only";
        client.sendMessageToClient(message);
    }
    }
}

void    Channel::setChannelPassword(int a, Client& client, std::string* password)
{
    std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + this->_topic + " ";
    if (!checkOperatorStatus(client))
    {
        message += "You don't have operator privileges";
        client.sendMessageToClient(message);
    }
    else
    {
        if (a == 0)
        {
          _channelPassword = false;
          message += this->getTopic() + " is no longer restricted by a password";
          client.sendMessageToClient(message);
        }
        else if (a == 1)
        {
            if (!password)
            {
                message += "Argument for password missing" ;
                client.sendMessageToClient(message);
                return ;
            }
            _channelPassword = true;
            _password = *password;
            message += this->getTopic() + " is now restricted by a password";
            client.sendMessageToClient(message);
        }
    }
}

void    Channel::addUser(Client& client)
{
    _users.push_back(&client);
    if (_operators.empty())
    {
        _operators.push_back(&client); //Is this best way to do this? 
        std::cout << "Client " << (*_operators.begin())->getNickName() << " is now operator of channel: " << getTopic() <<  std::endl;
    }
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