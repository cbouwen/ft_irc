#pragma once

#include <string>


class Client
{
    private:
        int         _fd;
        std::string _IPaddr;

    public:
        Client();
        ~Client();

        int         getFD();

        void        setFD(int fd);
        void        setIPaddr(std::string IPaddr);

};