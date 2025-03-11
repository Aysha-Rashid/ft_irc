# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include <map>

# include "Client.hpp"

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::string _channelKey;
    size_t _userLimit;
    std::vector<Client *> _clientList;
    std::map<std::string, bool> _operators;
    bool _inviteOnly;

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
    bool getInviteOnly() const;

    //setters
    void setChannelKey(const std::string &key);
    void setName(const std::string &name);
    void setTopic(const std::string &topic);
    void setUserLimit(const size_t limit);
    void setInviteOnly(bool inviteValue);

    //member functions
    void addClient(Client *client);
    void removeClient(Client *client);
    void addOperator(const Client *client);
    void removeOperator(const Client *client);
    void broadcast(const std::string &message);
    bool isOperator(const Client *client);

};

# endif