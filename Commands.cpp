
#include "Commands.h"

void Commands::kick(const Client& admin, const Client  /* channel to kick from, client to kick*/)
{
	//if (admin.getChannel()) 
	if (admin.isAdmin())
	{

	}
	else
		std::cout << "Can't perform command\n";
}

void Commands::invite(const Client& admin, const Client& invitee/*, Channel to invite to*/)
{
	if (client.isAdmin()) //get the channel the client is in, and check if its an admin of that channel
	{
		
	}
	else
		std::cout << "Can't perform command\n";
}

