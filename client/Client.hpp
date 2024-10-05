#pragma once

#include <string>
#include <algorithm>
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
        std::string _serverPassword;

        bool        _nickNameSet;
        bool        _userNameSet;
        bool        _passwordMatch;
    public:
        Client(std::string);
        ~Client();

        int         getFD() const;

        void        setFD(int fd);
        void        setIPaddr(std::string IPaddr);
        const std::string   getNickName() const;
        const std::string   getUserName() const;
        const std::string   getPassword() const;
        const std::string   getHostName() const;
        const std::string   getFullName() const;
        void                setNickName(std::string);
        void                setUserName(std::string);
        void                checkPassword(std::string);

        bool                isValid();
        std::vector<std::string> split(std::string str);
        void    setUserData(std::string userData);

        void    sendMessageToClient(std::string message) const;
};

std::ostream& operator << (std::ostream& os, const Client& client);