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



};