#include "commons.h"
#include "types.h"

#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BI_COUNT 4

#pragma region Help Functions
//Find the first position in wish c character appear. If order is 1 search in reverse.
size_t charfind(char *str,size_t start,size_t n,char c,int order);
//Concatenates the second string at the end of the first and store the result in a new string.
char * strpaste(char *first,char*second, size_t *len);
//Divides the string in two parts, the second part first character is the one in pos.
void strpeck(char*str,char**first,char**second,size_t pos);
//Insert the second string into the first one in the position declared and stores
//the result into a new string.
char *strinsert(char*first,char*second,size_t pos);
//Replace every character c in str by character r
void strrep(char*str,size_t n,char c, char r);

//Check if the process has terminated successfully
int checkp(cmd_handler h, BOOL hang);
#pragma endregion

#pragma region Built-In
int cd(int argc, char **argv);
int cmd_exit(int argc, char**argv);
int cmd_true(int argc, char**argv);
int cmd_false(int argc, char**argv);

extern char *builtin_name[BI_COUNT];
extern bi_cmd builtin[BI_COUNT];

//Get index of the command in the built-in commands list
int get_bi_index(char *cmd_name);
#pragma endregion

#pragma region Main Functions
//Initialize Shell
void init();
//Read the entire line
char *readline(FILE *input);
//Execute command
cmd_handler execute(cmd *command, int in_fd, int out_fd);
#pragma endregion