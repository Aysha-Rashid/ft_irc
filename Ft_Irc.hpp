#pragma once
#include <poll.h>
#include <iostream>
#include <exception>
#include <vector>
#include <cctype>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h> // (sockaddr_in) holds internet address
#include <fcntl.h>
#include <vector>
#include <string>
#include <signal.h>
#include <sstream>
#include <unistd.h>
# include <map>



extern bool running;

#include "Client.hpp"
# include "Server.hpp"

/* Error Messages */

# define ERR_ALREADYREGISTERED std::string (" 462 :You may not reregister\r\n")
# define ERR_PASSWDMISMATCH std::string (" 464 :Password incorrect\r\n")

#define ERR_NONICKNAMEGIVEN std::string ("431 :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME std::string ("432 :Erroneous Nickname\r\n")
#define ERR_NICKNAMEINUSE std::string ("433 :The requested nickname is already in use by another client\r\n")
#define ERR_NEEDMOREPARAMS std::string ("461 :Not enough parameters\r\n")


/*Replies */
# define RPL_WELCOME std::string (" :Welcome to the IRC Network, ") 

/* Commands */
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