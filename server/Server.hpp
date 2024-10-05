#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <netinet/in.h>
#include <sys/socket.h>
#include <csignal> //Do we want to add this?
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include "../client/Client.hpp"
#include "../channel/Channel.hpp"
#include "../commands/Command.hpp"



class Server
{
    private:
        int                         _port;
        int                         _serverSocketFD;
        static bool                 _signal;
        std::string                 _password;

        std::list<Client>         _clients;
        std::vector<Channel>        _channels;
        std::vector<struct pollfd>  _fds;

    public:
        Server();
        ~Server();

        const std::string&  getPassword() const;
        const std::vector<Channel>&  getChannels() const;
        const std::list<Client>&  getServerClients() const;
        std::vector<Channel>&  getChannels();
        Client* getClientByFD(int fd);
        Client* getClientByName(const std::string);

        void    setPassword(char* password);
        void    setPort(char* argv);

        void    ServerInit();
        void    ServerSocket();

        void    AcceptNewClient();
        void    ReceiveNewData(int fd);
        
 //       std::string receiveUserData(int &fd);
//        std::string readUserData(int &fd);

        Channel*    findChannel(std::string channelName);
//        void        sendPrivateMessage(std::string targetClient, Client& sender);

        void    CloseFD();
        void    ClearClient(int fd);
        
        static void    SignalHandler(int signum);

};