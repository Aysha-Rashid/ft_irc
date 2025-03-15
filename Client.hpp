# ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include <string>

enum    clientState
{   
    UNAUTHENTICATED,
    AUTHENTICATED,
    REGISTERED,
    DISCONNECTED,
};  

class Client    
{
private:
    int _fd;
    std::string _ipAddress;
    std::string _username;
    std::string _nickname;
    std::string _realname;
    clientState  _state;
    int _channelCount;
    
    

public:
    Client();  
    Client(const Client &other);
    Client &operator = (const Client &other);
    Client(int fd, std::string &ip);
    ~Client();
    std::string inputBuffer;

    //setters
    void    setSocketFd(int socketFd);
    void    setNickName(std::string &nickname);
    void    setUserName(std::string &username);
    void    setRealName(std::string &realname);
    void    setState(clientState state);

    //getters
    int getSocketFd(void) const;
    std::string getUserName(void)   const;
    std::string getRealName(void)   const;
    std::string getNickName(void)   const;
    std::string getIpAddress(void) const;
    clientState getState(void)  const;
    int getChannelCount(void)   const;

    //member    functions
    void    write(const   std::string msg)    const;
    void incrementChannelCount(void);
    void decrementChannelCount(void);

};

# endif