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

std::vector<Channel>&  Server::getChannels()
{
    return _channels;
}

const std::vector<Channel>&  Server::getChannels() const
{
    return _channels;
}

const std::list<Client>&  Server::getServerClients() const
{
    return _clients;
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

Channel*    Server::findChannel(std::string channelName)
{
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (it->getTopic() == channelName)
            return &(*it);
    }
    throw (std::runtime_error("Null return at channel"));
}

//Static function so we can call on this from everywhere
void Server::SignalHandler(int signum)
{
    (void)signum;
    std::cout << std::endl << "Interrupt signal detected." << std::endl;
    Server::_signal = true;
}

void    Server::CloseFD()
{
    for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        std::cout << "Client<" << it->getFD() << "> disconnected." << std::endl;
        close(it->getFD());
    }
    if (_serverSocketFD != -1)
    {
        std::cout << "Server <" << _serverSocketFD << "> disconnected" << std::endl;
        close(_serverSocketFD);
    }
}

void Server::removeFromChannels(Client client)
{
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        it->removeUser(client);
    }
}

void    Server::ClearClient(int fd)
{
    for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getFD() == fd)
        {
            removeFromChannels(*it);
            close(it->getFD());
            it = _clients.erase(it);
            break;
        } 
    }
    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds.at(i).fd == fd)
        {
            _fds.erase(_fds.begin() + i);  // Remove the pollfd struct using an iterator
            break;  // Exit loop once the fd is removed
        }
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
        if ((poll(&_fds.at(0), _fds.size(), -1) == -1) && Server::_signal == false)
            throw (std::runtime_error("poll() failed"));
        
        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds.at(i).revents & POLLIN)
            {
                if (_fds.at(i).fd == _serverSocketFD)
                    AcceptNewClient();
                else
                    ReceiveNewData(_fds.at(i).fd);
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

    try
    {
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

    newClient.setFD(incFD);
    newClient.setIPaddr(inet_ntoa((clientAddr.sin_addr)));
    userData = receiveUserData(newPoll.fd);
    //nc will get stuck in the above here. If we can find a way to recognize nc here and then are able to break off here and go to a new function like authenticateNCclient then all the problems are solved
    if (userData != "NC")
    {
        newClient.setUserData(userData, *this);
        if (newClient.getPassword() != this->_password)
        {
            std::string msg = "Incorrect password: " + newClient.getPassword() + "\r\n";
            newClient.sendMessageToClient(msg);
            close(incFD);
            return;
        }
        std::string welcomeMessage = ":serverhostname 001 " + newClient.getNickName() + " :Welcome to the IRC network, " + newClient.getNickName() + "!\r\n";
	    newClient.sendMessageToClient(welcomeMessage);
        newClient.setAuthorized();
    }
    else 
    {
    //    std::string welcomeMessage = "Welcome to the IRC network!\r\n"; //is this part of the server handshake? Maybe delete this and just ask for authorization
	//    newClient.sendMessageToClient(welcomeMessage);
        std::string authorizeMessage = "Please get authorized by setting NICK, USER and PASS\r\n";
	    newClient.sendMessageToClient(authorizeMessage);
    }
    _clients.push_back(newClient);
    _fds.push_back(newPoll);


    std::cout << "Client <" << incFD << "> Connected" << std::endl;
    }
    catch (const std::exception& e) // Catch error
    {
        newClient.sendMessageToClient(e.what()); // Send error message to client
        std::cerr << "Error: " << e.what() << std::endl; // Log the error

    }
}

std::string	Server::receiveUserData(int &fd)
{
	std::string buffer;
	std::string str;
	bool user_received = false;
	size_t pos;
    int     i = 0;

	buffer += readUserData(fd);
    while (!user_received) //Keeps parsing until str contains "USER". IRSSI does this everytime, NC does not
    {
        i++; //to measure the timeout to find nc
		while ((pos = buffer.find("\r\n")) != std::string::npos)
        {
			str += buffer.substr(0, pos); // Extract the complete message
			str += " ";
			buffer.erase(0, pos + 2); // Remove the processed message

//			std::cout << "Current message: " << str << std::endl; //Testing: Think we can erase this yeah?

			if (str.find("USER") != std::string::npos)
            {
				user_received = true;
				break;
			}
		}
		buffer += readUserData(fd);
        if (i > 100) //timeout
            return "NC";
//		if (user_received)
//			std::cout << "Full USER command received: " << str << std::endl; //Testing purposes, fluff. Can let this in or remove it
	}
	return str;
}

std::string Server::readUserData(int &fd)
{
	char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
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

Client* Server::getClientByFD(int fd)
{
    for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getFD() == fd)
            return &(*it);
    }
    std::cout << "Null return at getClientbyFD" << std::endl;
    return NULL;
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
        class Command cmd(*this, "");
        Client* client = getClientByFD(fd); //error handling or okay? FT can't be called if fd does not exist in vector
        buffer[bytes] = '\0';
        std::cout << "Client <" << fd << "> Data: " << buffer; //Think we can remove this or at least change it. getNickname() instead of Client <fd>
        if (client->getAuthorized() == false)    
            cmd.getAuthenticated(buffer, *client);
        else
            cmd.parseCMD(buffer, *client);
    }
}

Client*    Server::getClientByName(const std::string targetClient)
{
    for (std::list<Client>::iterator it = _clients.begin(); it !=_clients.end(); it++)
    {
        if (it->getNickName() == targetClient)
            return &(*it);
    }
    std::cout << "Null return at getClientbyName" << std::endl;
    return NULL;
}
