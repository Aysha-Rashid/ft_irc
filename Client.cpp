# include "Client.hpp"
# include <stdexcept>
# include <sys/socket.h>
# include <iostream>
# include <ctime>

Client::Client() : _fd(-1), _state(UNAUTHENTICATED), _channelCount(0), _lastActivity(time(NULL)) {}

Client::Client(int fd, std::string &ip) : _fd(fd), _ipAddress(ip), _state(UNAUTHENTICATED), _channelCount(0), _lastActivity(time(NULL)) {}

Client::Client(const Client &other)
{
        _fd = other._fd;
        _ipAddress = other._ipAddress;
        _nickname = other._nickname;
        _username = other._username;
        _realname = other._realname;
        _channelCount = other._channelCount;
        _state = other._state;
}

Client& Client::operator=(const Client &other)
{
    if(this != &other)
    {
        _fd = other._fd;
        _ipAddress = other._ipAddress;
        _nickname = other._nickname;
        _username = other._username;
        _realname = other._realname;
        _channelCount = other._channelCount;
        _state = other._state;
    }
    return(*this);
}

Client::~Client() {}

//getters
int Client::getSocketFd(void) const
{
    return (_fd);
}

std::string Client::getNickName(void)   const
{
    return (_nickname);
}

std::string Client::getUserName(void)   const
{
    return (_username);
}

std::string Client::getRealName(void)   const
{
    return (_realname);
}

std::string Client::getIpAddress(void)   const
{
    return (_ipAddress);
}

std::string Client::getPrefix(void)   const
{
    std::string username = _username.empty() ? "" : "!" + _username;
    return (_nickname + username);
}

int Client::getChannelCount(void)   const
{
    return (_channelCount);
}

clientState Client::getState(void)  const
{
    return (_state);
}

//setters
void Client::setSocketFd(int socketFd) 
{
    this->_fd = socketFd;
}


void    Client::setNickName(std::string &nickname)
{
    _nickname = nickname;
}

void    Client::setUserName(std::string &username)
{
    _username = username;
}

void    Client::setRealName(std::string &realname)
{
    _realname = realname;
}

void    Client::setState(clientState state)
{
    _state = state;
}

//member functions
void Client::write(const   std::string msg)    const
{
   if (this->getState() == REGISTERED)
       std::cout << this->getNickName() << ": " << msg;
   if(send(_fd, msg.c_str(), msg.size(), 0) < 0)
         throw std::runtime_error("Failed to send message.");
}



void Client::incrementChannelCount(void)
{
    _channelCount++;
}

void Client::decrementChannelCount(void)
{
    _channelCount--;
}

void Client::updateLastActivity()
{
    _lastActivity = time(NULL);
}

bool Client::isInactive(time_t timeout) const
{
    time_t currentTime = time(NULL);
    return (currentTime - _lastActivity) > timeout;
}

time_t Client::getLastActivity() const
{
    return _lastActivity;
}