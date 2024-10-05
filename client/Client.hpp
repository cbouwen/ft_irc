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

        bool        _nickSet;
        bool        _userSet;
        bool        _passwordCorrect;

    public:
        Client();
        ~Client();

        int         getFD() const;

        void        setFD(int fd);
        void        setIPaddr(std::string IPaddr);
        const std::string   getNickName() const;
        const std::string   getUserName() const;
        const std::string   getPassword() const;
        const std::string   getServerPassword() const;
        const std::string   getHostName() const;
        const std::string   getFullName() const;
        void                setNickName(std::string);
        void                setServerPassword(std::string);
        void                setNickSet();
        void                setUserSet();
        void                setPasswordCorrect();

        std::vector<std::string> split(std::string str);
        void    setUserData(std::string userData);

        bool    isValidated();
        void    sendMessageToClient(std::string message) const;
};

std::ostream& operator << (std::ostream& os, const Client& client);