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
        std::string _username;
    public:
        bool authenticated;
        bool waitingForUsername;
        Client(){};
        Client(int socketFd) :_socketFd(socketFd) {}
        std::string getUserName(void) const {return (_username);};
        void setUserName(std::string username) {_username = username;};
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
        fd_set              _readfds;
        int                 _maxfd;
    public:
        std::vector<Client *> clients;
        Server(std::string name);
        ~Server();
        size_t      getPort(void) const;
        std::string getPassword(void) const;
        int         getSocket(void) const {return (_socketFd);};
        void        portAndPass(const std::string &port, std::string password);
        void        creatingServer(Server &server);
        void        acceptConnection(void);
        void        run(void);
        void        setFds(void);
        void        ClientCommunication(void);
        void        broadcastMessage(int sender, const std::string &message);
};
