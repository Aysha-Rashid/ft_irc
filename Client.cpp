#include "Ft_Irc.hpp"

void Client::broadcastMessage(Server *server, int sender, const std::string &message) {
	for (std::vector<Client*>::iterator it = server->clients.begin(); it != server->clients.end(); ++it) {
		Client* client = *it;

		if (client->authenticated && (client->waitingForUsername || client->waitingForNickName) && client->getSocket() != sender) { 
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
	if (nickName.empty() || nickName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") != std::string::npos) {
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
		std::cout << "Client " << client->getSocket() << " disconnected." << std::endl;
		close(client->getSocket());
		delete client;
		client = nullptr; // To prevent further access to the deleted pointer
		out = true;
	}
}


void Client::ClientCommunication(Server *server) {
	out = 0;
	for (std::vector<Client *>::iterator it = server->clients.begin(); it != server->clients.end();) {
		Client* client = *it;
		if (FD_ISSET(client->getSocket(), &server->getReadfds())) {
			char buffer[1024];
			int bytesReceived = recv(client->getSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytesReceived == 0) {
				disconnected(client, client->getSocket());
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
			while ((newlinePos = client->inputBuffer.find('\n')) != std::string::npos) {
				std::string message = client->inputBuffer.substr(0, newlinePos);
				client->inputBuffer.erase(0, newlinePos + 1); // Remove processed part
				if (!message.empty() && message.back() == '\r')
					message.pop_back();
				if (message.empty()) continue;
				if (handleAuthentication(message, &client, server))
				{
					disconnected(client, client->getSocket());
					it = server->clients.erase(it);  // Remove from client list
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
					if (message == "/QUIT")
					{
						disconnected(client, client->getSocket());
						it = server->clients.erase(it);
						break; 
					}
					else if (message.substr(0, 6) == "/JOIN ")
					{
						channel = message.substr(7);
						if (!channel.empty())
						{
							std::cout << "let's create a channel" << std::endl;
							std::string broadcastMsg = client->getNickName() + ": " + message + "\r\n";
							broadcastMessage(server, client->getSocket(), broadcastMsg);
							std::cout << broadcastMsg;
							joinChannel = true;
						}
						else
							std::cout << "enter a valid channel name" << std::endl;
					}
					if (joinChannel == true)
					{
						std::string broadcastMsg = client->getNickName() + ": " + message + "\r\n";
						broadcastMessage(server, client->getSocket(), broadcastMsg);
						std::cout << broadcastMsg;
					}
					else
					{
						std::string my_message = "Error(421): " + message + " UNKNOWN COMMAND\r\n";
						send(client->getSocket(), my_message.c_str(), my_message.length(), 0);
					}
				}
			}
			if (out == 1)
				continue;
		}
		++it;
	}
}
