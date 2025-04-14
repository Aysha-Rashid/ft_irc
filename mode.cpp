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

    if(!channel){
            client.write(":"+server.getServerName()+" 403 "+ client.getNickName() + " " + target +  " :No such channel!\r\n");
            return;
    }
    std::cout << "channel mode= "<< channel->getChannelMode() << std::endl;
    if(params.size() < 2){
        client.write(":" + server.getServerName() + " 324 " + client.getNickName() + " " + target + " :+" + channel->getChannelMode() + "\r\n");
        return;
    }
    
    std::vector <std::string> reqArgs;
    for (size_t i = 2; i < params.size(); i++)
              reqArgs.push_back(params[i]);
   
    bool isAdded;
    for (size_t i = 0; i < params[1].size(); i++)
    {
        /*TODO: check for multiple + or - */
       switch (params[1][i])
       {
        case '+':  isAdded = true;
                   break;
        case '-':  isAdded = false;
                   break;
        case 'i':  // invite-only channel
                    if(!channel->isOperator(&client))   
                    {
                        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
                        return;
                    }  
                    if(isAdded)
                        channel->setInviteOnly(true);
                    else
                        channel->setInviteOnly(false);   
                    channel->broadcast(":" + client.getPrefix() + " MODE " + target + (isAdded ? " +i" : " -i") + "\r\n");     
                    break;  
        case 't':  // set topic
                    if(isAdded)
                        channel->setTopicPrivilege(true);
                    else
                        channel->setTopicPrivilege(false);   
                    channel->broadcast(":" + client.getPrefix() + " MODE " + target + (isAdded ? " +t" : " -t") + "\r\n");     
                    break; 
           
        case 'l':  // set userlimit
                    if(reqArgs.empty() && isAdded)
                    {
                        client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
                        return;
                    }
                    if(!channel->isOperator(&client))   
                    {
                        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
                        return;
                    }  
                    if(isAdded)
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
                        }
                    else
                        channel->setUserLimit(0);   
                    if(isAdded)    
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " +l " + reqArgs[0] + " \r\n");   
                    else if(!isAdded && reqArgs.size() > 0)  
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " -l *\r\n");     
                    if(reqArgs.size() > 0)
                        reqArgs.erase(reqArgs.begin());
                    break;   
        case 'k':  // set channel key
                    if(reqArgs.empty() && isAdded)
                    {
                        client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
                        return;
                    }
                    if(!channel->isOperator(&client))   
                    {
                        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
                        return;
                    }       
                    if(isAdded)
                        channel->setChannelKey(reqArgs[0]);
                    else
                        channel->setChannelKey("");   
                    if(isAdded)    
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " +k " + reqArgs[0] + " \r\n");   
                    else if(!isAdded && reqArgs.size() > 0)  
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " -k *\r\n");   
                    if(reqArgs.size() > 0)
                        reqArgs.erase(reqArgs.begin());   
                    break;    
        case 'o':  // grant or revoke operator privilege
                    if(reqArgs.empty())
                    {
                        client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " MODE :Not enough parameters\r\n");
                        return;
                    }
                    if(!channel->isOperator(&client))   
                    {
                        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + target + " :You're not channel operator\r\n");
                        return;
                    }  
                    Client *targetClient = channel->getClientWithNickname(reqArgs[0]);
                    if(!targetClient)
                    {
                        client.write(":" + server.getServerName() + " 441 " + client.getNickName() + " " + target + " :They aren't on that channel\r\n");
                        return; 
                    }
                    if(isAdded)
                        channel->addOperator(targetClient);                                      
                    else
                        channel->removeOperator(targetClient);   
                    if(isAdded)    
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " +o " + reqArgs[0] + " \r\n");   
                    else if(!isAdded && reqArgs.size() > 0)  
                        channel->broadcast(":" + client.getPrefix() + " MODE " + target + " -o " + reqArgs[0] + " \r\n");   
                    reqArgs.erase(reqArgs.begin());   
                    break; 
    // default:        client.write(":" + server.getServerName() + " 501 " + client.getNickName() + " " + target + " :Unknown MODE flag\r\n");
    //                 return;
       
       }
    } 
}