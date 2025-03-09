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

int Client::handleAuthentication(std::string message, Client **client, Server *server) {
	if (!(*client)->authenticated) {
		if (message.substr(0, 5) == "PASS ") {
			if (message.substr(5) == server->getPassword()) {
				(*client)->authenticated = true;
			} else {
				printClientError((*client)->getSocket(), "Incorrect Password\r\n");
				return 1;
			}
		}
	}
	if ((*client)->authenticated) {
		if (message.substr(0, 5) == "NICK ") {
			std::string nick = message.substr(5);
			if (!isValidNickName(nick))
				printClientError((*client)->getSocket(), ERR_ERRONEUSNICKNAME);
			else if (isNickNameInUse(server, nick))
				printClientError((*client)->getSocket(), ERR_NICKNAMEINUSE);
			else {
				(*client)->setNickName(nick);
				std::cout << "checking nickname : " << (*client)->_nickName << std::endl;
				(*client)->waitingForNickName = true;
			}
		}
		if (message.substr(0, 5) == "USER ") {
			std::stringstream ss(message.substr(5));
			std::string username, realName, permission;
			char mode;
			ss >> username >> mode >> permission >> realName;
			if (ss.fail() || username.empty() || permission.empty() || realName.empty())
				printClientError((*client)->getSocket(), ERR_NEEDMOREPARAMS);
			else {
				if (mode > '8' && mode < '0')  
					printClientError((*client)->getSocket(), "Invalid arguments\r\n");
				else {
					(*client)->setUserName(username);
					(*client)->setRealName(realName);
					(*client)->waitingForUsername = true;
				}
			}
		}
	}
	return 0;
}

void Client::disconnected(Client *&client, int socket) {
	if (client != nullptr) {
		if (!client->getNickName().empty())
			std::cout << this->getNickName() << " disconnected." << std::endl;
		close(client->getSocket());
		delete client;
		client = nullptr; // To prevent further access to the deleted pointer
		out = true;
	}
}

void trim(std::string& str) {
	str.erase(0, str.find_first_not_of(' '));
	str.erase(str.find_last_not_of(' ') + 1);
}

int Client::Commands(Client **client, int socket, std::string commands, Server *server)
{
	if (commands.substr(0, 5) == "JOIN ") {
		(*client)->_channel = commands.substr(5);  // Set the channel
		trim((*client)->_channel);  // Clean the channel name
		if (!(*client)->_channel.empty() && (*client)->_channel[0] == '#' && (*client)->_channel.length() > 1) {
			(*client)->joinChannel = true;  // Mark as joined
			std::cout << "checking command " << commands << std::endl;
			std::string broadcastMsg = (*client)->getNickName() + ": " + commands + "\r\n";
			broadcastMessage(server, (*client), broadcastMsg);  // Broadcast the join message
		}
	}
	else if (commands.substr(0, 5) == "QUIT")
		disconnected((*client), (*client)->getSocket());
	else
		return 0;
	return (1);
}

void Client::ClientCommunication(Server *server) {
	out = 0;
	for (std::vector<Client *>::iterator it = server->clients.begin(); it != server->clients.end();) {
		Client* client = *it;
		if (FD_ISSET(client->getSocket(), &server->getReadfds())) {
			char buffer[1024];
			int bytesReceived = recv(client->getSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytesReceived == 0) {
				Commands(&client, client->getSocket(), "QUIT", server);
				// disconnected(client, client->getSocket());
				it = server->clients.erase(it);
				continue;
			}
			if (bytesReceived < 0) {
				perror("recv error");
				continue;
			}
			buffer[bytesReceived] = '\0';
			std::string receivedData(buffer);
			client->inputBuffer += receivedData;
			size_t newlinePos;
			while ((newlinePos = client->inputBuffer.find('\n')) != std::string::npos) // to handle the ctrl+d buffer
			{
				std::string message = client->inputBuffer.substr(0, newlinePos);
				if (message[0] == '/')
					message = message.substr(1);
				client->inputBuffer.erase(0, newlinePos + 1); // Remove processed part
				if (!message.empty() && message.back() == '\r')
					message.pop_back();
				if (message.empty()) continue;
				if (handleAuthentication(message, &client, server))
				{
					Commands(&client, client->getSocket(), "QUIT", server);
					it = server->clients.erase(it);
					break;
				}
				if (client->authenticated && client->waitingForUsername && client->waitingForNickName && !client->registered) {
					client->registered = true;
					std::string welcomeMsg = ":" + server->getServerName() + " 001 " + client->getNickName() + " :Welcome to the IRC server, " + client->getNickName() + "\r\n";
					// server 001 is a numeric reply code used by the IRC server to indicate that the client has successfully connected.
					send(client->getSocket(), welcomeMsg.c_str(), welcomeMsg.length(), 0);
					std::cout << welcomeMsg;
				}
				else if (client->registered)
				{
					if (message == "QUIT")
					{
						Commands(&client, client->getSocket(), "QUIT", server);
						it = server->clients.erase(it);
						break; 
					}
					else if (message.substr(0, 5) == "JOIN ")
					{
						// std::string removeSlash = message.substr(1);
						if (!Commands(&client, client->getSocket(), message, server))
						{
							// testing in the server (for debug); better to have this function as void
							std::cout << "enter a valid channel name" << std::endl;
						}
					}
					if (client->joinChannel)
					{
						std::string broadcastMsg = client->getNickName() + ": " + message + "\r\n";
						broadcastMessage(server, client, broadcastMsg);
						std::cout << broadcastMsg;
					}
					else
					{
						std::string my_message = "Error(421): " + message + " UNKNOWN COMMAND\r\n";
						send(client->getSocket(), my_message.c_str(), my_message.length(), 0);
						std::cout << my_message;
					}
				}
			}
			if (out == 1)
				continue;
		}
		++it;
	}
}
