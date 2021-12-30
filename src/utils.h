#ifndef _UTILS_H
#define _UTILS_H

#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "commons.h"
#include "types.h"

typedef size_t range[2];

//Find the first position in wish c character appear. If order is 1 search in reverse.
size_t charfind(char *str,size_t start,size_t n,char c,int order);

//Like charfind but find multiple possible characters.
size_t charfindm(char *str,size_t start,size_t n,char *cs,int order);

//Concatenates the second string at the end of the first and store the result in a new string.
char * strpaste(char *first,char*second, size_t *len);

//Divides the string in two parts, the second part first character is the one in pos.
void strpeck(char*str,char**first,char**second,size_t pos);

//Insert the second string into the first one in the position declared and stores.
//the result into a new string.
char *strinsert(char*first,char*second,size_t pos);

//Replace every character c in str by character r.
void strrep(char*str,size_t n,char c, char r);

//Puts the string in reverse.
void strrev(char *str,size_t n);

//Gets a new preffix substring of size n.
char *strsub(char *str, size_t n);


//Check if the process has terminated successfully.
int checkp(cmd_handler h, BOOL hang);

#endif