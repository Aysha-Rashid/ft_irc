# include "Client.hpp"

Client::Client() {}

Client::Client(int fd,std::string &ip, std::string  &hostname) 
    :_fd(fd), _ipAddress(ip),_hostname(hostname),_channelCount(0),_state(UNAUTHENTICTED) {}

Client::Client(const Client &other)
{
        _fd = other._fd;
        _ipAddress = other._ipAddress;
        _hostname = other._hostname;
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
        _hostname = other._hostname;
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
int Client::getFd(void) const
{
    return (_fd);
}

std::string Client::getHostName(void)   const
{
    return (_hostname);
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

int Client::getChannelCount(void)   const
{
    return (_channelCount);
}

clientState Client::getState(void)  const
{
    return (_state);
}

//setters
void    Client::setHostName(std::string &hostname)
{
    _hostname = hostname;
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