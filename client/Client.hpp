//#pragma once

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <algorithm>

#include "../server/Server.hpp"

class Server;

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

        bool        _passwordMatch;
        bool        _nicknameSet;
        bool        _usernameSet;

        bool        _authorized;

    public:
        Client();
        ~Client();

        int         getFD() const;

        void        setFD(int fd);
        void        setAuthorized();
        void        setNickname(std::string, Server);
        void        setUsername(std::string, std::vector<std::string>);
//        void        setUsername();
        void        setIPaddr(std::string IPaddr);
        void        setPasswordMatch();
        void        setUsernameSet();
        void        setNicknameSet();


        const std::string   getNickName() const;
        const std::string   getUserName() const;
        const std::string   getPassword() const;
        bool                getPasswordMatch() const;
        const std::string   getHostName() const;
        const std::string   getFullName() const;
        bool                getAuthorized();
        bool                checkAuthorized();
        bool                findPass(const std::vector<std::string>&);

        std::vector<std::string> split(std::string str);
        void    setUserData(std::string userData, Server _server);

        bool    nicknameUnique(std::string newNickname, Server _server);
        void    sendMessageToClient(std::string message) const;
};

std::ostream& operator << (std::ostream& os, const Client& client);

#endif