# include "Server.hpp"
# include <ctype.h>

Server::Server(std::string name) : _serverName(name) {
	commands.push_back(Command("PASS", handlePass, UNAUTHENTICATED));
	commands.push_back(Command("NICK", handleNick, AUTHENTICATED));
	commands.push_back(Command("USER", handleUser, AUTHENTICATED));
	commands.push_back(Command("JOIN", handleJoin, REGISTERED));
	commands.push_back(Command("PART", handlePart, REGISTERED));
	commands.push_back(Command("INVITE", handleInvite, REGISTERED));
	commands.push_back(Command("MODE", handleMode, REGISTERED));
	commands.push_back(Command("TOPIC", handleTopic, REGISTERED));
	commands.push_back(Command("PING", handlePing, REGISTERED));
	commands.push_back(Command("QUIT", handleQuit, UNAUTHENTICATED));
	commands.push_back(Command("WHO", handleWho, REGISTERED));
	commands.push_back(Command("KICK", handleKick, REGISTERED));
	commands.push_back(Command("PRIVMSG", handlePrivMsg, REGISTERED));
	commands.push_back(Command("PONG", handlePong, UNAUTHENTICATED));
}

Server::~Server()
{
    std::string quitMsg = "server QUIT :Server shutting down\n";

	if (!clients.empty())
	{
		for (size_t i = 0; i < clients.size(); i++) {
			int clientSocket = clients[i]->getSocketFd();
			
            if (clients[i]->getChannelCount() > 0)
            {
                for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
                    delete it->second;
                channels.clear();
            }
			if (clientSocket > 0) {
				send(clientSocket, quitMsg.c_str(), quitMsg.length(), 0);
				close(clientSocket);
			}
			delete clients[i];
		}
        clients.clear();
	}
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
	if (this->_port < 6665 ||  this->_port > 6669)
		throw std::runtime_error("Port should be within 6665 and 6669");
	this->_password = password;
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

void   Server::disconnectClient(int socket, const std::string reason)
{
	for(std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->getSocketFd() == socket)
		{
			Client *client = *it;
			std::map<std::string, Channel *> :: iterator chanIter = channels.begin();
			while (chanIter != channels.end())
			{
				if(chanIter->second->isClientInChannel(client))
				{
					chanIter->second->broadcast(":" + client->getNickName()+ " QUIT " + reason + "\r\n", client);
					chanIter->second->removeClient(client);
				}
				if(chanIter->second->getClients().size() == 0)
				{
					delete chanIter->second;
					channels.erase(chanIter++);
				}
				else
					++chanIter;	
			}
		    if (client)
            {
                FD_CLR(client->getSocketFd(), &_readfds);
                close(client->getSocketFd());
                delete client;
            }
            clients.erase(it);
			break;
		}
	}
}

void Server::ClientCommunication()
 {
	
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end();)
	 {
        out = 0;
		Client* client = *it;
        if (FD_ISSET(client->getSocketFd(), &getReadfds())) 
		{
			char buffer[1024];
			memset(buffer,0, sizeof(buffer));
			ssize_t bytesReceived = recv(client->getSocketFd(), buffer, sizeof(buffer) - 1, 0);
			if (bytesReceived <= 0) {
                disconnectClient(client->getSocketFd(), "QUIT");
                continue;
            }            
			buffer[bytesReceived] = '\0';
			try
			{
				std::string line(buffer);
                client->_receiveBuffer += line;
                size_t pos;
                while ((pos = client->_receiveBuffer.find('\n')) != std::string::npos && out == 0) {
                    line = client->_receiveBuffer.substr(0, pos);
					if (!line.empty() && line[line.size() - 1] == '\r')
                        line = line.substr(0, line.size() - 1);
					int cmd_length = 0;
					for(size_t i=0; i < line.length(); i++)
					{
						line[i] = toupper((char)line[i]);
						if(line[i] == ' ')
						{
							cmd_length =i;
							break;
						}
					}
					if(line[0] == '/')
						line.erase(0,1);
                    client->_receiveBuffer.erase(0, pos + 1);
					std::vector <Command> :: iterator cmd = commands.begin();
					while(cmd != commands.end())
						{
							if (line.rfind(cmd->label, 0) == 0) 
							{
								if(line.size() == cmd->label.size())
									line += " ";
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
									client->write(":" + this->getServerName() + " 451 * :You have not registered\r\n");
								break;	
							}		
							++cmd;
						}
						if(cmd == commands.end() && client->getState() == REGISTERED)
									client->write(":" + this->getServerName() +" 421 " + client->getNickName() + " " + line.substr(0, cmd_length) + " :Unknown Command\r\n");
                        if (out == 1)
                            break ;
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