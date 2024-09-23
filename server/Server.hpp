#pragma once

#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <csignal> //Do we want to add this?
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include "../client/Client.hpp"



class Server
{
    private:
        int                         _port;
        int                         _serverSocketFD;
        static bool                 _signal;

        std::vector<Client>         _clients;
        std::vector<struct pollfd>  _fds;

    public:
        Server();
        ~Server();

        void    setPort(char* argv);

        void    ServerInit();
        void    ServerSocket();

        void    AcceptNewClient();
        void    ReceiveNewData(int fd);

        void    CloseFD();
        void    ClearClient(int fd);
        
        static void    SignalHandler(int signum);

};