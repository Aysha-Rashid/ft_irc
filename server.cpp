#include "Ft_Irc.hpp"
Server::Server(std::string name) : _serverName(name) {}
Server::~Server()
{
    std::string quitMsg = "server QUIT :Server shutting down\n";

    if (!clients.empty())
    {
        for (size_t i = 0; i < clients.size(); i++) {
            int clientSocket = clients[i]->getSocket();
            
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
        FD_SET(client->getSocket(), &_readfds);
        if (client->getSocket() > _maxfd) {
            _maxfd = client->getSocket();
        }
    }
}

void Server::acceptConnection(void)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newClient = accept(_socketFd, (struct sockaddr*)&client_addr, &client_len);

    if (newClient >= 0){
        std::cout << "New client attempting to connect: " << newClient << std::endl;
        fcntl(newClient, F_SETFL, O_NONBLOCK);

        // Create client object and mark as unauthenticated
        // Send password prompt (ClientCommunication() will handle receiving)
        std::string passwordPrompt = "Enter server password: ";
        send(newClient, passwordPrompt.c_str(), passwordPrompt.length(), 0);
        Client *currentClient = new Client();
        currentClient->setSocket(newClient);
        currentClient->authenticated = false;  // New field in `Client` class
        clients.push_back(currentClient);

    }
}

void Server::broadcastMessage(int sender, const std::string &message) {
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client* client = *it;
        if (client->getSocket() != sender) {
            send(client->getSocket(), message.c_str(), message.length(), 0);
        }
    }
}

void Server::ClientCommunication()
{
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); )
    {
        Client* client = *it;
        if (FD_ISSET(client->getSocket(), &_readfds))
        {
            char buffer[1024];
            int bytesReceived = recv(client->getSocket(), buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0)
            {
                std::cout << "Client " << client->getSocket() << " disconnected." << std::endl;
                close(client->getSocket());
                delete client;
                it = clients.erase(it);
                continue;
            }
            buffer[bytesReceived] = '\0';
            std::string message(buffer);
            message.erase(message.find_last_not_of("\r\n") + 1);

            if (!client->authenticated)
            {
                if (message == _password)
                {
                    client->authenticated = true;
                    std::string successMsg = "Authentication successful. Welcome!\n";
                    send(client->getSocket(), successMsg.c_str(), successMsg.length(), 0);
                    std::cout << "Client " << client->getSocket() << " authenticated.\n";
                }
                else
                {
                    std::string errorMsg = "Incorrect password. Connection closed.\n";
                    send(client->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
                    close(client->getSocket());
                    delete client;
                    it = clients.erase(it);
                    continue;
                }
            }
            else
            {
                // Message for other clients
                std::string broadcastMsg = "Client " + std::to_string(client->getSocket()) + ": " + message + "\n";
                broadcastMessage(client->getSocket(), broadcastMsg);
                // Server logs (same as broadcast message)
                std::cout << broadcastMsg;
            }
        }
        ++it;
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
        // Check if there's a new connection
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
