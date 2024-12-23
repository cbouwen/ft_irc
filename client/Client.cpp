#include "Client.hpp"

Client::Client()
{
	_passwordMatch = false;
	_nicknameSet = false;
	_usernameSet = false;
	_authorized = false;
}

Client::~Client()
{

}

int Client::getFD() const
{
    return _fd;
}

void    Client::setAuthorized()
{
    _authorized = true;
}

bool	Client::checkAuthorized()
{
	if (_passwordMatch == false)
		return false;
	if (_nicknameSet == false)
		return false;
	if (_usernameSet == false)
		return false;
	return true;
}

//This is now part of init authorisation. let's see if this matches.
/*
void	Client::setNickname(std::string newNickname)
{
	_nickName = newNickname;
}
*/

void    Client::setFD(int fd)
{
    _fd = fd;
}

void    Client::setIPaddr(std::string IPaddr)
{
    _IPaddr = IPaddr;
}

void    Client::setPasswordMatch()
{
    _passwordMatch = true;
}

void    Client::setUsernameSet()
{
    _usernameSet = true;
}

void    Client::setNicknameSet()
{
    _nicknameSet = true;
}

const std::string   Client::getPassword() const
{
    return _userPassword;
}

bool   Client::getPasswordMatch() const
{
    return _passwordMatch;
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

bool	Client::getAuthorized()
{
	return _authorized;
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


bool Client::findPass(const std::vector<std::string>& words)
{
    return std::find(words.begin(), words.end(), "PASS") != words.end();
}

void    Client::setUserData(std::string userData, Server _server)
{
	std::vector<std::string> words = split(userData);	


//Current message: CAP LS 
//Current message: CAP LS PASS test 
//Current message: CAP LS PASS test NICK cbouwen 
//Current message: CAP LS PASS test NICK cbouwen USER cbouwen cbouwen localhost :Cedric Bouwen 
//Full USER command received: CAP LS PASS test NICK cbouwen USER cbouwen cbouwen localhost :Cedric Bouwen 

//	std::cout << "testuserdata if" << std::endl;
//	if (words.find("PASS") != std::string::npos)
//		return; 
//	std::cout << "testuserdata after if" << std::endl;

	if (!findPass(words))
		throw std::runtime_error("No password given");
	while (words.front().compare("PASS") != 0) //Concerned about error handling here. What happens when we can't find "PASS"? Client dc's and no problem?
		words.erase(words.begin());
	words.erase(words.begin());
	_userPassword = *words.begin();

	words.erase(words.begin()); //extra skip. Don't really understand why but nickname got set as NICK if we didn't do this. Hey, it works.
	words.erase(words.begin());
	setNickname(*words.begin(), _server); ///////////////////////////

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

//	std::cout << "Userdata: " << *this << std::endl;     Comment back in for testing purposes to see if everything got parsed correctly
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