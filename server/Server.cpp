#include "Server.hpp"
//#include "../client/Client.hpp"
//included in header

bool    Server::_signal = false;
Server::Server()
{
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

void    Server::CloseFD()
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        std::cout << "Client<" << _clients[i].getFD() << "> disconnected." << std::endl;
        close(_clients[i].getFD());
    }
    if (_serverSocketFD != -1)
    {
        std::cout << "Server <" << _serverSocketFD << "> disconnected" << std::endl;
        close(_serverSocketFD);
    }
}

void    Server::ClearClient(int fd)
{
    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
            _clients.erase(_clients.begin() + i);
        break;
    }
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
                    //maybe make the port a macro to easily find and change it?
    ServerSocket();

    std::cout << "Test: Server and listening socket correctly set up" << std::endl; //Change the message

    while (Server::_signal == false)
    {
        if ((poll(&_fds[0], _fds.size(), -1) == -1) && Server::_signal == false)
            throw (std::runtime_error("poll() failed"));
        
        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _serverSocketFD)
                    AcceptNewClient();
                else
                    ReceiveNewData(_fds[i].fd);
            }
        }
    }
    CloseFD();
}

void    Server::AcceptNewClient()
{
    Client              newClient;
    struct sockaddr_in  clientAddr;
    struct pollfd       newPoll;
    socklen_t           len = sizeof(clientAddr);
    int                 incFD;
    
    incFD = accept(_serverSocketFD, (sockaddr *)&(clientAddr), &len);
    if (incFD == -1)
    {
        std::cout << "Client Accept failed" << std::endl;
        return ;
    }
    if (fcntl(incFD, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cout << "fcntl() failed" << std::endl;
        return;
    }

    newPoll.fd = incFD;
    newPoll.events = POLLIN;
    newPoll.revents = 0;

    newClient.setFD(incFD);
    newClient.setIPaddr(inet_ntoa((clientAddr.sin_addr)));
    _clients.push_back(newClient);
    _fds.push_back(newPoll);

    std::cout << "Client <" << incFD << "> Connected" << std::endl;

}

void    Server::ReceiveNewData(int fd)
{
    char    buffer[1024];
    ssize_t bytes;

    memset(buffer, 0, sizeof(buffer));
    bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0 )
    {
        std::cout << "Client <" << fd << "> disconnected" << std::endl;
        ClearClient(fd);
        close(fd);
    }
    else
    {
        buffer[bytes] = '\0';
        std::cout << "Client <" << fd << "> Data: " << buffer;
        //Parse
        //Check data
        //Handle commands
        //ETC
    }
}