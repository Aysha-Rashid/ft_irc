# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "Utils.hpp"

void handleMode(Server &server, Client &client, std::vector<std::string> &params)
{
	
	if(params.empty())
	{
		client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
		return;
	}
	std::string target = params[0];
	Channel *channel = server.getChannel(target);
	if (!channel){
		client.write(":"+server.getServerName()+" 403 "+ client.getNickName() + " " + target +  " :No such channel!\r\n");
		return;
	}
	if (params.size() < 2){
		client.write(":" + server.getServerName() + " 324 " + client.getNickName() + " " + target + " :+" + channel->getChannelMode() + "\r\n");
		return;
	}
	if(!channel->isOperator(&client)){
		client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
		return;
	}
	std::vector <std::string> reqArgs;
	for (size_t i = 2; i < params.size(); i++)
		reqArgs.push_back(params[i]);
	bool isAdded;
	std::string channelModes = "";
	Client *targetClient;
	for (size_t i = 0; i < params[1].size(); i++)
	{
	   switch (params[1][i])
	   {
		case '+':  
					isAdded = true;
					break;
		case '-':
					isAdded = false;
					break;
		case 'i': 
					if(isAdded && channel->getInviteOnly() == false)
					{
						channel->setInviteOnly(true);
						channelModes.append("+i ");
					}
					else if (!isAdded)
					{
						channel->setInviteOnly(false);
						channelModes.append("-i ");
					}
					break;
		case 't':
					if(isAdded && channel->getTopicPrivilege() == false) // not sure
					{
						channel->setTopicPrivilege(true);
						channelModes.append("+t ");
					}
					else if (!isAdded)
					{
						channel->setTopicPrivilege(false);
						channelModes.append("-t ");
					}
					break;
		case 'l':
					if(reqArgs.empty() && isAdded)
					{
						client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
						return;
					}
					if(isAdded && channel->getUserLimit() == 0)
					{
						size_t limit;
						limit = stringToNumber(reqArgs[0]);
						if(limit >= INT_MAX ||( limit == 0 && reqArgs[0] != "0"))
						{
							client.write(":" + server.getServerName() + " 501 " + client.getNickName() + " " + target + " :Unknown MODE flag\r\n");
							return;
						}
						try
						{
							channel->setUserLimit(limit);
						}
						catch(const std::exception& e)
						{
							std::cerr << e.what() << '\n';
							client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
							return;
						}
						channelModes.append("+l " + reqArgs[0]);
					}
					else if(!isAdded && reqArgs.size() > 0)
					{
						channelModes.append("-l * ");
						channel->setUserLimit(0);
					}
					if(reqArgs.size() > 0)
						reqArgs.erase(reqArgs.begin());
					break;
		case 'k':
					if(reqArgs.empty() && isAdded)
					{
						client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
						return;
					}
					if(isAdded && channel->getChannelKey().empty())
					{
						channel->setChannelKey(reqArgs[0]);
						channelModes.append("+k " + reqArgs[0]);
					}
					else if (!isAdded && reqArgs.size() > 0 && channel->getChannelKey() == reqArgs[0])
					{
						channel->setChannelKey("");  
						channelModes.append("-k " + reqArgs[0]);
					}
					reqArgs.erase(reqArgs.begin());
					break;
		case 'o':
				{
					if(reqArgs.empty())
					{
						client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
						return;
					}
					targetClient = channel->getClientWithNickname(reqArgs[0]);
					if(!targetClient)
					{
						client.write(":" + server.getServerName() + " 441 " + client.getNickName() + " " + target + " :They aren't on that channel\r\n");
						return;
					}
					if(isAdded && !channel->isOperator(targetClient))
					{
						channel->addOperator(targetClient);
						channelModes.append("+o " + reqArgs[0]);
					}
					else
					{
						channel->removeOperator(targetClient);
						channelModes.append("+o " + reqArgs[0]);
					}
					break;
				}
		default:
				return;
		}
	}
	if(!channelModes.empty())
		channel->broadcast(":" + client.getPrefix() + " MODE " + target + " " + channelModes + " \r\n");
}