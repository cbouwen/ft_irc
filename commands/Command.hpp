#pragma once

#include <string>
#include <sstream>

#include "../server/Server.hpp"

class Server;

class Command
{
    private:
        std::string _channelName;
        std::string _command;
        std::string _arguments;

    public:
        Command();
        ~Command();

        const std::string getChannelName() const;
        const std::string getCommand() const;
        const std::string getArguments() const;


        void    parseStr(std::string str);
        void    parseCMD(std::string input, Server& server);
};

std::ostream& operator <<(std::ostream& os, const Command& command);