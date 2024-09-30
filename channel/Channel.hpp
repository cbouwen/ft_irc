#pragma once

#include <vector>
#include <algorithm>

#include "../client/Client.hpp"

class   Client;
class Channel
{
    private:
        std::vector<Client*>    _users;
        std::string             _topic;
        std::vector<Client*>      _operators;

        bool                    _inviteOnly;
        bool                    _changeTopic;
        bool                    _channelPassword;
        std::string             _password;
    public:
        Channel();
        ~Channel();

        const std::string   getTopic() const;
        bool   getInviteOnly() const;
        bool   getChannelPassword() const;
        const std::string   getPassword() const;
        const std::vector<Client*>   getUsers() const;

        void    setUp(std::string channelName);
        void    addUser(Client& client);
        bool    checkOperatorStatus(Client &client) const;

        void    giveOperatorStatus(Client &client, Client*);
        void    removeOperatorStatus(Client &client, Client*);
        void    setInviteOnly(int a, Client& client);
        void    setTopicPrivileges(int a, Client& client);
        void    setChannelPassword(int a, Client& client, std::string);
        void    broadcastMessage(const std::string& message, const Client& sender);

};