
# include "Commands.hpp"

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
	{
		client.setState(REGISTERED);
		client.write(":" + server.getServerName() +" 001 " + client.getNickName() + " :Welcome to the " + server.getServerName() + " Network, " +client.getNickName() + "\r\n");	
		std::cout << ":" + server.getServerName() +" 001 " + client.getNickName() + " :Welcome to the " + server.getServerName() + " Network, " +client.getNickName() + "\r\n";
	}	
}


void handlePart(Server &server, Client &client, std::vector<std::string>& params) {
	if (params.empty()) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " PART :Not enough parameters\r\n");
        return;
    }

    std::vector<std::string> channelNames = split(params[0], ',');
    std::string reason = params.size() > 1 ? "" : "Leaving";

    if(reason.empty())
		for(std::vector<std::string> ::iterator it = params.begin(); it != params.end(); ++it)		
			reason.append(*it + " ");
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

        bool wasOperator = channel->isOperator(&client);
        if (wasOperator) {
            channel->removeOperator(&client);
        }
        channel->broadcast(":" + client.getPrefix() + " PART " + channelName + " :" + reason + "\r\n");
        channel->removeClient(&client);
        
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
    Client *targetClient = NULL;
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
    targetClient->write(":" + client.getPrefix() + " INVITE " + targetNick + " " + channelName + "\r\n");
    client.write(":" + server.getServerName() + " 341 " + client.getNickName() + " " + targetNick + " " + channelName + "\r\n");
    channel->setInvited(targetClient);
}

void handlePing(Server &server, Client &client, std::vector<std::string>& params) {
	std::string msg;
    if (params.empty()) {
        msg = ":" + server.getServerName() + " 409 " + client.getNickName() + " :No origin specified\r\n";
        client.write(msg);
        return;
    }
    
    std::string token = params[0];
    msg = ":" + server.getServerName() + " PONG " + server.getServerName() + " :" + token + "\r\n";
    client.write(msg);
}


void handlePong(Server &server, Client &client, std::vector<std::string>& params) {    
    if (params.empty()) {
        std::string msg= ":" + server.getServerName() + " 409 " + client.getNickName() + " :No origin specified\r\n";
        client.write(msg);
        return;
    }
    client.updateLastActivity();
}

void handleQuit(Server &server, Client &client, std::vector<std::string>& params) {
	
	
	std::string reason = params.size() > 1 ? ""  : "Leaving...";
	
	if(reason.empty())
		for(std::vector<std::string> ::iterator it = params.begin(); it != params.end(); ++it)		
			reason.append(*it + " ");
	
	if(client.getState() == REGISTERED)
		server.disconnectClient(client.getSocketFd(),reason);
	else
	{
		if(client.getNickName().empty())
			client.write(":"+ server.getServerName() + " * QUIT :" + reason + "\r\n");
		else
			client.write(":"+ server.getServerName() + " " + client.getNickName() +" QUIT :" + reason + "\r\n");
		std::cout << ": "+ server.getServerName() + " " + client.getNickName() +" QUIT :" + reason + "\r\n";
	}
}

void handleWho(Server &server, Client &client, std::vector<std::string>& params) {
	if (params.empty()) {
        client.write(":" + server.getServerName() + " 461 " + client.getNickName() + " WHO :Not enough parameters\r\n");
        return;
    }

    std::string target = params[0];
    bool operFlag = (params.size() > 1 && params[1] == "o");

    if (target[0] == '#' || target[0] == '&') {
        Channel *channel = server.getChannel(target);
        if (channel) {
            std::vector<Client*> clients = channel->getClients();
            for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                Client *member = *it;
                if (operFlag && !channel->isOperator(member))
                    continue;
                std::string flags = "H";
                if (channel->isOperator(member))
                    flags += "@";
                client.write(":" + server.getServerName() + " 352 " + client.getNickName() + " " + 
                             channel->getName() + " " + member->getUserName() + " " + 
                             member->getIpAddress() + " " + server.getServerName() + " " + 
                             member->getNickName() + " " + flags + " :0 " + 
                             member->getRealName() + "\r\n");
            }
        }
    } else {
        for (std::vector<Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
            Client *cli = *it;
            if (cli->getNickName() == target || target == "*") {
                if (operFlag) {
                    bool isOp = false;
                    for (std::map<std::string, Channel*>::iterator ch = server.channels.begin(); ch != server.channels.end(); ++ch) {
                        if (ch->second->isOperator(cli)) {
                            isOp = true;
                            break;
                        }
                    }
                    if (!isOp) continue;
                }
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
    if (params.size() > 2) {
        reason = params[2];
        if (reason[0] == ':')
            reason = reason.substr(1);
        for (size_t i = 3; i < params.size(); i++) {
            reason += " " + params[i];
        }
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
    if (!channel->isOperator(&client)) {
        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }
    Client *targetClient = NULL;
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
    if (!channel->isClientInChannel(targetClient)) {
        client.write(":" + server.getServerName() + " 441 " + client.getNickName() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
        return;
    }
    std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    channel->broadcast(kickMsg);
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
        channel->broadcast(":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n", &client);
    } else {
        Client *targetClient = NULL;
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
        targetClient->write(":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n");
    }
}
