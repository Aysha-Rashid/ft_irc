# ifndef SERVER_HPP
# define SERVER_HPP

# include "Ft_Irc.hpp"
# include "Utils.hpp"
# include "Channel.hpp"
# include "Client.hpp"
#include <map>

#define ERR_NONICKNAMEGIVEN std::string ("431 :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME std::string ("432 :Erroneous Nickname\r\n")
#define ERR_NICKNAMEINUSE std::string ("433 :The requested nickname is already in use by another client\r\n")
#define ERR_NEEDMOREPARAMS std::string ("461 :Not enough parameters\r\n")

extern bool running;

// void handlePass(int client_fd, std::vector<std::string> param);
// void handleNick(int client_fd, std::vector<std::string> param);
// void handleUser(int client_fd, std::vector<std::string> param);
class Server;
// Define the command handler type
typedef void (*CommandHandler)(Server &server, Client &client, std::vector<std::string>&);

struct Command{
    std::string label;
    CommandHandler handler;
    clientState requiredAuthState;
    Command(const std::string& lbl, CommandHandler handle,  clientState auth) : label(lbl), handler(handle), requiredAuthState(auth) {}
};



void handlePass(Server &server, Client &client, std::vector<std::string>& params);
void handleNick(Server &server, Client &client, std::vector<std::string>& params);
void handleUser(Server &server, Client &client, std::vector<std::string>& params);
void handleJoin(Server &server, Client &client, std::vector<std::string>& param);
void handlePart(Server &server, Client &client, std::vector<std::string>& param);
void handleInvite(Server &server, Client &client, std::vector<std::string>& param);
void handleMode(Server &server, Client &client, std::vector<std::string>& param);
void handlePing(Server &server, Client &client, std::vector<std::string>& param);
void handleQuit(Server &server, Client &client, std::vector<std::string>& param);
void handleWho(Server &server, Client &client, std::vector<std::string>& param);
void handleKick(Server &server, Client &client, std::vector<std::string>& param);
void handlePrivMsg(Server &server, Client &client, std::vector<std::string>& param);
void handleCap(Server &server, Client &client, std::vector<std::string>& param);
void handlePong(Server &server, Client &client, std::vector<std::string>& param);

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
        std::vector<Command> commands;  // ✅ Replace `map` with `vector`
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
        void        disconnected(Client *&client, int socket);
        void        registerChannel(Channel *channel);
        Channel     *getChannel(std::string &name);
        void        deleteChannel(Channel *Channel);
        void        disconnectClient(int socket, const std::string reason);
};

# endif
