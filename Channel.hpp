# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include <map>

# include "Ft_Irc.hpp"
# include "Client.hpp"

class Channel
{
private:
    std::string _name;
    std::vector<Client *> _clientList;
    std::map<std::string, bool> _operators;
    std::string _channelKey;
    std::string _topic;
    size_t _userLimit;
    bool _inviteOnly;

    Channel();
    Channel(const Channel &other);
    Channel &operator= (const Channel &other);

public:
    Channel(std::string name, std::string password);
    ~Channel();

    //member functions
    void addClient(Client *client);
};

# endif