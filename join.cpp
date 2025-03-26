# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"

/*

The JOIN command indicates that the client wants to join the given channel(s), each channel using the given key for it.
The server receiving the command checks whether or not the client can join the given channel, and processes the request. 
Servers MUST process the parameters of this command as lists on incoming message from clients, with the first <key>
being used for the first <channel>, the second <key> being used for the second <channel>, etc.


JOIN #foobar                    ; join channel #foobar.

JOIN &foo fubar                 ; join channel &foo using key "fubar".

JOIN #foo,&bar fubar            ; join channel #foo using key "fubar"
                                  and &bar using no key.

JOIN #foo,#bar fubar,foobar     ; join channel #foo using key "fubar".
                                  and channel #bar using key "foobar".

JOIN #foo,#bar                  ; join channels #foo and #bar.

*/

void handleJoin(Server &server, Client &client, std::vector<std::string>  &params)
{
   if(params.empty())
     {
        client.write(":"+server.getServerName()+ " 461 " + client.getNickName() + " JOIN :Not enough parameters \r\n");
        return;
     }
     std::vector<std::string> channels = split(params[0], ',');
     std::vector<std::string> keys;
     if(params.size() == 2)
          keys = split(params[1],',');
 
     for(size_t i = 0; i < channels.size(); i++)
     {
          
       std::string name = channels[i];
       std::string channelKey = keys[i].size() > i ? keys[i] : "" ;
              
       if(client.getChannelCount() >= MAX_CHANNEL_COUNT)
       {
        client.write(":"+server.getServerName()+" 405 "+ client.getNickName() +  ", You have joined too many channels!\r\n");
        return;
       }
       if(name[0] !='#' && name[0] != '&')
       {
        client.write(":"+server.getServerName()+" 403 "+ client.getNickName() +  ", : No such channel!\r\n");
        return;
       }
       Channel *channel = server.getChannel(name);
       if(!channel)
        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
        channel = new Channel(name, channelKey );
        server.registerChannel(channel);
        channel->addClient(&client);
        channel->addOperator(&client);
        std::string users;
        std::vector<std::string> nicknames = channel->getNickNames();
        std::vector<std::string> :: iterator it = nicknames.begin();
        while( it != nicknames.end())
        {
          users.append(*it + " ");
          ++it;
        }
        client.write(":" + client.getNickName() + " JOIN " + channel->getName() + "\r\n");
        client.write(":" +server.getServerName() + " MODE "+ channel->getName() + " +t\r\n");
        client.write(":"+ server.getServerName() + " 353 " + client.getNickName()+ " = " + channel->getName() + " :" + users + "\r\n");
        client.write(":"+ server.getServerName() + " 366 " + client.getNickName()+ " = " + channel->getName() + " :End of /NAMES list.\r\n");
        std::cout << ":" << client.getNickName() << " JOIN "<< channel->getName() << "\r\n";
        channel->broadcast(":" +client.getNickName()+ " JOIN " + channel->getName() + "\r\n", &client);
        continue;
        }
        if(channel->isClientInChannel(&client))
             {
               client.write(":ircserv.server 443 "+ client.getNickName() + " " + channel->getName()+ "  is already on channel.\r\n");
               return;
             }
        channel->addClient(&client);
        client.write(":" + client.getNickName() + " JOIN " + channel->getName() + "\r\n");
        channel->broadcast(":" +client.getNickName()+ " JOIN " + channel->getName() + "\r\n", &client);
        }
    }
