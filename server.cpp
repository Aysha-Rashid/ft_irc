
# include "Ft_Irc.hpp"
# include "Server.hpp"

Server::Server(std::string name) : _serverName(name) {
	commands.push_back(Command("PASS", handlePass, UNAUTHENTICATED)); //done
	commands.push_back(Command("NICK", handleNick, AUTHENTICATED)); // done
	commands.push_back(Command("USER", handleUser, AUTHENTICATED)); // done
	commands.push_back(Command("JOIN", handleJoin, REGISTERED)); // done but the real implementation is left
	commands.push_back(Command("PART", handlePart, REGISTERED));
	commands.push_back(Command("INVITE", handleInvite, REGISTERED));
	commands.push_back(Command("MODE", handleMode, REGISTERED));
	commands.push_back(Command("PING", handlePing, REGISTERED));
	commands.push_back(Command("WHO", handleWho, REGISTERED));
	commands.push_back(Command("KICK", handleKick, REGISTERED));
	commands.push_back(Command("PRIVMSG", handlePrivMsg, REGISTERED));
	// commands.push_back(Command("CAP", handleCap, UNAUTHENTICATED));
	// commands.push_back(Command("PONG", handlePong, UNAUTHENTICATED));
}


// ✅ Ensure These Are Defined Before Using Them
void handlePass(const Server &server, const Client &client, std::vector<std::string>& params) 
{
	if (client->getState() == UNAUTHENTICATED) {
        if (params.size() < 2) {
            return;
        }
        if (params[1] == server->getPassword())
            client->setState(AUTHENTICATED);
	}
}

void handleUser(const Server &server, const Client &client, std::vector<std::string>& params) {
	if (client->getState() == AUTHENTICATED && !client->getNickName().empty() && client->getUserName().empty()) {
		std::string username, realName, mode, permission;
		username = params[1];
		mode = params[2];
		permission = params[3];
		realName = params[4];
		if (mode.size() > 1 || mode.empty() || username.empty() || permission.empty() || realName.empty())
			client->write(ERR_NEEDMOREPARAMS);
		else {
			if (isdigit(*mode.c_str()) < 0 || isdigit(*mode.c_str()) > 8)  
				client->write("Invalid arguments\r\n");
			else {
				client->setUserName(username);
				client->setRealName(realName);
			}
		}
	}
}

void handlePart(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handleInvite(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handleMode(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handlePing(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}
// void handleQuit(const Server &server, const Client &client, std::vector<std::string>& params) {
// 	// Function logic
// }


void handleWho(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handleKick(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handlePrivMsg(const Server &server, const Client &client, std::vector<std::string>& params) {
	// Function logic
}

// void handleCap(Server *server, int client_fd, std::vector<std::string>& params) {
//     // Function logic
// }
// void handlePong(Server *server, Client *client, std::vector<std::string>& params) {
//     // Function logic
// 	(void) server;
// 	(void) params;
// 	(void) client;
// }


void   Server::deleteClient(int socket)
{
	for(std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		// should state the reason for quiting
		if ((*it)->getSocketFd() == socket)
		{
			std::string message;
			if (!(*it)->getNickName().empty())
				message = (*it)->getNickName() + " disconnected.\r\n";
			else
				message = "client " + std::to_string((*it)->getSocketFd()) + " disconnected.\r\n";
			send((*it)->getSocketFd(), message.c_str(), message.length(), 0);
			std::cout << message;
			FD_CLR((*it)->getSocketFd(), &_readfds);
            close((*it)->getSocketFd());
            delete *it;
            clients.erase(it);
            break;
		}
	}
}

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


void    Server::creatingServer(Server &server)
{
	int	sockOpt = 1;

	_address.sin_family = AF_INET;
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
	server.run();
}

void Server::setFds()
{
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

void Server::acceptConnection(void)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int newClient_fd = accept(_socketFd, (struct sockaddr*)&client_addr, &client_len);
	if (newClient_fd >= 0)
	{
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
		if (activity < 0)
 			continue;
		if (FD_ISSET(_socketFd, &_readfds))
			acceptConnection();
		ClientCommunication();
	}
	close(_maxfd);
}

int Server::Commands(Client *client, std::string commandStr)
{
	for (size_t i = 0; i < commands.size(); i++) {
		if (commandStr.substr(0, commands[i].label.size()) == commands[i].label) {
			std::vector<std::string> params = split(commandStr.substr(commands[i].label.size()), ' ');
			if (client->getState() >= commands[i].requiredAuthState)
			{
				commands[i].handler(this, client, params);
				return 0;
			}
		}
	}
	if (client->getState() == REGISTERED)
	{
		std::string my_message = "Error(421): " + commandStr + " UNKNOWN COMMAND\r\n";
		send(client->getSocketFd(), my_message.c_str(), my_message.length(), 0);
		std::cout << my_message;
	}
	return 1;
}

void Server::ClientCommunication() {
	out = false;
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end();) {
		Client* client = *it;
		if (FD_ISSET(client->getSocketFd(), &getReadfds())){
			char buffer[1024];
			size_t bytesReceived = recv(client->getSocketFd(), buffer, sizeof(buffer) - 1, 0);
			if (bytesReceived == 0) {
				deleteClient(client->getSocketFd());
				out = true;
				continue;
			}
			if (bytesReceived < 0) {
				deleteClient(client->getSocketFd());
				out = true;
				perror("recv error");
				continue;
			}
			buffer[bytesReceived] = '\0';
			std::string receivedData(buffer);
			client->inputBuffer += receivedData;
			size_t newlinePos;
			while ((newlinePos = client->inputBuffer.find('\n')) != std::string::npos && client) // to handle the ctrl+d buffer
			{
				std::string message = client->inputBuffer.substr(0, newlinePos);
				if (message[0] == '/')
					message = message.substr(1);
				client->inputBuffer.erase(0, newlinePos + 1); // Remove processed part
				if (!message.empty() && message.back() == '\r')
					message.pop_back();
				if (message.empty()) continue;
				if (client->getState() <= AUTHENTICATED)
				{
					Commands(client, message);
					if (client->getState() == UNAUTHENTICATED && message.substr(0, 5) == "PASS ")
					{
						std::cout << "Authentication failed for client " << client->getSocketFd() << std::endl;
						std::string errorMsg = "Authentication failed. Disconnecting...\r\n";
						send(client->getSocketFd(), errorMsg.c_str(), errorMsg.length(), 0);
						deleteClient(client->getSocketFd());
						out = true;
						break;
					}
				}
				if (client->getState() == AUTHENTICATED && !client->getNickName().empty() && !client->getUserName().empty()) {
					client->setState(REGISTERED);
					std::string welcomeMsg = ":" + getServerName() + " 001 " + client->getNickName() + " :Welcome to the IRC server, " + client->getNickName() + "\r\n";
					// server 001 is a numeric reply code used by the IRC server to indicate that the client has successfully connected.
					send(client->getSocketFd(), welcomeMsg.c_str(), welcomeMsg.length(), 0);
					std::cout << welcomeMsg;
				}
				else if (client->getState() == REGISTERED)
				{
					if (message == "QUIT :")
					{
						deleteClient(client->getSocketFd());
						out = true;
						break;
					}
					Commands(client, message);
				}
				// std::cout << message << "\n";
			}
			if (out == true)
				continue ;
		}
		++it;
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