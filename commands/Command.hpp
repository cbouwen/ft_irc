//#pragma once

#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <string>
#include <set>
#include <sstream>

#include "../server/Server.hpp"
#include "../channel/Channel.hpp"
#include "../client/Client.hpp"

class Client;
class Server;

class Command
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

        std::string&    checkCommand(std::string& command);
        std::string&    checkChannel(std::string& input);
        void            parseStr(std::string str);
        void            parseCMD(std::string input, Client& client);
        void            getAuthenticated(std::string input, Client& client);

        void            addPrivileges(Client& client);
        void            removePrivileges(Client& client);

        void            executeKick(Client& client, std::string targetClientName);
        void            executeInvite(Client& client, std::string targetClientName);

        bool            isValidInteger(const std::string&);
        bool            targetIsUser();
        void            joinChannel(Client& client);

        void            handleTopic(Client&);

        void            checkPassword(Client&, std::string, std::string);
};

std::ostream& operator <<(std::ostream& os, const Command& command);

#endif
