# ifndef CLIENT_HPP
# define CLIENT_HPP
# include "Ft_Irc.hpp"
# include <string>

class Client
{
private:
    int _fd;
    int _port;
    std::string _username;
    std::string _nickname;
    std::string _realname;
    std::string _hostname;
    
public:
    Client()
    Client(int fd);
    Client(const Client &other);
    ~Client();
};




# endif