#include "Client.hpp"

Client::Client(std::string password) : _serverPassword(password)
{
	_nickNameSet = false;
	_userNameSet = false;
	_passwordMatch = false;
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

const std::string   Client::getPassword() const
{
    return _userPassword;
}

const std::string   Client::getNickName() const
{
    return _nickName;
}

const std::string   Client::getUserName() const
{
    return _userName;
}

const std::string   Client::getHostName() const
{
    return _hostName;
}

const std::string   Client::getFullName() const
{
    return _fullName;
}

bool	Client::isValid()
{
	if (_nickNameSet == false)
		return false;
	if (_userNameSet == false)
		return false;
	if (_passwordMatch == false)
		return false;
	return true;
}

void	Client::checkPassword(std::string password)
{
	if (_passwordMatch == true)
		return ;
	if (_serverPassword == password)
		_passwordMatch = true;
	else
	{
       std::string notice = ":localhost NOTICE * :Incorrect password\r\n";
        send(getFD(), notice.c_str(), notice.length(), 0);
	}
}

void	Client::setUserName(std::string userName)
{
	std::string oldUser = getUserName();
	_userName = userName;
	_userNameSet = true;
	sendMessageToClient(":" + oldUser + " USER " + userName);
}

void	Client::setNickName(std::string nickName)
{
	std::string oldNick = getNickName();
	_nickName = nickName;
	_nickNameSet = true;
	sendMessageToClient(":" + oldNick + " NICK " + nickName);
}

std::vector<std::string> Client::split(std::string str) //might be deletable
{
	std::vector<std::string> words;
	std::string word;
	std::stringstream stream(str);

	while (stream >> word)
		words.push_back(word);
	return words;
}

void    Client::setUserData(std::string userData) //might be deletable. keep it till finish
{
	std::vector<std::string> words = split(userData);	

	std::cout << userData << std::endl;

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
	fullName.erase(0, 1); //Why is this again?
	_fullName = fullName;

	std::cout << "Userdata: " << *this << std::endl;     //Comment back in for testing purposes to see if everything got parsed correctly
}

void    Client::sendMessageToClient(std::string message) const
{
    std::string buffer = message + "\r\n";
    if (send(this->getFD(), buffer.c_str(), buffer.size(), 0) < 0)
        throw std::runtime_error("Error while sending message to the client");
}

std::ostream& operator << (std::ostream &os,const Client & client)
{

	os << "Host name " << client.getHostName() << std::endl;
	os << "full name " << client.getFullName() << std::endl;
	os << "nick name " << client.getNickName() << std::endl;
	os << "user name " << client.getUserName() << std::endl;
	return os;
}