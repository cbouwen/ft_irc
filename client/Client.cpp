#include "Client.hpp"

Client::Client()
{

}

Client::~Client()
{

}

int Client::getFD() const
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

const std::string   Client::getNickName() const
{
    return _nickName;
}

const std::string   Client::getUserName() const
{
    return _userName;
}

std::vector<std::string> Client::split(std::string str)
{
	std::vector<std::string> words;
	std::string word;
	std::stringstream stream(str);

	while (stream >> word)
		words.push_back(word);
	return words;
}

void    Client::setUserData(std::string userData)
{
	std::vector<std::string> words = split(userData);	

	while (words.front().compare("NICK") != 0)
		words.erase(words.begin());
	words.erase(words.begin());
	_nickName = *words.begin();

	words.erase(words.begin());
	words.erase(words.begin());
	_userName = *words.begin();

	words.erase(words.begin());
	_hostName = *words.begin();

	words.erase(words.begin());
	words.erase(words.begin());//1 extra to skip IP address, guide for IRC had this covered pretty well so don't want to implement it here
	
	std::string fullName = *words.begin();
	words.erase(words.begin());
	while (!words.empty())
	{
		fullName += " ";
		fullName += *words.begin();
		words.erase(words.begin());
	}
	fullName.erase(0, 1);
	_fullName = fullName;
}

void    Client::sendMessageToClient(std::string message) const
{
    std::string buffer = message + "\r\n";
    if (send(this->getFD(), buffer.c_str(), buffer.size(), 0) < 0)
        throw std::runtime_error("Error while sending message to the client");
}