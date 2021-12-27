#ifndef _CMDEXECUTE_H
#define _CMDEXECUTE_H

#include "types.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

//Main execute function of the shell
cmd_handler execute(cmd *command, int in_fd, int out_fd);

#endif