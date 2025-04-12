# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"


bool isNickNameInUse(Server &server, const std::string& nickName) {
	for (std::vector<Client *>::iterator it = server.clients.begin(); it != server.clients.end(); it++) {
		if ((*it)->getNickName() == nickName) {
			return true;
		}
	}
	return false;
}

bool isValidNickName(const std::string& nickName) {
	if (nickName.empty() || nickName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") != std::string::npos || nickName.length() > 9) {
		return false;
	}
	return true;
}

void handleNick(Server &server, Client &client, std::vector<std::string>& params)
{
	if(params.empty() || params[0].empty())
	{
		
		if(client.getNickName().empty())
			client.write(ERR_NONICKNAMEGIVEN);
		else
			client.write("431" + client.getNickName() + ":No nickname given\r\n");	
		return;
	}
	if (!isValidNickName(params[0]))
		(client).write(ERR_ERRONEUSNICKNAME);
	else if (isNickNameInUse(server, params[0]))
		(client).write(ERR_NICKNAMEINUSE);
	else {
		(client).setNickName(params[0]);
		}
}
