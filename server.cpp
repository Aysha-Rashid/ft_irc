#include "Ft_Irc.hpp"
# include "Server.hpp"

Server::Server(std::string name) : _serverName(name) {}
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
    // Client startClient;
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

int Server::Commands(Client **client, int socket, std::string commands)
{
	if (commands.substr(0, 5) == "JOIN ")
	    {
		join(this, *client, commands);
		}
	else if (commands.substr(0, 5) == "QUIT")
		disconnected((*client), (*client)->getSocketFd());
	else
		return 0;
	return (1);
}

void Server::ClientCommunication() {
	out = 0;
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end();) {
		Client* client = *it;
		if (FD_ISSET(client->getSocketFd(), &getReadfds())) {
			char buffer[1024];
			size_t bytesReceived = recv(client->getSocketFd(), buffer, sizeof(buffer) - 1, 0);
			if (bytesReceived == 0) {
				Commands(&client, client->getSocketFd(), "QUIT");
				it = clients.erase(it);
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
				if (handleAuthentication(message, &client))
				{
					Commands(&client, client->getSocketFd(), "QUIT");
					it = clients.erase(it);
					break;
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
					if (message == "QUIT")
					{
						Commands(&client, client->getSocketFd(), "QUIT");
						it = clients.erase(it);
						break; 
					}
					else if (message.substr(0, 5) == "JOIN ")
						Commands(&client, client->getSocketFd(), message);
				// 	if (client->getJoinChannel())
				// 	{
				// 		std::string broadcastMsg = client->getNickName() + ": " + message + "\r\n";
				// 		client->broadcastMessage(this, client, broadcastMsg);
				// 		std::cout << broadcastMsg;
				// }
					else
					{
						std::string my_message = "Error(421): " + message + " UNKNOWN COMMAND\r\n";
						send(client->getSocketFd(), my_message.c_str(), my_message.length(), 0);
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