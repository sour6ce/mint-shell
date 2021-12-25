#ifndef _BUILTIN_H
#define _BUILTIN_H

#include "commons.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BI_COUNT 4

int cd(int argc, char **argv);
int cmd_exit(int argc, char**argv);
int cmd_true(int argc, char**argv);
int cmd_false(int argc, char**argv);

//List of built-in commands names.
extern char *builtin_name[BI_COUNT];
//List of built-in commands.
extern bi_cmd builtin[BI_COUNT];

//Get index of the command in the built-in commands list
int get_bi_index(char *cmd_name);

#endif