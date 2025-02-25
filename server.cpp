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
        Client *currentClient = new Client();
        currentClient->setSocket(newClient);
        currentClient->authenticated = false;
        clients.push_back(currentClient);
    }
}

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
            return false;
        }
    }
    return true;
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
void Client::ClientCommunication(Server *server) {
    for (std::vector<Client *>::iterator it = server->clients.begin(); it != server->clients.end();) {
        Client* client = *it;

        if (FD_ISSET(client->getSocket(), &server->getReadfds())) {
            char buffer[1024];
            int bytesReceived = recv(client->getSocket(), buffer, sizeof(buffer) - 1, 0);

            if (bytesReceived <= 0) {
                std::cout << "Client " << client->getSocket() << " disconnected." << std::endl;
                close(client->getSocket());
                delete client;
                it = server->clients.erase(it);
                continue;
            }
            buffer[bytesReceived] = '\0';
            std::string message(buffer);
            message.erase(message.find_last_not_of("\r\n") + 1);

            if (!client->authenticated)
            {
                if (message.substr(0, 5) == "PASS ")
                {
                    if (message.substr(5) == server->getPassword())
                        client->authenticated = true;
                    else
                    {
                        client->waitingForUsername = false;
                        client->waitingForNickName = false; 
                    }
                }
            }
            if (client->authenticated && (!client->waitingForUsername || !client->waitingForNickName)) {
                if (message.substr(0, 5) == "NICK ") {
                    if (message.empty()) 
                        printClientError(client->getSocket(), ERR_NONICKNAMEGIVEN);
                    else if (!isValidNickName(message.substr(5)))
                        printClientError(client->getSocket(), ERR_ERRONEUSNICKNAME);
                    else if (!isNickNameInUse(server, message.substr(5)))
                        printClientError(client->getSocket(), ERR_NICKNAMEINUSE);
                    else
                    {
                        client->setNickName(message.substr(5));
                        client->waitingForNickName = true;
                    }
                }
                if (client->waitingForNickName)
                {
                    if (message.substr(0, 5) == "USER ") {
                        std::stringstream ss(message.substr(5));
                        std::string username, realName, permission;
                        int mode = 0;
                        ss >> username >> mode >> permission >> realName;
                        if (ss.fail() || username.empty() || permission.empty() || realName.empty())
                            printClientError(client->getSocket(), ERR_NEEDMOREPARAMS);
                        else
                        {
                            // if (!isdigit(mode))  
                            //     printClientError(client->getSocket(), "Invalid arguments\r\n");
                            // else
                            // {
                                client->setUserName(username);
                                client->setRealName(realName);
                                client->waitingForUsername = true;
                            // }
                        }
                    }
                }
            } 
            if (client->authenticated && client->waitingForUsername && client->waitingForNickName){
                // for other server.clients and server (general chats)
                std::string broadcastMsg = client->getUserName() + ": " + message + "\r\n";
                broadcastMessage(server, client->getSocket(), broadcastMsg);
                std::cout << broadcastMsg;
            }
            // else
            //     std::cout << message << std::endl;
        }
        ++it;
    }
}


void Server::run(void)
{
    Client startClient;
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
        startClient.ClientCommunication(this);
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
