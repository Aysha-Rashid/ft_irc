
#include "bircd.h"

int	main(int ac, char **av)
{
  t_env	e;

  init_env(&e); // initializing fd
  get_opt(&e, ac, av); // getting port
  srv_create(&e, e.port);
  main_loop(&e);
  return (0);
}
