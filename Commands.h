
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

	static void kick(const Client& admin, const Client& client /* channel to kick from*/);
	static void invite(const Client& admin, const Client& invitee /*, channel to invite to*/);
	static void topic(/**/);
	static void mode(/**/);
};

#endif
