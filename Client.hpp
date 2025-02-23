# ifndef CLIENT_HPP
# define CLIENT_HPP
# include "Ft_Irc.hpp"
# include <string>

enum    clientState
{   
    UNAUTHENTICTED,
    AUTHENTICTED,
    REGISTERED,
    DISCONNECTED,
};  

class Client    
{
private:
    int _fd;
    std::string _ipAddress;
    std::string _hostname;
    std::string _username;
    std::string _nickname;
    std::string _realname;
    clientState  _state;
    int _channelCount;
    
    Client();  
    Client(const Client &other);
    Client &operator = (const Client &other);

public:
    Client(int fd,std::string &ip, std::string  &hostname);
    ~Client();

    //getters
    int getFd(void) const;
    std::string getHostName(void)   const;
    std::string getUserName(void)   const;
    std::string getRealName(void)   const;
    std::string getNickName(void)   const;
    std::string getIpAddress(void) const;
    clientState getState(void)  const;
    int getChannelCount(void)   const;

    //setters
    void    setHostName(std::string &hostname);
    void    setUserName(std::string &username);
    void    setRealName(std::string &realname);
    void    setNickName(std::string &nickname);
    void    setState(clientState state);

    //member    functions
    void    write(const   std::string msg)    const;
};

# endif