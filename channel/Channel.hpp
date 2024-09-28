#pragma once

#include <vector>

#include "../client/Client.hpp"

class   Client;
class Channel
{
    private:
        std::vector<Client*>    _users;
        std::string             _topic;


    public:
        Channel();
        ~Channel();

        const std::string   getTopic() const;
        const std::vector<Client*>   getUsers() const;

        void    setUp(std::string channelName);
        void    addUser(Client& client);

        void    broadcastMessage(const std::string& message, const Client& sender);

};