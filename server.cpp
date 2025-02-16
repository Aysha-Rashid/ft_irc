#include "Ft_Irc.hpp"

Server::Server(std::string name) : _serverName(name) {}
Server::~Server()
{
    if (!this->clients.empty())
    {
        for (size_t i = 0; i < clients.size(); i++) {
            int clientSocket = clients[i]->getSocket();
            // send(clientSocket, quitMsg.c_str(), quitMsg.length(), 0);
            // Maybe if client will exits on its own from their side then we dont need to send them the message
            // maybe Delete the client in its function?? or should we delete them here when server is down
            if (clientSocket > 0)
              close(clientSocket);
            std::cout << ":server QUIT :cleaning up kids && Server shutting down\r\n";
        }
    }
    else
        std::cout << ":server QUIT :Server shutting down\r\n";
    clients.clear();
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

void    Server::creatingServer(Server &server)
{
  int	sockOpt = 1;
  int _addrlen;
  _address.sin_family = AF_INET;
  // INADDR_ANY is a special IP address that tells the socket to listen on all available network interfaces.
  _address.sin_addr.s_addr = INADDR_ANY;
  _address.sin_port = htons(_port);
	_addrlen = sizeof(_address);
	if ((_socketFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		throw std::runtime_error("Error: Failed to create the server socket");
	}
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(int)) < 0)
	{
		perror("setsockopt");
		throw std::runtime_error("Error: Failed to set socket options");
	}
	if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << strerror(errno) << std::endl;
		throw std::runtime_error("Error setting socket flags");
	}
	if (bind(_socketFd, (struct sockaddr *)&_address, _addrlen) < 0)
  {
		perror("bind failed");
		throw std::runtime_error("Error: Failed to bind the server socket to a port");
	}
	if (listen(_socketFd, 500) < 0) // i have set a default max of 500 connection
	{
		perror("listen");
		throw std::runtime_error("Error: Failed to start listening for incoming connections");
	}
	std::cout << "Server started and listening for incoming connections on port " << _port << std::endl;
}
