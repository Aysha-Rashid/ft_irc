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
        std::string _channel;
        bool authenticated;
        bool waitingForUsername;
        bool waitingForNickName;
        bool registered;
        bool joinChannel;

    public:
        std::string inputBuffer;
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
        bool getAuthenticated(void) const {return (this->authenticated);};
        bool getWaitingForUsername(void) const {return (this->waitingForUsername);};
        bool getWaitingForNickName(void) const {return (this->waitingForNickName);};
        bool getRegistered(void) const {return (this->registered);};
        bool getJoinChannel(void) const {return (this->joinChannel);};
        int  getSocket(void) const {return (_socketFd);};
        void setChannel(std::string channel) {this->_channel = channel;};
        void setJoinChannel(bool join) {this->joinChannel = join;};
        void setAuthenticated(bool authenticate) {this->authenticated = authenticate;};
        void setWaitingForNickName(bool nick) {this->waitingForNickName = nick;};
        void setWaitingForUsername(bool user) {this->waitingForUsername = user;};
        void setRegistered(bool regis) {this->registered = regis;};
        void setUserName(std::string &username) {this->_username = username;};
        void setRealName(std::string &realName) {this->_realName = realName;};
        void setNickName(std::string &nickName) {this->_nickName = nickName;};
        void setSocket(int socket) {this->_socketFd = socket;};
        bool isNickNameInUse(Server *server, const std::string& nickName);
        bool isValidNickName(const std::string& nickName);
        void broadcastMessage(Server *server, Client *curClient, const std::string &message);
        void printClientError(int socket, std::string Errmessage);
};
    