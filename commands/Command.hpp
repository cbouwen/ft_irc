#pragma once

#include <string>
#include <sstream>

#include "../server/Server.hpp"
#include "../channel/Channel.hpp"

class Server;

class Command
{
    private:
        std::string _channelName;
        std::string _command;
        std::string _arguments;

        Server&      _server;  //reference to the server so we can easily adjust

    public:
        Command(Server& server) : _server(server) {} ;
        ~Command();

        const std::string getChannelName() const;
        const std::string getCommand() const;
        const std::string getArguments() const;

        void    parseStr(std::string str);
        void    parseCMD(std::string input, Client& client);

        bool    targetIsUser();
        void    joinChannel(Client& client);
};

std::ostream& operator <<(std::ostream& os, const Command& command);