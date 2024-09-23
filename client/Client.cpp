#include "Client.hpp"

Client::Client()
{

}

Client::~Client()
{

}

int Client::getFD()
{
    return _fd;
}

void    Client::setFD(int fd)
{
    _fd = fd;
}

void    Client::setIPaddr(std::string IPaddr)
{
    _IPaddr = IPaddr;
}