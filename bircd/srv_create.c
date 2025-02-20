
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include "bircd.h"

void			srv_create(t_env *e, int port)
{
  int			s;
  struct sockaddr_in	sin;
  struct protoent	*pe;
  
  pe = (struct protoent*)Xv(NULL, getprotobyname("tcp"), "getprotobyname"); // retrieve protocol number dynamically
  s = X(-1, socket(PF_INET, SOCK_STREAM, pe->p_proto), "socket");
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  // accepts connection on any network interface
  sin.sin_port = htons(port);
  // convert port into network byte order
  X(-1, bind(s, (struct sockaddr*)&sin, sizeof(sin)), "bind");
  // bind socket to ip address and port
  X(-1, listen(s, 42), "listen");
  // tells the system the socket is ready for connection
  e->fds[s].type = FD_SERV;
  e->fds[s].fct_read = srv_accept;
}
