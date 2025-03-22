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
	commands.push_back(Command("PING", handlePing, REGISTERED));
	commands.push_back(Command("QUIT", handleQuit, UNAUTHENTICATED));
	commands.push_back(Command("WHO", handleWho, REGISTERED));
	commands.push_back(Command("KICK", handleKick, REGISTERED));
	commands.push_back(Command("PRIVMSG", handlePrivMsg, REGISTERED));
	// commands.push_back(Command("CAP", handleCap, UNAUTHENTICATED));
	// commands.push_back(Command("PONG", handlePong, UNAUTHENTICATED));
}


// ✅ Ensure These Are Defined Before Using Them
void handlePass(Server *server, Client *client, std::vector<std::string>& params) {
	if(params.empty())
	{
		client->write("Not enough parameters \r\n");
		return;
	}
	if(client->getState() == AUTHENTICATED)
	{
		client->write("You cannot reregister. \r\n");
		return;
	}
	if(params[0] != server->getPassword())
	{
		client->write("Incorrect Password.\r\n");
		return;
	}
	client->setState(AUTHENTICATED);
}

void handleNick(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}

void handleUser(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}

void handlePart(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handleInvite(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handleMode(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handlePing(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handleQuit(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handleWho(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handleKick(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
}
void handlePrivMsg(Server *server, Client *client, std::vector<std::string>& params) {
	// Function logic
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

int Server::handleAuthentication(std::string message, Client **client) {
	if (((*client)->getState() == UNAUTHENTICATED)) {
		if (message.substr(0, 5) == "PASS ") {
			if (message.substr(5) == this->getPassword()) {
				(*client)->setState(AUTHENTICATED);
			} else {
				(*client)->write("Incorrect Password\r\n");
				return 1;
			}
		}
	}
	if ((*client)->getState() == AUTHENTICATED) {
		if (message.substr(0, 5) == "NICK ") {
			std::string nickname = message.substr(5);
			nick(this, *client, nickname);
		}
		if (message.substr(0, 5) == "USER ") {
			std::stringstream ss(message.substr(5));
			std::string username, realName, permission;
			char mode;
			ss >> username >> mode >> permission >> realName;
			if (ss.fail() || username.empty() || permission.empty() || realName.empty())
				(*client)->write(ERR_NEEDMOREPARAMS);
			else {
				if (mode > '8' && mode < '0')  
					(*client)->write("Invalid arguments\r\n");
				else {
					(*client)->setUserName(username);
					(*client)->setRealName(realName);
					//(*client)->setWaitingForUsername(true);
				}
			}
		}
	}
	return 0;
}

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

int Server::Commands(Client **client, int socket, std::string commandStr)
{
	for (size_t i = 0; i < commands.size(); i++) {
		// if (commandStr.fi)
		if (commandStr.substr(0, commands[i].label.size()) == commands[i].label) {
			std::vector<std::string> params = split(commandStr.substr(commands[i].label.size() + 1), ' ');
			if ((*client)->getState() >= commands[i].requiredAuthState)
			{
				if (commandStr.substr(0, 5) == "JOIN ")
					commands[i].handler(this, *client, params);
				return 0;
			}
		}
	}
	return 1;
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
					std::vector <Command> :: iterator cmd = commands.begin();
						while(cmd != commands.end())
						{
							if ((line.substr(0, cmd->label.size()) == cmd->label)) 
							{
								std::vector <std::string> params = split(line.substr(cmd->label.size() + 1),' ');
								if(cmd->requiredAuthState == UNAUTHENTICATED)
									cmd->handler(this, client, params);
								else if(cmd->requiredAuthState == AUTHENTICATED && client->getState() != UNAUTHENTICATED)
									cmd->handler(this, client, params);
								else if(cmd->requiredAuthState == REGISTERED && client->getState() == REGISTERED)
									cmd->handler(this, client, params);
								else
									client->write(":ft_irc.server 451 : You have not registered\r\n ");
							}
							++cmd;
						}
				}
			}catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
	
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