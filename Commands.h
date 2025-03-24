
#ifndef	COMMANDS_H
#define COMMANDS_H

// #include "Client.hpp"

// class Commands
// {
// public:
// 	Commands() = delete;
// 	Commands(const Commands& other) = delete;
// 	Commands&	operator=(Commands& rhs) = delete;
// 	~Commands() = delete;

// 	static void join(/**/);
// 	static void kick(const Client& admin, const Client& client /* channel to kick from*/);
// 	static void invite(const Client& admin, const Client& invitee /*, channel to invite to*/);
// 	static void topic(/**/);
// 	static void mode(/**/);
// };

#include "Ft_Irc.hpp"

class Server;
typedef void (*CommandHandler)(Server *,Client *, std::vector<std::string>&);

struct Command {
    std::string label;
    CommandHandler handler;
    clientState requiredAuthState;
    Command(const std::string& lbl, CommandHandler handle, clientState Auto) : label(lbl), handler(handle), requiredAuthState(Auto) {}
};

void handlePass(const Server &server,const Client &client, std::vector<std::string>& params);
void handleNick(const Server &server,const Client &client, std::vector<std::string>& params);
void handleUser(const Server &server,const Client &client, std::vector<std::string>& params);
void handleJoin(const Server &server,const Client &client, std::vector<std::string>& param);
void handlePart(const Server &server,const Client &client, std::vector<std::string>& param);
void handleInvite(const Server &server,const Client &client, std::vector<std::string>& param);
void handleMode(const Server &server,const Client &client, std::vector<std::string>& param);
void handlePing(const Server &server,const Client &client, std::vector<std::string>& param);
void handleQuit(const Server &server,const Client &client, std::vector<std::string>& param);
void handleWho(const Server &server,const Client &client, std::vector<std::string>& param);
void handleKick(const Server &server,const Client &client, std::vector<std::string>& param);
void handlePrivMsg(const Server &server,const Client &client, std::vector<std::string>& param);
void handlePong(const Server &server,const Client &client, std::vector<std::string>& param);
// void handleCap(Server *server, Client *client, std::vector<std::string>& param);
#endif
