# ifndef SERVER_HPP
# define SERVER_HPP

# include "Ft_Irc.hpp"
# include "Utils.hpp"
# include "Channel.hpp"
# include "Client.hpp"
#include <map>


extern bool running;

class Server;
// Define the command handler type
typedef void (*CommandHandler)(Server &server, Client &client, std::vector<std::string>&);

struct Command{
    std::string label;
    CommandHandler handler;
    clientState requiredAuthState;
    Command(const std::string& lbl, CommandHandler handle,  clientState auth) : label(lbl), handler(handle), requiredAuthState(auth) {}
};


class Server
{
    private:
        std::string         _serverName;
        std::string         _password;
        int                 _port;
        int                 _socketFd;
        struct sockaddr_in	_address;
        socklen_t           _addrlen;
        fd_set              _readfds;
        int                 _maxfd;
        int                 out;

    public:
        std::vector<Client *> clients;
        std::vector<Command> commands;
        std::map<std::string, Channel *> channels;
        Server(std::string name);
        ~Server();
        size_t          getPort(void) const;
        std::string     getPassword(void) const;
        std::string     getServerName(void) const { return (_serverName);};
        fd_set          &getReadfds(void) { return(_readfds);};
        int             getSocket(void) const {return (_socketFd);};
        void            portAndPass(const std::string &port, std::string password);
        void            creatingServer(Server &server);
        void            acceptConnection(void);
        void            run(void);
        void            setFds(void);
        void            ClientCommunication(void);
        void            disconnected(Client *&client, int socket);
        void            registerChannel(Channel *channel);
        Channel         *getChannel(std::string &name);
        void            deleteChannel(Channel *Channel);
        void            disconnectClient(int socket, const std::string reason);
        std::vector<Channel *> getChannelsByClient(Client *client);
};

# endif
