//#pragma once

#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <string>
#include <sstream>

#include "../server/Server.hpp"
#include "../channel/Channel.hpp"

class Server;

class Command : public Server, public Channel
{
    private:
        std::string                 _command;
        std::vector<std::string>    _arguments;

        Server&      _server;  //reference to the server so we can easily adjust
        std::string                 _channelName;

    public:
        Command(Server& server,std::string init) : _server(server), _channelName(init) {} ;
        ~Command();

        const std::string getChannelName() const;
        const std::string getCommand() const;
        const std::vector<std::string> getArguments() const;

        void    parseStr(std::string str);
        void    parseCMD(std::string input, Client& client);
        void    getAuthenticated(std::string input, Client& client);

        void    addPrivileges(Client& client);
        void    removePrivileges(Client& client);

        void     executeKick(Client& client, std::string targetClientName);
        void     executeInvite(Client& client, std::string targetClientName);

        bool    targetIsUser();
        void    joinChannel(Client& client);

        void    handleTopic(Client&);

        void    checkPassword(Client&, std::string, std::string);
};

std::ostream& operator <<(std::ostream& os, const Command& command);

#endif