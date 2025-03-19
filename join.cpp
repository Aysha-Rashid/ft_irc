# include "Ft_Irc.hpp"
# include "Server.hpp"
# include "Client.hpp"

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

void join(Server *server, Client *client, std::string &message)
{
   if(message.empty())
     {
        client->write(":ft_irc.server 461 " + client->getNickName() + "JOIN :Not enough parameters \r\n");
        return;
     }
     std::vector<std::string> channels = split(message, ' ');
     std::string name = channels[1];
     
     Channel *channel = server->getChannel(name);
     if(!channel)
     {
       channel = new Channel(name, "");
       server->registerChannel(channel);
       channel->addClient(client);
       channel->addOperator(client);
       client->write(" JOIN " + channel->getName() + "\r\n");
       channel->broadcastExclude(" JOIN " + channel->getName() + "\r\n", client);
     }
     else
     {
      channel->addClient(client);
      client->write(" JOIN " + channel->getName() + "\r\n");
      channel->broadcastExclude(" JOIN " + channel->getName() + "\r\n", client);
     }
}