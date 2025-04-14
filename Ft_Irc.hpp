#pragma once
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
# include <map>

extern bool running;

class Server;
# include "Client.hpp"
# include "Server.hpp"
# include "Commands.hpp"
// Define the command handler type
typedef void (*CommandHandler)(Server &server, Client &client, std::vector<std::string>&);

struct Command{
    std::string label;
    CommandHandler handler;
    clientState requiredAuthState;
    Command(const std::string& lbl, CommandHandler handle,  clientState auth) : label(lbl), handler(handle), requiredAuthState(auth) {}
};
/* Error Messages */

# define ERR_ALREADYREGISTERED std::string (" 462 :You may not reregister\r\n")
# define ERR_PASSWDMISMATCH std::string (" 464 :Password incorrect\r\n")

#define ERR_NONICKNAMEGIVEN std::string ("431 :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME std::string ("432 :Erroneous Nickname\r\n")
#define ERR_NICKNAMEINUSE std::string ("433 :The requested nickname is already in use by another client\r\n")
#define ERR_NEEDMOREPARAMS std::string ("461 :Not enough parameters\r\n")


/*Replies */
# define RPL_WELCOME std::string (" :Welcome to the IRC Network, ") 

