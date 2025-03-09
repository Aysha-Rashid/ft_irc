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
        std::string _channel;
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
        std::string getUserName(void) const {return (this->_username);};
        std::string getNickName(void) const {return (this->_nickName);};
        std::string getRealName(void) const {return (this->_realName);};
        std::string getChannel(void) const {return (this->_channel);};
        int  getSocket(void) const {return (_socketFd);};
        void setChannel(std::string &channel) {this->_channel = channel;};
        void setUserName(std::string &username) {this->_username = username;};
        void setRealName(std::string &realName) {this->_realName = realName;};
        void setNickName(std::string &nickName) {this->_nickName = nickName;};
        void setSocket(int socket) {this->_socketFd = socket;};
        void ClientCommunication(Server *server);
        bool isNickNameInUse(Server *server, const std::string& nickName);
        bool isValidNickName(const std::string& nickName);
        void broadcastMessage(Server *server, Client *curClient, const std::string &message);
        void printClientError(int socket, std::string Errmessage);
        int handleAuthentication(std::string message, Client **client, Server *server);
        void disconnected(Client *&client, int socket);
        int Commands(Client **client, int socket, std::string commands, Server *server);
};
    