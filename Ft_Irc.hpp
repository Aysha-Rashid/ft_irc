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
#include <string>
#include <signal.h>
#include <sstream>
#include <unistd.h>


#define ERR_NONICKNAMEGIVEN std::string ("431 :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME std::string ("432 :Erroneous Nickname\r\n")
#define ERR_NICKNAMEINUSE std::string ("433 :The requested nickname is already in use by another client\r\n")
#define ERR_NEEDMOREPARAMS std::string ("461 :Not enough parameters\r\n")
extern bool running;

#include "Client.hpp"
class Client;

class Server
{
    private:
        std::string         _serverName;
        std::string         _password;
        size_t              _port;
        int                 _socketFd;
        struct sockaddr_in	_address;
        socklen_t           _addrlen;
        fd_set              _readfds;
        int                 _maxfd;
    public:
        std::vector<Client *> clients;
        Server(std::string name);
        ~Server();
        size_t      getPort(void) const;
        std::string getPassword(void) const;
        std::string getServerName(void) const { return (_serverName);};
        fd_set      &getReadfds(void) { return(_readfds);};
        int         getSocket(void) const {return (_socketFd);};
        void        portAndPass(const std::string &port, std::string password);
        void        creatingServer(Server &server);
        void        acceptConnection(void);
        void        run(void);
        void        setFds(void);
};
