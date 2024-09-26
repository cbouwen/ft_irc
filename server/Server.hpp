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
#include <sstream>

#include "../client/Client.hpp"



class Server
{
    private:
        int                         _port;
        int                         _serverSocketFD;
        static bool                 _signal;
        std::string                 _password;

        std::vector<Client>         _clients;
        std::vector<struct pollfd>  _fds;

    public:
        Server();
        ~Server();

        const std::string&  getPassword() const;
        void    setPassword(char* password);
        void    setPort(char* argv);

        void    ServerInit();
        void    ServerSocket();

        void    AcceptNewClient();
        void    ReceiveNewData(int fd);
        
        std::string receiveUserData(int &fd);
        std::string readUserData(int &fd);


        void    CloseFD();
        void    ClearClient(int fd);
        
        static void    SignalHandler(int signum);

};