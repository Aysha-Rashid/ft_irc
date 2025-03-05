# include "Channel.hpp"

Channel::Channel() :_inviteOnly(false), _userLimit(0) {}

Channel::Channel(std::string name, std::string password) 
    :_name(name), _channelKey(password), _inviteOnly(false), _userLimit(0) {}

Channel::Channel(const Channel &other) 
{
    _name = other._name;
    _topic = other._topic;
    _channelKey = other._channelKey;
    _inviteOnly = other._inviteOnly;
    _userLimit = other._userLimit;
}

Channel & Channel::operator = (const Channel &other)
{
    if(this != &other)
    {
        _name = other._name;
        _topic = other._topic;
        _channelKey = other._channelKey;
        _inviteOnly = other._inviteOnly;
        _userLimit = other._userLimit;
    }
    return (*this);
}

Channel::~Channel() {}


//member functions
void Channel::addClient(Client *client)
{
    _clientList.push_back(client);
    _operators[client->getNickName()] = false;
    client->incrementChannelCount();
}

