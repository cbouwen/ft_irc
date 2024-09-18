#include "Server.hpp"

Server::Server()
{
    _signal = false;
    _serverSocketFD = -1;    
}

Server::~Server()
{

}

//Static function so we can call on this from everywhere
void Server::SignalHandler(int signum)
{
    (void)signum;
    std::cout << "Interrupt signal detected." << std::endl;
    Server::_signal = true;
}

void    Server::ServerSocket()
{
    struct sockaddr_in  add;
    struct pollfd       newPoll;    

    add.sin_family = AF_INET;
    add.sin_port = htons(this->_port); //Should we be writing Getters and Setters for this project?
    add.sin_addr.s_addr = INADDR_ANY;

    _serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocketFD == -1)
        throw (std::runtime_error("failed to create socket"));

    int reuse = 1;
    if (setsockopt(_serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        throw(std::runtime_error("Failed to set option (SO_REUSEADDR) on socket"));    
    if (bind(_serverSocketFD, (struct sockaddr *)&add, sizeof(add)) == -1)
        throw(std::runtime_error("Failed to bind socket"));
    if (listen(_serverSocketFD, SOMAXCONN) == -1)
        throw(std::runtime_error("Listen() failed"));

    newPoll.fd = _serverSocketFD;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    _fds.push_back(newPoll);
}

void    Server::ServerInit()
{
    _port = 8000; //Can configure this to another port if needed. 8000 isn't occupied in DPG
    ServerSocket();

    std::cout << "Server and listening socket correctly set up" << std::endl;

}