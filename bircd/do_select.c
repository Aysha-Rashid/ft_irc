
#include <stdlib.h>
#include "bircd.h"

void	do_select(t_env *e)
{
  e->r = select(e->max + 1, &e->fd_read, &e->fd_write, NULL, NULL);
  // waits for an event (read/write) on the active file descriptors.
  // select() monitors multiple file descriptors and tells us which are ready
  // selects()returns the number of active FDs
  // if (e->r > 0)
  //    process ready socket for reading or writing
}
