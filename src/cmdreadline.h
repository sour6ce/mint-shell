#ifndef _CMDREADLINE_H
#define _CMDREADLINE_H

#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "commons.h"
#include "types.h"
#include "utils.h"

//Read the entire line
char *readline(FILE *input);

#endif