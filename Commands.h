
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


void handlePass(Server *server, Client *client, std::vector<std::string>& params);
void handleNick(Server *server, Client *client, std::vector<std::string>& params);
void handleUser(Server *server, Client *client, std::vector<std::string>& params);
void handleJoin(Server *server, Client *client, std::vector<std::string>& param);
void handlePart(Server *server, Client *client, std::vector<std::string>& param);
void handleInvite(Server *server, Client *client, std::vector<std::string>& param);
void handleMode(Server *server, Client *client, std::vector<std::string>& param);
void handlePing(Server *server, Client *client, std::vector<std::string>& param);
void handleQuit(Server *server, Client *client, std::vector<std::string>& param);
void handleWho(Server *server, Client *client, std::vector<std::string>& param);
void handleKick(Server *server, Client *client, std::vector<std::string>& param);
void handlePrivMsg(Server *server, Client *client, std::vector<std::string>& param);
void handlePong(Server *server, Client *client, std::vector<std::string>& param);
// void handleCap(Server *server, Client *client, std::vector<std::string>& param);
#endif
