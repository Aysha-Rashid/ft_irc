# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"

std::vector<Channel*> Server::getChannelsByClient(Client* client) {
	std::vector<Channel*> result;
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel* chan = it->second;
		if (chan->isClientInChannel(client)) {
			result.push_back(chan);
		}
	}
	return result;
}

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
		client.write(":" + server.getServerName() + " 432 " + params[0] + " " + params[0] + " :Erroneous nickname\r\n");
	else if (isNickNameInUse(server, params[0]))
		client.write(":" + server.getServerName() + " 433 " + params[0] + " " + params[0] + " :Nickname is already in use\r\n");
	else {
			if (client.getNickName().empty())
			{
				client.setNickName(params[0]);
				if (!client.getUserName().empty())
				{
					client.setState(REGISTERED);
					client.write(":" + server.getServerName() + " 001 " + client.getNickName() + " :Welcome " +client.getNickName() +" ,to the IRC server\r\n");	
					std::cout << " 001 " + client.getNickName() + " :Welcome " +client.getNickName() +" to the IRC server\r\n";
				}
			}
			else
			{
				std::string oldNick = client.getNickName();
				client.setNickName(params[0]);
				std::string newNick = client.getNickName();

				std::string nickChangeMsg = ":" + oldNick + "!" + client.getUserName() + "@" + server.getServerName() + " NICK :" + newNick + "\r\n";
				std::vector<Channel *> channels = server.getChannelsByClient(&client);
				for (size_t i = 0; i < channels.size(); ++i) {
					channels[i]->broadcast(nickChangeMsg);
			}
		}
	}
}
