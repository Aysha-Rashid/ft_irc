# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"


bool isNickNameInUse(Server *server, const std::string& nickName) {
	for (std::vector<Client *>::iterator it = server->clients.begin(); it != server->clients.end(); it++) {
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


void handleNick(Server *server, Client *client, std::vector<std::string>& params)
{
	if (params.size() > 2)
		(client)->write(ERR_ERRONEUSNICKNAME);
	std::string nick = params[1];
 	if (!isValidNickName(nick))
		(client)->write(ERR_ERRONEUSNICKNAME);
	else if (isNickNameInUse(server, nick))
		(client)->write(ERR_NICKNAMEINUSE);
	else {
		(client)->setNickName(nick);
		}
}
  

