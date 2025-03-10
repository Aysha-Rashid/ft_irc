
#ifndef	COMMANDS_H
#define COMMANDS_H

#include "Client.hpp"

class Commands
{
public:
	Commands() = delete;
	Commands(const Commands& other) = delete;
	Commands&	operator=(Commands& rhs) = delete;
	~Commands() = delete;

	static void kick(const Client& client /* channel to kick from, client to kick*/);
	static void invite(const Client& admin, const Client& invitee);
	static void topic(/**/);
	static void mode(/**/);
};

#endif
