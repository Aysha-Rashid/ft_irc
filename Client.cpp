#include "Ft_Irc.hpp"

void Client::broadcastMessage(Server *server, Client *curClient, const std::string &message) {
    for (std::vector<Client*>::iterator it = server->clients.begin(); it != server->clients.end(); ++it) {
        Client* client = *it;
            if (client->authenticated && client->registered && client->joinChannel && client->getChannel() == curClient->getChannel() && client->getSocket() != curClient->getSocket()) {
            	send(client->getSocket(), message.c_str(), message.length(), 0);
        }
    }
}

bool Client::isNickNameInUse(Server *server, const std::string& nickName) {
	for (std::vector<Client *>::iterator it = server->clients.begin(); it != server->clients.end(); it++) {
		if ((*it)->getNickName() == nickName) {
			return true;
		}
	}
	return false;
}

bool Client::isValidNickName(const std::string& nickName) {
	if (nickName.empty() || nickName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") != std::string::npos || nickName.length() > 9) {
		return false;
	}
	return true;
}

void Client::printClientError(int socket, std::string Errmessage)
{
	send(socket, Errmessage.c_str(), Errmessage.length(), 0);
	std::cout << Errmessage;
}
