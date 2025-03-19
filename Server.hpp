# ifndef SERVER_HPP
# define SERVER_HPP

# include "Ft_Irc.hpp"
# include "Utils.hpp"
# include "Channel.hpp"

class Server
{
    private:
        std::string         _serverName;
        std::string         _password;
        size_t              _port;
        int                 _socketFd;
        struct sockaddr_in	_address;
        socklen_t           _addrlen;
        fd_set              _readfds;
        int                 _maxfd;
        int                 out;

    public:
        std::vector<Client *> clients;
        std::map<std::string, Channel *> channels;
        Server(std::string name);
        ~Server();
        size_t      getPort(void) const;
        std::string getPassword(void) const;
        std::string getServerName(void) const { return (_serverName);};
        fd_set      &getReadfds(void) { return(_readfds);};
        int         getSocket(void) const {return (_socketFd);};
        void        portAndPass(const std::string &port, std::string password);
        void        creatingServer(Server &server);
        void        acceptConnection(void);
        void        run(void);
        void        setFds(void);
        void        ClientCommunication(void);
        int         Commands(Client **client, int socket, std::string commands);
        void        disconnected(Client *&client, int socket);
        int         handleAuthentication(std::string message, Client **client);
        void        registerChannel(Channel *channel);
        Channel     *getChannel(std::string &name);
        void        deleteChannel(Channel *Channel);
};

# endif