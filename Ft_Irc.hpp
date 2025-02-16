#pragma once
#include <poll.h>
#include <iostream>
#include <exception>
#include <vector>
#include <cctype>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h> // (sockaddr_in) holds internet address
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <csignal>
// #include <fstream>
#include <unistd.h>

extern bool running;
class Client
{
    private:
        int _socketFd;
    public:
        Client() {}
        void setSocket(int socket) {this->_socketFd = socket;};
        int getSocket(void) const {return (_socketFd);};
    };
    
    class Server
{
    private:
        std::string         _serverName;
        std::string         _password;
        size_t              _port;
        int                 _socketFd;
        struct sockaddr_in	_address;
        socklen_t           _addrlen;
    public:
        std::vector<Client *> clients;
        Server(std::string name);
        ~Server();
        size_t getPort(void) const;
        std::string getPassword(void) const;
        void portAndPass(const std::string &port, std::string password);
        void creatingServer(Server &server);
        void acceptConnection(void);
        int getSocket(void) const {return (_socketFd);};
};
