# include "Channel.hpp"
Channel::Channel() :_channelKey(""),_topic("") , _userLimit(0), _inviteOnly(false){}

Channel::Channel(std::string name, std::string password) 
    :_name(name), _channelKey(password), _topic("") , _userLimit(0), _inviteOnly(false),_topicPrivilege(false){}

Channel::Channel(const Channel &other) 
{
    _name = other._name;
    _topic = other._topic;
    _channelKey = other._channelKey;
    _inviteOnly = other._inviteOnly;
    _userLimit = other._userLimit;
    _topicPrivilege = other._topicPrivilege;
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
        _topicPrivilege = other._topicPrivilege;
    }
    return (*this);
}

Channel::~Channel() {}

//getters
std::string Channel::getName() const
{
    return (_name);
}

std::string Channel::getTopic() const
{
    return(_topic);
}

std::string Channel::getChannelKey() const
{
    return(_channelKey);
}

size_t Channel::getUserLimit() const
{
    return(_userLimit);
}

std::vector<Client *> Channel::getClients() const
{
    return(_clientList);
}

std::vector<std::string> Channel::getNickNames() const
{
    std::vector<std::string> nicknames;
    std::vector<Client *>::const_iterator it = _clientList.begin();
    while(it != _clientList.end())
    {
       nicknames.push_back((isOperator(*it) ? "@" : "" ) + (*it)->getNickName());
       ++it;
    }
    return (nicknames);
}

bool Channel::getInviteOnly() const
{
    return(_inviteOnly);
}

bool Channel::getTopicPrivilege() const
{
    return(_topicPrivilege);
}

std::string Channel::getChannelMode() const
{
  std::string modeStr = "";

  if(this->getTopicPrivilege())
    modeStr.append("t");
  if(this->getUserLimit() > 0)  
    modeStr.append("l");
  if(this->getInviteOnly())
    modeStr.append("i");
  if(!this->getChannelKey().empty())
    modeStr.append("k");

  return (modeStr);
}

//setters
void Channel::setChannelKey(const std::string &key)
{
    _channelKey = key;
}

void Channel::setName(const std::string &name)
{
    _name = name;
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

void Channel::setUserLimit(const size_t limit)
{
    _userLimit = limit;
}

void Channel::setInviteOnly(bool inviteValue)
{
    _inviteOnly = inviteValue;
}

void Channel::setTopicPrivilege(bool topicPriv)
{
    _topicPrivilege = topicPriv;
}
void Channel::setInvited(Client *client)
{
    std::vector<Client *> :: iterator it = _invitedList.begin();
     while (it != _invitedList.end())
    {
       if(*it == client)
            return;
       ++it;
    }
    _invitedList.push_back(client);
}

//member functions
void Channel::addClient(Client *client)
{
    _clientList.push_back(client);
    _operators[client->getNickName()] = false;
    client->incrementChannelCount();
}

void Channel::removeClient(Client *client)
{
    std::vector<Client *>::iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        if(*it == client)
        {
            _clientList.erase(it);
            break;
        }
        ++it;
        
    }
    it = _invitedList.begin();
    while (it != _invitedList.end())
    {
        if(*it == client)
        {
            _invitedList.erase(it);
            break;
        }
        ++it;
    }
    _operators.erase(client->getNickName());
    client->decrementChannelCount();
}

void Channel::addOperator(const Client *client)
{
    _operators[client->getNickName()] = true;
}

void Channel::removeOperator(const Client *client)
{
      _operators[client->getNickName()] = false;
}

void Channel::broadcast(const std::string &message)
{
    std::vector<Client *>::iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        (*it)->write(message);
        ++it;
    }
}

void Channel::broadcast(const std::string &message,const Client *excludeClient)
{
    std::vector<Client *>::iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        if(*it != excludeClient)
            (*it)->write(message);
        ++it;
    }
}

bool Channel::isOperator(const Client *client) const
{
    std::map<std::string, bool> ::const_iterator it = _operators.find(client->getNickName());
    if(it != _operators.end())
        return (it->second);
    return (false);    
}

bool Channel::isInvited(const Client *client)
{
    std::vector<Client *> :: iterator it = _invitedList.begin();
     while (it != _invitedList.end())
    {
        if(*it == client)
            return (true);
        ++it;
    }
    return (false);
}

bool Channel::isClientInChannel(const Client *client) const
{
    std::vector<Client *>::const_iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        if(*it == client)
            return (true);
        ++it;
    }
    return (false);
}

Client * Channel::getClientWithNickname(const std::string &nickname)
{
    std::vector<Client *>::const_iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        if((*it)->getNickName() == nickname)
            return (*it);
        ++it;
    }
    return (NULL); 
}


