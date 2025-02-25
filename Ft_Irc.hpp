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
class Server;
class Client
{
    private:
        int _socketFd;
        std::string _username;
        std::string _nickName;
        std::string _realName;
    public:
        bool authenticated;
        bool waitingForUsername;
        bool waitingForNickName;
        Client(){};
        Client(int socketFd) :_socketFd(socketFd) {}
        std::string getUserName(void) const {return (_username);};
        std::string getNickName(void) const {return (_nickName);};
        std::string getRealName(void) const {return (_realName);};
        void setUserName(std::string username) {_username = username;};
        void setRealName(std::string realName) {_realName = realName;};
        void setNickName(std::string nickName) {_nickName = nickName;};
        void setSocket(int socket) {this->_socketFd = socket;};
        int  getSocket(void) const {return (_socketFd);};
        void ClientCommunication(Server *server);
        bool isNickNameInUse(Server *server, const std::string& nickName);
        bool isValidNickName(const std::string& nickName);
        void broadcastMessage(Server *server, int sender, const std::string &message);
        void printClientError(int socket, std::string Errmessage);
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
        fd_set      &getReadfds(void) { return(_readfds);};
        int         getSocket(void) const {return (_socketFd);};
        void        portAndPass(const std::string &port, std::string password);
        void        creatingServer(Server &server);
        void        acceptConnection(void);
        void        run(void);
        void        setFds(void);
};
