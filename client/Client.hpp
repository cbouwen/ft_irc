#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>


class Client
{
    private:
        int         _fd;
        std::string _nickName;
        std::string _userName;
        std::string _hostName;
        std::string _userPassword;
        std::string _IPaddr;
        std::string _fullName;
        bool        _authorized;

    public:
        Client();
        ~Client();

        int         getFD() const;

        void        setFD(int fd);
        void        setAuthorized();
        void        setIPaddr(std::string IPaddr);
        const std::string   getNickName() const;
        const std::string   getUserName() const;
        const std::string   getPassword() const;
        const std::string   getHostName() const;
        const std::string   getFullName() const;
        bool                getAuthorized();

        std::vector<std::string> split(std::string str);
        void    setUserData(std::string userData);

        void    sendMessageToClient(std::string message) const;
};

std::ostream& operator << (std::ostream& os, const Client& client);