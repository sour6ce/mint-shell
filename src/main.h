#ifndef _MAIN_H
#define _MAIN_H

#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "commons.h"
#include "types.h"
#include "cmdexecute.h"
#include "cmdreadline.h"
#include "cmdparse.h"
#include "lklist.h"
#include "jobs.h"

//Initialize Shell.
void init();
//Free the command information allocated memory.
void freecmd(cmd *c);
//Main loop of th Shell.
void loop();

#endif