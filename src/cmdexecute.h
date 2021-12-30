#ifndef _CMDEXECUTE_H
#define _CMDEXECUTE_H

#include <sys/wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "types.h"
#include "builtin.h"

//Check if the handler is from a command that ends without errors. If it is not yet
//finished wait for it.
BOOL good_exit(cmd_handler h);

//Return the value of the builtin function if is a builtin or the exit status if is
//an executable.
int wait_status(cmd_handler h);

//Main execute function of the shell
cmd_handler execute(cmd *command, int in_fd, int out_fd);

#endif