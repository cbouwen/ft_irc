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