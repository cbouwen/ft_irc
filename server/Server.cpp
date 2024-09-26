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

void    Server::setPassword(char* password)
{
    _password = password;
}

const std::string&  Server::getPassword() const
{
    return _password;
}

void    Server::setPort(char *argv)
{
    for (size_t i = 0; i < strlen(argv); i++)
    {
        if (!isdigit(argv[i])) 
            throw(std::runtime_error("Port number isn't exclusively digits"));
    }
    int port = atoi(argv);
    if (port < 1024 || port > 49151)
        throw(std::runtime_error("Invalid port: Enter a number between 1024 and 49151"));
    this->_port = port;
}

//Static function so we can call on this from everywhere
void Server::SignalHandler(int signum)
{
    (void)signum;
    std::cout << std::endl << "Interrupt signal detected." << std::endl;
    Server::_signal = true;
}

void    Server::CloseFD() //Does fd get deleted from pollfd?
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        std::cout << "Client<" << _clients[i].getFD() << "> disconnected." << std::endl;
        close(_clients[i].getFD());//check comment below in ClearClient
    }
    if (_serverSocketFD != -1)
    {
        std::cout << "Server <" << _serverSocketFD << "> disconnected" << std::endl; //this is a blank print statement. Only print on success. Change it
        close(_serverSocketFD);
    }
}

void    Server::ClearClient(int fd)
{
    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
            _clients.erase(_clients.begin() + i);//Client disconnects correctly but on server shutdown, clients disconnects again? Look into it
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
    std::string         userData;

    incFD = accept(_serverSocketFD, (sockaddr *)&(clientAddr), &len);
    if (incFD == -1)
    {
        std::cout << "Client Accept failed" << std::endl;
        return ;
    }
    if (fcntl(incFD, F_SETFL, O_NONBLOCK) == -1) //This is for MACOS only. Should we remove this? I have no idea if it has any influence on any aspect of the program
    {
        std::cout << "fcntl() failed" << std::endl;
        return;
    }

    newPoll.fd = incFD;
    newPoll.events = POLLIN;
    newPoll.revents = 0;

    userData = receiveUserData(newPoll.fd);
    newClient.setFD(incFD);
    newClient.setIPaddr(inet_ntoa((clientAddr.sin_addr)));
    newClient.setUserData(userData);
    if (newClient.get != this->_password)
    {
        close(newClient);
        return;
    }

    _clients.push_back(newClient);
    _fds.push_back(newPoll);

    std::string welcomeMessage = ":serverhostname 001 " + newClient.getNickName() + " :Welcome to the IRC network, " + newClient.getNickName() + "!\r\n";
	newClient.sendMessageToClient(welcomeMessage);

    std::cout << "Client <" << incFD << "> Connected" << std::endl;

}

std::string	Server::receiveUserData(int &fd)
{
	std::string buffer;
	std::string str;
	bool user_received = false;
	size_t pos;
	
    while (!user_received)
    {
		buffer += readUserData(fd);

		while ((pos = buffer.find("\r\n")) != std::string::npos)
        {
			str += buffer.substr(0, pos); // Extract the complete message
			str += " ";
			buffer.erase(0, pos + 2); // Remove the processed message

			std::cout << "Current message: " << str << std::endl; //Testing: Think we can erase this yeah?

			if (str.find("USER") != std::string::npos)
            {
				user_received = true;
				break;
			}
		}

		if (user_received)
			std::cout << "Full USER command received: " << str << std::endl; //Testing purposes, fluff. Can let this in or remove it
	}
	return str;
}

std::string Server::readUserData(int &fd)
{
	char buffer[1024];
	int bytes_read = recv(fd, buffer, 1024, 0);
	if (bytes_read == 0)
	{
		close(fd);
		fd = -1;
		std::cout << "Client disconnected" <<std::endl;
	}
	else
	{
		buffer[bytes_read] = '\0';
		std::string msg(buffer);
		return msg;
	}
	return ("");
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