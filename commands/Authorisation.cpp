#include "Command.hpp"

bool    Client::nicknameUnique(std::string newNickname, Server _server)
{
    for (std::list<Client>::const_iterator it = _server.getServerClients().begin(); it != _server.getServerClients().end(); ++it) 
    {
        if (it->getNickName() == newNickname)
            return false;
    }
    return true;
}

void    Command::checkPassword(Client& client, std::string userPassword, std::string serverPassword)
{
    if (userPassword == serverPassword)
    {
        client.sendMessageToClient("Password correct!");
        client.setPasswordMatch();
    }
    else
        client.sendMessageToClient("Password incorrect");
}

void    Client::setNickname(std::string newNickname, Server _server)
{
    int i = 1;
    std::string tempNickname = newNickname;
    while (nicknameUnique(tempNickname, _server) == false)
    {
        std::stringstream ss;
        ss << newNickname << i;
        tempNickname = ss.str();
        i++;
        if (i == 6)
            throw std::runtime_error("Already 5 variants of this nickname exist. Please choose another");
    }
    _nickName = tempNickname;
    sendMessageToClient("Nickname is set!");
    setNicknameSet();
}

void    Client::setUsername(std::string username, std::vector<std::string> words)
{
    if (words.size() < 3 )
        throw std::runtime_error("Add more arguments to USER");
    else
    {
    _userName = username;
	_hostName = *words.begin();

	words.erase(words.begin()); //delete hostname
	words.erase(words.begin()); //delete servername (we don't need to store this so just delete)
	
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
    setUsernameSet();
    sendMessageToClient("USER is set!");
    }
}