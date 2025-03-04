#pragma once
#include "Ft_Irc.hpp"

class Server;
class Client
{
    private:
        int _socketFd;
        std::string _username;
        std::string _nickName;
        std::string _realName;
        std::string inputBuffer;
        std::string channel;
        bool authenticated;
        bool waitingForUsername;
        bool waitingForNickName;
        bool registered;
        bool joinChannel;
        int out;

    public:
        Client()
        {
            this->authenticated = false;
            this->waitingForNickName = false;
            this->waitingForUsername = false;
            this->registered = false;
            this->joinChannel = false;
        };
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
        int handleAuthentication(std::string message, Client **client, Server *server);
        void disconnected(Client *&client, int socket);
};
    