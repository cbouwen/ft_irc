#include "Command.hpp"

bool    Command::nicknameUnique(std::string newNickname)
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

void    Client::setNickname(std::string newNickname)
{

    std::cout << "nickname test: " << newNickname << std::endl;

}

void    Client::setUsername(std::vector<std::string> arguments)
{
    (void)arguments;
//    std::cout << arguments << std::endl;
}