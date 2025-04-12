# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"

void handleJoin(Server &server, Client &client, std::vector<std::string>  &params)
{
    if(params.empty()){
        client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " JOIN :Not enough parameters\r\n");
        return;
    }

  std::vector<std::string> channels = split(params[0], ',');
  std::vector<std::string> keys;
  if(params.size() == 2)
        keys = split(params[1],',');

  for(size_t i = 0; i < channels.size(); i++){

    std::string name = channels[i];
    std::string channelKey = keys.size() > i ? keys[i] : "" ;

    if(client.getChannelCount() >= MAX_CHANNEL_COUNT){
        client.write(":"+server.getServerName()+" 405 "+ client.getNickName() + " " + name +  " :You have joined too many channels!\r\n");
        return;
    }
    if(name[0] !='#' && name[0] != '&'){
        client.write(":"+server.getServerName()+" 403 "+ client.getNickName() + " " + name +  " :No such channel!\r\n");
        return;
    }


    Channel *channel = server.getChannel(name);

        // creating a new channel
       if(!channel)
        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
        channel = new Channel(name, channelKey);
        server.registerChannel(channel);
        channel->addClient(&client);
        channel->addOperator(&client);
        std::string users;
        std::vector<std::string> nicknames = channel->getNickNames();
        for(std::vector<std::string> :: iterator it = nicknames.begin();it != nicknames.end(); ++it)
                  users.append(*it + " ");

        client.write(":" + client.getPrefix() + "@" + server.getServerName() + " JOIN " + name + "\r\n");
        client.write(":"+ server.getServerName() + " MODE " + channel->getName() + " +t\r\n");       
        channel->setTopicPrivilege(true);  
        client.write(":"+ server.getServerName() + " 353 " + client.getNickName()+ " = " + channel->getName() + " :" + users + "\r\n");
        client.write(":"+ server.getServerName() + " 366 " + client.getNickName()+ " " + channel->getName() + " :End of /NAMES list.\r\n");
        std::cout << ":" << client.getPrefix() + "@" + server.getServerName() + " JOIN " + name << "\r\n";
        channel->broadcast(":" + client.getPrefix() + "@" + server.getServerName() + " JOIN " + name + "\r\n", &client);
        continue;
        }  
    
        //channel exists
        if(channel->isClientInChannel(&client)){
            client.write(":" + server.getServerName() + " 443 " + client.getNickName() + " " + name + "  :is already on channel.\r\n");
            return;
        }
        if(channel->getUserLimit() > 0 && channel->getClients().size() > channel->getUserLimit()){
            client.write(":"+ server.getServerName() + " 471 " + client.getNickName() + "  " + name + " :Cannot join channel (+l) \r\n");
            return;
        }  
        if(!channel->getChannelKey().empty()){
              if(channelKey.empty() || channel->getChannelKey() != channelKey)
              {
                client.write(":"+ server.getServerName() + " 475 " + client.getNickName() + "  " + name + " :Cannot join channel (+k) - bad key\r\n");
                return;
              }  
        }
        if(channel->getInviteOnly() && !channel->isInvited(&client)){
          client.write(":"+ server.getServerName() + " 473 " + client.getNickName() + "  " + name + " :Cannot join channel (+i) - you must be invited\r\n");
          return;
        }
     
        //Add client to channel
        channel->addClient(&client);
        std::string users;
        std::vector<std::string> nicknames = channel->getNickNames();
        for(std::vector<std::string> :: iterator it = nicknames.begin();it != nicknames.end(); ++it)
               users.append(*it + " ");

        client.write(":" + client.getPrefix() + "@" + server.getServerName() + " JOIN " + name + "\r\n");
        if(channel->getTopic().empty())
            client.write(":" + server.getServerName() + " 331 " + client.getNickName() + " " + channel->getName() + " :No topic is set\r\n");  
        else
            client.write(":" + server.getServerName() + " 332 " + client.getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");
        
        client.write(":"+ server.getServerName() + " 353 " + client.getNickName()+ " = " + channel->getName() + " :" + users + "\r\n");
        client.write(":"+ server.getServerName() + " 366 " + client.getNickName()+ " " + channel->getName() + " :End of /NAMES list.\r\n");
        std::cout << ":" << client.getPrefix() + "@" + server.getServerName() + " JOIN " + name << "\r\n";
        channel->broadcast(":" + client.getPrefix() + "@" + server.getServerName() + " JOIN " + name + "\r\n", &client);
   }
}