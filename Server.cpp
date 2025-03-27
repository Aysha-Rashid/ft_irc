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
void handlePass(Server &server, Client &client, std::vector<std::string>& params)
 {
	if(params.empty())
	{
		client.write(server.getServerName() + ERR_NEEDMOREPARAMS);
		return;
	}
	if(client.getState() == AUTHENTICATED || client.getState() == REGISTERED)
	{
		client.write(server.getServerName() + ERR_ALREADYREGISTERED);
		return;
	}
	if(params[0] != server.getPassword())
	{
		client.write(server.getServerName() + ERR_PASSWDMISMATCH);
		return;
	}
	client.setState(AUTHENTICATED);
}

void handleUser(Server &server, Client &client, std::vector<std::string>& params) 
{
	(void) server;
	if(client.getState() == REGISTERED)
	{
		client.write(server.getServerName() + ERR_ALREADYREGISTERED);
		return;
	}
	if(params.empty() || params.size() < 4)
	{
		client.write(server.getServerName() + ERR_NEEDMOREPARAMS);
		return;
	}

	client.setUserName(params[0]);
	client.setRealName(params[3]);
	if(!client.getNickName().empty())
		client.setState(REGISTERED);
	client.write(":" + server.getServerName() + " 001 " + client.getNickName() + RPL_WELCOME + client.getNickName() + "\r\n");	
	std::cout << " 001 " + client.getNickName() + " :Welcome " +client.getNickName() +" to the IRC server\r\n";
	}


void handlePart(Server &server, Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handleInvite(Server &server, Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handleMode(Server &server, Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handlePing(Server &server, Client &client, std::vector<std::string>& params) {
	// Function logic
}

void handleQuit(Server &server, Client &client, std::vector<std::string>& params) {
	
	std::string reason = params.empty() ? "Leaving..." : "";

	if(!params.empty())
		for(std::vector<std::string> ::iterator it = params.begin(); it != params.end(); ++it)		
				reason.append(*it + " ");
	
	if(client.getState() == REGISTERED)
	{
		if (reason.at(0) == ':')
			reason = reason.substr(1);
		server.disconnectClient(client.getSocketFd(),reason);

	}
	else
	{
		if(client.getNickName().empty())
			client.write(":"+ server.getServerName() + " * QUIT :" + reason + "\r\n");
		else
			client.write(":"+ server.getServerName() + " " + client.getNickName() +" QUIT :" + reason + "\r\n");
	}
	std::cout << ": "+ server.getServerName() + " " + client.getNickName() +" QUIT :" + reason + "\r\n";
}

void handleWho(Server &server, Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handleKick(Server &server, Client &client, std::vector<std::string>& params) {
	// Function logic
}
void handlePrivMsg(Server &server, Client &client, std::vector<std::string>& params) {
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

void   Server::disconnectClient(int socket, const std::string reason)
{
	for(std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->getSocketFd() == socket)
		{
			Client *client = *it;
			std::map<std::string, Channel *> :: iterator chanIter = channels.begin();  // Deleting client from all channels
			while (chanIter != channels.end())
			{
				chanIter->second->broadcast(":" + client->getNickName()+ " QUIT :" + reason +"\r\n", client);
				chanIter->second->removeClient(client);
				if(chanIter->second->getClients().size() == 0)
					channels.erase(chanIter++);
				else
					++chanIter;	
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
								if (!cmd->label.compare("QUIT"))
									out = 1;
								if(cmd->requiredAuthState == UNAUTHENTICATED)
									cmd->handler(*this, *client, params);
								else if(cmd->requiredAuthState == AUTHENTICATED && client->getState() != UNAUTHENTICATED)
									cmd->handler(*this, *client, params);
								else if(cmd->requiredAuthState == REGISTERED && client->getState() == REGISTERED)
									cmd->handler(*this, *client, params);
								else
									client->write(":" + this->getServerName() + " 451 : You have not registered\r\n ");
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