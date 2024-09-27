#pragma once

#include <vector>

#include "../client/Client.hpp"


class Channel
{
    private:
        std::vector<Client*>    _users;
        std::string             _topic;


    public:
        Channel();
        ~Channel();

        const std::string   getTopic() const;

        void    setUp(std::string channelName);
        void    addUser(Client& client);
};