#include "Ft_Irc.hpp"
# include "Server.hpp"

Server::Server(std::string name) : _serverName(name) {
	commands.push_back(Command("PASS", handlePass, UNAUTHENTICATED));
	commands.push_back(Command("NICK", handleNick, AUTHENTICATED));
	commands.push_back(Command("USER", handleUser, AUTHENTICATED));
	commands.push_back(Command("JOIN", handleJoin, REGISTERED));
	commands.push_back(Command("PART", handlePart, REGISTERED));
	commands.push_back(Command("INVITE", handleInvite, REGISTERED));
	commands.push_back(Command("MODE", handleMode, REGISTERED));
	commands.push_back(Command("QUIT", handleQuit, UNAUTHENTICATED));
	commands.push_back(Command("WHO", handleWho, REGISTERED));
	commands.push_back(Command("KICK", handleKick, REGISTERED));
	commands.push_back(Command("PRIVMSG", handlePrivMsg, REGISTERED));
	commands.push_back(Command("PING", handlePing, REGISTERED));
	commands.push_back(Command("PONG", handlePong, UNAUTHENTICATED));
}


// ✅ Ensure These Are Defined Before Using Them
void handlePass(Server &server, Client &client, std::vector<std::string>& params)
 {
	if(params.empty())
	{
		client.write(ERR_NEEDMOREPARAMS);
		return;
	}
	if(client.getState() == AUTHENTICATED || client.getState() == REGISTERED)
	{
		client.write(ERR_ALREADYREGISTERED);
		return;
	}
	if(params[0] != server.getPassword())
	{
		client.write(ERR_PASSWDMISMATCH);
		return;
	}
	client.setState(AUTHENTICATED);
}

void handleUser(Server &server, Client &client, std::vector<std::string>& params) 
{
	(void) server;
	if(client.getState() == REGISTERED)
	{
		client.write(ERR_ALREADYREGISTERED);
		return;
	}
	if(params.empty() || params.size() < 4)
	{
		client.write(ERR_NEEDMOREPARAMS);
		return;
	}

	client.setUserName(params[0]);
	client.setRealName(params[3]);
	if(!client.getNickName().empty())
		client.setState(REGISTERED);
	client.write(":" + server.getServerName() + " 001 " + client.getNickName() + " :Welcome " +client.getNickName() +" ,to the IRC server\r\n");	
	std::cout << " 001 " + client.getNickName() + " :Welcome " +client.getNickName() +" to the IRC server\r\n";
	}


void handlePart(Server &server, Client &client, std::vector<std::string>& params) {
	if (params.empty()) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " PART :Not enough parameters\r\n");
        return;
    }

    std::vector<std::string> channelNames = split(params[0], ',');
    std::string reason = params.size() > 1 ? params[1] : "Leaving";

    for (size_t i = 0; i < channelNames.size(); i++) {
        std::string channelName = channelNames[i];
        Channel *channel = server.getChannel(channelName);

        if (!channel) {
            client.write(":" + server.getServerName() + " 403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
            continue;
        }

        if (!channel->isClientInChannel(&client)) {
            client.write(":" + server.getServerName() + " 442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
            continue;
        }

        // Check if the leaving client is an operator
        bool wasOperator = channel->isOperator(&client);
        
        // Remove operator status if the client is an operator
        if (wasOperator) {
            channel->removeOperator(&client);
        }

        // Broadcast PART message to all channel members
        channel->broadcast(":" + client.getPrefix() + " PART " + channelName + " :" + reason + "\r\n");
        
        // Remove client from channel
        channel->removeClient(&client);
        
        // If channel is empty, delete it
        if (channel->getClients().empty()) {
            server.channels.erase(channelName);
            delete channel;
        }
    }
}
void handleInvite(Server &server, Client &client, std::vector<std::string>& params) {
    if (params.size() < 2) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " INVITE :Not enough parameters\r\n");
        return;
    }

    std::string targetNick = params[0];
    std::string channelName = params[1];

    // Find target client
    Client *targetClient = nullptr;
    for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
        if ((*it)->getNickName() == targetNick) {
            targetClient = *it;
            break;
        }
    }

    if (!targetClient) {
        client.write(":" + server.getServerName() + " 401 " + client.getNickName() + " " + targetNick + " :No such nick\r\n");
        return;
    }

    Channel *channel = server.getChannel(channelName);
    if (!channel) {
        client.write(":" + server.getServerName() + " 403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!channel->isClientInChannel(&client)) {
        client.write(":" + server.getServerName() + " 442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (channel->isClientInChannel(targetClient)) {
        client.write(":" + server.getServerName() + " 443 " + client.getNickName() + " " + targetNick + " " + channelName + " :is already on channel\r\n");
        return;
    }

    if (!channel->isOperator(&client)) {
        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // Send INVITE message to target client
    targetClient->write(":" + client.getPrefix() + " INVITE " + targetNick + " " + channelName + "\r\n");
    
    // Send RPL_INVITING to inviting client
    client.write(":" + server.getServerName() + " 341 " + client.getNickName() + " " + targetNick + " " + channelName + "\r\n");
    
    // Add target to invited list
    channel->setInvited(targetClient);
}
void handleMode(Server &server, Client &client, std::vector<std::string>& params) {
    if (params.empty()) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
        return;
    }

    std::string target = params[0];
    std::string mode = params[1];

    // If target starts with # or &, it's a channel
    if (target[0] == '#' || target[0] == '&') {
        Channel *channel = server.getChannel(target);
        if (!channel) {
            client.write(":" + server.getServerName() + " 403 " + client.getNickName() + " " + target + " :No such channel\r\n");
            return;
        }

        // Check if client is in the channel first
        if (!channel->isClientInChannel(&client)) {
            client.write(":" + server.getServerName() + " 442 " + client.getNickName() + " " + target + " :You're not on that channel\r\n");
            return;
        }

        // Then check if they're an operator
        if (!channel->isOperator(&client)) {
            client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
            return;
        }

        // Handle mode changes
        if (mode[0] == '+') {
            for (size_t i = 1; i < mode.size(); i++) {
                switch (mode[i]) {
                    case 'i': // Invite-only mode
                        channel->setInviteOnly(true);
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " +i\r\n", &client);
                        break;
                    case 'o': // Give operator status
                        if (params.size() > 2) {
                            Client *targetClient = nullptr;
                            for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
                                if ((*it)->getNickName() == params[2]) {
                                    targetClient = *it;
                                    break;
                                }
                            }
                            if (targetClient && channel->isClientInChannel(targetClient)) {
                                channel->addOperator(targetClient);
                                channel->broadcast(":" + client.getPrefix() + " MODE " + target + " +o " + params[2] + "\r\n", &client);
                            }
                        }
                        break;
                }
            }
        } else if (mode[0] == '-') {
            for (size_t i = 1; i < mode.size(); i++) {
                switch (mode[i]) {
                    case 'i': // Remove invite-only mode
                        channel->setInviteOnly(false);
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " -i\r\n", &client);
                        break;
                    case 'o': // Remove operator status
                        if (params.size() > 2) {
                            Client *targetClient = nullptr;
                            for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
                                if ((*it)->getNickName() == params[2]) {
                                    targetClient = *it;
                                    break;
                                }
                            }
                            if (targetClient && channel->isClientInChannel(targetClient)) {
                                channel->removeOperator(targetClient);
                                channel->broadcast(":" + client.getPrefix() + " MODE " + target + " -o " + params[2] + "\r\n", &client);
                            }
                        }
                        break;
                }
            }
        }
    }
}
void handlePing(Server &server, Client &client, std::vector<std::string>& params) {
    if (params.empty()) {
        client.write(":" + server.getServerName() + " 409 " + client.getNickName() + " :No origin specified\r\n");
        return;
    }
    
    std::string token = params[0];
    
    // Reply with PONG message containing the same token
    client.write(":" + server.getServerName() + " PONG " + server.getServerName() + " :" + token + "\r\n");
}
void handlePong(Server &server, Client &client, std::vector<std::string>& params) {    
    if (params.empty()) {
        std::string msg= ":" + server.getServerName() + " 409 " + client.getNickName() + " :No origin specified\r\n";
        client.write(msg);
        return;
    }
    
    // Update the client's last activity time
    client.updateLastActivity();
    
    // The token in params[0] should match what was sent in the PING
    // We don't need to do anything else as this just confirms the client is alive
}

void handleQuit(Server &server, Client &client, std::vector<std::string>& params) {
	std::string reason = "Leaving";

	if(client.getState() == REGISTERED)
	{
		server.disconnectClient(client.getSocketFd(),reason);
	}
	else
	{
		if(client.getNickName().empty())
			client.write(":"+ server.getServerName() + " QUIT :" + reason + "\r\n");
		else
			client.write(":"+ server.getServerName() + " " + client.getNickName() +" QUIT :" + reason + "\r\n");
	}
	std::cout << ": "+ server.getServerName() + " " + client.getNickName() +" QUIT :" + reason + "\r\n";
}
void handleWho(Server &server, Client &client, std::vector<std::string>& params) {
	if (params.empty()) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " WHO :Not enough parameters\r\n");
        return;
    }

    std::string target = params[0];
    bool operFlag = (params.size() > 1 && params[1] == "o");

    // Check if target is a channel
    if (target[0] == '#' || target[0] == '&') {
        Channel *channel = server.getChannel(target);
        if (channel) {
            std::vector<Client*> clients = channel->getClients();
            for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                Client *member = *it;
                
                // Skip if "o" flag specified and member is not a channel operator
                if (operFlag && !channel->isOperator(member))
                    continue;
                
                // Format: <channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>
                std::string flags = "H"; // H for "here", G would be "gone"
                if (channel->isOperator(member))
                    flags += "@"; // @ for channel operator
                
                client.write(":" + server.getServerName() + " 352 " + client.getNickName() + " " + 
                             channel->getName() + " " + member->getUserName() + " " + 
                             member->getIpAddress() + " " + server.getServerName() + " " + 
                             member->getNickName() + " " + flags + " :0 " + 
                             member->getRealName() + "\r\n");
            }
        }
    } else {
        // Handle WHO for a specific mask (nickname)
        for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
            Client *cli = *it;
            
            // Check if client matches the mask (simple implementation - just exact nickname match)
            if (cli->getNickName() == target || target == "*") {
                if (operFlag) {
                    // Check if client is an operator in any channel
                    bool isOp = false;
                    for (std::map<std::string, Channel*>::iterator ch = server.channels.begin(); ch != server.channels.end(); ++ch) {
                        if (ch->second->isOperator(cli)) {
                            isOp = true;
                            break;
                        }
                    }
                    if (!isOp) continue;
                }
                
                // Find a channel the client is in (if any) for the response
                std::string channelName = "*";
                std::string flags = "H";
                
                for (std::map<std::string, Channel*>::iterator ch = server.channels.begin(); ch != server.channels.end(); ++ch) {
                    if (ch->second->isClientInChannel(cli)) {
                        channelName = ch->second->getName();
                        if (ch->second->isOperator(cli))
                            flags += "@";
                        break;
                    }
                }
                
                client.write(":" + server.getServerName() + " 352 " + client.getNickName() + " " + 
                             channelName + " " + cli->getUserName() + " " + 
                             cli->getIpAddress() + " " + server.getServerName() + " " + 
                             cli->getNickName() + " " + flags + " :0 " + 
                             cli->getRealName() + "\r\n");
            }
        }
    }
    client.write(":" + server.getServerName() + " 315 " + client.getNickName() + " " + target + " :End of WHO list\r\n");
}

void handleKick(Server &server, Client &client, std::vector<std::string>& params) {
    if (params.size() < 2) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " KICK :Not enough parameters\r\n");
        return;
    }

    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = "Kicked";
    
    // If there's a reason provided, use it
    if (params.size() > 2) {
        reason = params[2];
        if (reason[0] == ':')
            reason = reason.substr(1);
        
        // Combine additional parameters for reason if there are any
        for (size_t i = 3; i < params.size(); i++) {
            reason += " " + params[i];
        }
    }

    // Check if channel exists
    Channel *channel = server.getChannel(channelName);
    if (!channel) {
        client.write(":" + server.getServerName() + " 403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
        return;
    }

    // Check if kicker is in channel
    if (!channel->isClientInChannel(&client)) {
        client.write(":" + server.getServerName() + " 442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // Check if kicker is a channel operator
    if (!channel->isOperator(&client)) {
        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // Find target client
    Client *targetClient = nullptr;
    for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
        if ((*it)->getNickName() == targetNick) {
            targetClient = *it;
            break;
        }
    }

    // Check if target exists
    if (!targetClient) {
        client.write(":" + server.getServerName() + " 401 " + client.getNickName() + " " + targetNick + " :No such nick\r\n");
        return;
    }

    // Check if target is in the channel
    if (!channel->isClientInChannel(targetClient)) {
        client.write(":" + server.getServerName() + " 441 " + client.getNickName() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
        return;
    }

    // Broadcast kick message to all members of the channel
    std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    channel->broadcast(kickMsg);

    // Remove client from channel
    channel->removeClient(targetClient);
}

void handlePrivMsg(Server &server, Client &client, std::vector<std::string>& params) {
	if (params.size() < 2) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " PRIVMSG :Not enough parameters\r\n");
        return;
    }

    std::string target = params[0];
    std::string message = params[1];
    if (message[0] == ':')
        message = message.substr(1);
    for (size_t i = 2; i < params.size(); i++)
        message += " " + params[i];

    // If target starts with # or &, it's a channel message
    if (target[0] == '#' || target[0] == '&') {
        Channel *channel = server.getChannel(target);
        if (!channel) {
            client.write(":" + server.getServerName() + " 403 " + client.getNickName() + " " + target + " :No such channel\r\n");
            return;
        }

        if (!channel->isClientInChannel(&client)) {
            client.write(":" + server.getServerName() + " 404 " + client.getNickName() + " " + target + " :Cannot send to channel\r\n");
            return;
        }

        // Broadcast message to all channel members
        channel->broadcast(":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n", &client);
    } else {
        // Private message to a user
        Client *targetClient = nullptr;
        for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
            if ((*it)->getNickName() == target) {
                targetClient = *it;
                break;
            }
        }

        if (!targetClient) {
            client.write(":" + server.getServerName() + " 401 " + client.getNickName() + " " + target + " :No such nick\r\n");
            return;
        }

        // Send message to target client
        targetClient->write(":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n");
    }
}
// void handleCap(Server *server, int client_fd, std::vector<std::string>& params) {
//     // Function logic
// }
// void handlePong(int client_fd, std::vector<std::string>& params) {
//     // Function logic
// }

Server::~Server()
{
    std::string quitMsg = "server QUIT :Server shutting down\n";

	if (!clients.empty())
	{
		for (size_t i = 0; i < clients.size(); i++) {
			int clientSocket = clients[i]->getSocketFd();
			
			if (clientSocket > 0) {
				send(clientSocket, quitMsg.c_str(), quitMsg.length(), 0);
				close(clientSocket);
			}

			delete clients[i];
		}
	}
	clients.clear();
	std::cout << "server QUIT :Server shutting down\r\n";
}

size_t Server::getPort(void) const
{
    return (this->_port);
}

std::string Server::getPassword(void) const
{
    return (this->_password);
}

// Client* Server::getClientByFd(int client_fd) {
// 	for (size_t i = 0; i < clients.size(); i++) {
// 		if (clients[i]->getSocketFd() == client_fd)
// 			return clients[i];
// 	}
// 	return NULL;
// }

void Server::portAndPass(const std::string& port, std::string password)
{
    if (port.empty() || password.empty())
        throw std::runtime_error("Empty Argument");
    for (std::string::const_iterator it = port.begin(); it != port.end(); ++it) {
        if (!std::isdigit(*it))
            throw std::runtime_error("Invalid Port");
    }
    char *end;
    this->_port = strtol(port.c_str(), &end, 10);
    if (this->_port < 0 ||  this->_port > 65535)
        throw std::runtime_error("Invalid Port");
    this->_password = password;
}

static void checkError(int result, const char *error, const std::string &errmeg)
{
    if (result < 0)
    {
        perror(error);
        throw std::runtime_error(errmeg);
    }
}

void Server::setFds() {
	FD_ZERO(&_readfds);
	FD_SET(_socketFd, &_readfds);
	_maxfd = _socketFd;

	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		Client* client = *it;
		FD_SET(client->getSocketFd(), &_readfds);
		if (client->getSocketFd() > _maxfd) {
			_maxfd = client->getSocketFd();
		}
	}
}

static void signal_handler(int signal)
{
	if (signal == SIGINT)
		running = 0;
	if (signal == SIGQUIT)
		running = 1;
}

void Server::acceptConnection(void)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int newClient_fd = accept(_socketFd, (struct sockaddr*)&client_addr, &client_len);
	if (newClient_fd >= 0){
		std::cout << "New client attempting to connect: " << newClient_fd << std::endl;
		fcntl(newClient_fd, F_SETFL, O_NONBLOCK);
		std::string inet_addr = inet_ntoa(client_addr.sin_addr);
		Client *currentClient = new Client(newClient_fd, inet_addr);
   		currentClient->setSocketFd(newClient_fd);
        clients.push_back(currentClient);
    }
}

void Server::run(void)
{
	while (running)
	{
		this->setFds();
		int activity = select(_maxfd + 1, &_readfds, NULL, NULL, NULL);
		if (activity < 0) {
			std::cerr << "Error in select" << std::endl;
			continue;
		}
		if (FD_ISSET(_socketFd, &_readfds))
			acceptConnection();
		ClientCommunication();
	}
}

void    Server::creatingServer(Server &server)
{
	int	sockOpt = 1;

	_address.sin_family = AF_INET;
	// INADDR_ANY is a special IP address that tells the socket to listen on all available network interfaces.
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
	_addrlen = sizeof(_address);
	_socketFd = socket(AF_INET, SOCK_STREAM, 0); // setting up socket ((domain)AF_INET - IPv4) ((type)SOCK_STREAM - tcp PROTOCOL) (0 - default for TCP)
	checkError(_socketFd, "socket failed", "Error: Failed to create the server socket"); 
	checkError(setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(int)), "setsockopt","Error: Failed to set socket options"); // making the socket/port reusable 
	checkError(fcntl(_socketFd, F_SETFL, O_NONBLOCK), "fcntl failed", "Error setting socket flags"); //setting socket flag and making it non-block
	checkError(bind(_socketFd, (struct sockaddr *)&_address, _addrlen), "bind failed","Error setting socket flags"); // binding to the socket
	checkError(listen(_socketFd, 500), "listen", "Error: Failed to start listening for incoming connections"); // letting all the clients know that its available for connection
	std::cout << "Server started and listening for incoming connections on port " << _port << std::endl;
	// waiting for client connection
	server.run();
}

/*
int Server::handleAuthentication(std::string message, Client **client) {
	// if (((*client)->getState() == UNAUTHENTICATED)) {
	// 	if (message.substr(0, 5) == "PASS ") {
	// 		if (message.substr(5) == this->getPassword()) {
	// 			(*client)->setState(AUTHENTICATED);
	// 		} else {
	// 			(*client)->write("Incorrect Password\r\n");
	// 			return 1;
	// 		}
	// 	}
	// }
	// if (message.substr(0, 5) == "USER ") {
	// 		std::stringstream ss(message.substr(5));
	// 		std::string username, realName, permission;
	// 		char mode;
	// 		ss >> username >> mode >> permission >> realName;
	// 		if (ss.fail() || username.empty() || permission.empty() || realName.empty())
	// 			(*client)->write(ERR_NEEDMOREPARAMS);
	// 		else {
	// 			if (mode > '8' && mode < '0')  
	// 				(*client)->write("Invalid arguments\r\n");
	// 			else {
	// 				(*client)->setUserName(username);
	// 				(*client)->setRealName(realName);
	// 				//(*client)->setWaitingForUsername(true);
	// 			}
	// 		}
	// 	}
	return 0;
}
*/

void Server::disconnected(Client *&client, int socket) {
	if (client != nullptr) {
		if (!client->getNickName().empty())
			std::cout << client->getNickName() << " disconnected." << std::endl;
		close(client->getSocketFd());
		delete client;
		client = nullptr; // To prevent further access to the deleted pointer
		out = true;
	}
}

// int Server::Commands(Client **client, int socket, std::string commandStr)
// {
// 	for (size_t i = 0; i < commands.size(); i++) {
// 		// if (commandStr.fi)
// 		if (commandStr.substr(0, commands[i].label.size()) == commands[i].label) {
// 			std::vector<std::string> params = split(commandStr.substr(commands[i].label.size() + 1), ' ');
// 			if ((*client)->getState() >= commands[i].requiredAuthState)
// 			{
// 				if (commandStr.substr(0, 5) == "JOIN ")
// 					commands[i].handler(this, *client, params);
// 				return 0;
// 			}
// 		}
// 	}
// 	return 1;
// }

void   Server::disconnectClient(int socket, const std::string reason)
{
	Client *client;
	for(std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->getSocketFd() == socket)
		{
			client = *it;
			std::map<std::string, Channel *> :: iterator cit = channels.begin();  // Deleting client from all channels
			while (cit != channels.end())
			{
				cit->second->broadcast(":" + client->getNickName()+ " QUIT :" + reason +"\r\n", client);
				cit->second->removeClient(client);
				if(cit->second->getClients().size() == 0)
					channels.erase(cit++);
				else
					++cit;	
			}
			clients.erase(it);
			FD_CLR(client->getSocketFd(), &_readfds);
			close(client->getSocketFd());
			delete client;
			break;
		}
	}
}

void Server::ClientCommunication()
 {
	out = 0;
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end();)
	 {
		Client* client = *it;
     	if (FD_ISSET(client->getSocketFd(), &getReadfds())) 
		{
			char buffer[1024];
			memset(buffer,0, sizeof(buffer));
			ssize_t bytesReceived = recv(client->getSocketFd(), buffer, sizeof(buffer) - 1, 0);
			buffer[bytesReceived] = '\0';
			try
			{
				std::vector<std::string> messages = split(buffer,'\n');
				for(std::vector <std::string> :: iterator it = messages.begin(); it != messages.end(); ++it)
				{
					std::string line = *it;
					if(line[0] == '/')
					      line.erase(0,1);
					std::vector <Command> :: iterator cmd = commands.begin();
						while(cmd != commands.end())
						{
							if (line.rfind(cmd->label, 0) == 0) 
							{
								std::vector <std::string> params = split(line.substr(cmd->label.size() + 1),' ');
								if (line.find("QUIT") == 0)
									out = 1;
								if(cmd->requiredAuthState == UNAUTHENTICATED)
									cmd->handler(*this, *client, params);
								else if(cmd->requiredAuthState == AUTHENTICATED && client->getState() != UNAUTHENTICATED)
									cmd->handler(*this, *client, params);
								else if(cmd->requiredAuthState == REGISTERED && client->getState() == REGISTERED)
									cmd->handler(*this, *client, params);
								else
									client->write(":ft_irc.server 451 : You have not registered\r\n ");
							}
							++cmd;
						}
				}
			}
			catch(const std::exception& e)
			{
					std::cerr << e.what() << '\n';
			}
		}
		if (out == 1)
			continue;
		else
			it++;
	}
 }

void Server::registerChannel(Channel *channel)
{
	channels[channel->getName()] = channel;
}

Channel * Server::getChannel(std::string &name)
{
	std::map<std::string, Channel *> :: iterator it = channels.find(name);
	if(it != channels.end())
		return (it->second);
	return NULL;		
}

void Server::deleteChannel(Channel *channel)
{
	std::string channelName = channel->getName();

	delete channel;
	channels.erase(channelName);
}