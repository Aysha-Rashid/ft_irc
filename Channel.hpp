# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include <map>

# include "Client.hpp"

# define MAX_CHANNEL_COUNT   10

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::string _channelKey;
    size_t _userLimit;
    std::vector<Client *> _clientList;
    std::map<std::string, bool> _operators;
    std::vector<Client *> _invitedList;
    bool _inviteOnly;
    bool _topicPrivilege;

    Channel();
    Channel(const Channel &other);
    Channel &operator= (const Channel &other);

public:
    Channel(std::string name, std::string password);
    ~Channel();

    //getters
    std::string getName() const;
    std::string getTopic() const;
    std::string getChannelKey() const;
    size_t getUserLimit() const;
    std::vector<Client *> getClients() const;
    std::vector<std::string> getNickNames() const;
    bool getInviteOnly() const;
    bool getTopicPrivilege() const;

    //setters
    void setChannelKey(const std::string &key);
    void setName(const std::string &name);
    void setTopic(const std::string &topic);
    void setUserLimit(const size_t limit);
    void setInviteOnly(bool inviteValue);
    void setInvited(Client *client);

    //member functions
    void addClient(Client *client);
    void removeClient(Client *client);
    void addOperator(const Client *client);
    void removeOperator(const Client *client);
    void broadcast(const std::string &message);
    void broadcast(const std::string &message, const Client *excludeClient);
    bool isOperator(const Client *client) const;
    bool isInvited(const Client *client);
    bool isClientInChannel(const Client *client) const;

};

# endif