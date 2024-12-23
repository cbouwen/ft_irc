//#pragma once

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <vector>
#include <algorithm>
#include <string>
#include <climits>

#include "../client/Client.hpp"

class Client;

class Channel
{
    private:
        std::vector<Client*>    _users;
        std::string             _topic;
        std::string             _topicName;
        std::vector<Client*>      _operators;
        std::vector<Client*>      _invitees;

        int                     _userLimit;
        bool                    _inviteOnly;
        bool                    _changeTopic;
        bool                    _channelPassword;
        std::string             _password;
    public:
        Channel();
        ~Channel();

        const std::string   getTopic() const;
        const std::string   getTopicName() const;
        int           getUserLimit() const;
        bool   getInviteOnly() const;
        bool   getChannelPassword() const;
        bool   getTopicPrivileges() const;
        const std::string   getPassword() const;
        const std::vector<Client*>   getUsers() const;
        void    setTopic(Client&, std::string);
        void    removeUser(Client); //

        void    setUp(std::string channelName);
        void    addUser(Client& client);
        bool    checkIsInvited(Client &client) const;
        bool    checkOperatorStatus(Client &client) const;

        bool    findUser(Client&);

        void    giveOperatorStatus(Client &client, Client*);
        void    removeOperatorStatus(Client &client, Client*);
        void    setInviteOnly(int a, Client& client);
        void    setTopicPrivileges(int a, Client& client);
        void    setChannelPassword(int a, Client& client, std::string*);
        void    setUserLimit(int a, Client& client, int limit);
        void    kickClient(Client& client, Client* targetClient);
        void    inviteClient(Client& client, Client* targetClient);

        void    broadcastMessageToAll(const std::string& message);
        void    broadcastMessage(const std::string& message, const Client& sender);
};

#endif