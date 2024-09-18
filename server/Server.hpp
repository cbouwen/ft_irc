#pragma once

#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <csignal> //Do we want to add this?
#include <poll.h>

#include "../client/Client.hpp"



class Server
{
    private:
        int _port;
        int _serverSocketFD;
        static bool _signal;
        std::vector<Client>         _clients;
        std::vector<struct pollfd>  _fds;

    public:
        Server();
        ~Server();

        void    ServerInit();
        void    ServerSocket();
        void    closeFD(); 
        
        static void    SignalHandler(int signum);

};