# ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "Ft_Irc.hpp"

class Server;

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
void handleTopic(Server &server, Client &client, std::vector<std::string>& param);
void handlePong(Server &server, Client &client, std::vector<std::string>& param);

# endif