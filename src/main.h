#ifndef _MAIN_H
#define _MAIN_H

#include "commons.h"
#include "types.h"
#include "cmdexecute.h"
#include "cmdreadline.h"
#include "cmdparse.h"
#include "lklist.h"

#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//Initialize Shell
void init();
//Main loop of th Shell
void loop();

#endif