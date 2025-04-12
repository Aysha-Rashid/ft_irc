# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"

void handleTopic(Server &server, Client &client, std::vector<std::string> &params)
{
    if(params.empty())
    {
        client.write(":"+ server.getServerName()+ " 461 " + client.getNickName() + " TOPIC :Not enough parameters\r\n");
        return;
    }
    
    std::string name = params[0];
    Channel *channel = server.getChannel(name);

    if(!channel){
            client.write(":"+server.getServerName()+" 403 "+ client.getNickName() + " " + name +  " :No such channel!\r\n");
            return;
    }
    else if (!channel->isClientInChannel(&client)) {
        client.write(":" + server.getServerName() + " 442 " + client.getNickName() + " " + channel->getName() + " :You're not on that channel\r\n");
        return;
    }

    if(params.size() == 1)
    {
        if(channel->getTopic().empty())
            client.write(":" + server.getServerName() + " 331 " + client.getNickName() + " " + channel->getName() + " :No topic is set\r\n");  
        else
            client.write(":" + server.getServerName() + " 332 " + client.getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");
        return;    
    }   
    
    if(channel->getTopicPrivilege() && !channel->isOperator(&client)){
        client.write(":" + server.getServerName() + " 482 " + client.getNickName() + " " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }

    //setting new topic
    std::string newTopic = "";
    if(params.size() >= 2)
    {
        newTopic = params[1];
        if(newTopic[0] == ':')
            newTopic.erase(0,1);

        for(size_t i = 2; i < params.size(); ++i)
                newTopic += " " + params[i];    
    }

    channel->setTopic(newTopic);
    channel->broadcast(":" + client.getPrefix() + " " + " TOPIC " + channel->getName() + " :" + newTopic + "\r\n");

}
