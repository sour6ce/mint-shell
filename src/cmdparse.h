#ifndef _CMDPARSE_H
#define _CMDPARSE_H

#include "utils.h"
#include "types.h"
#include "lklist.h"

//Token definitions

#define TOK_IF "if"
#define TOK_IF_LEN 2

#define TOK_THEN "then"
#define TOK_THEN_LEN 4

#define TOK_ELSE "else"
#define TOK_ELSE_LEN 4

#define TOK_END "end"
#define TOK_END_LEN 3

#define TOK_CHAIN_NORMAL ";"
#define TOK_CHAIN_NORMAL_LEN 1

#define TOK_CHAIN_AND "&&"
#define TOK_CHAIN_AND_LEN 2

#define TOK_CHAIN_OR "||"
#define TOK_CHAIN_OR_LEN 2

#define TOK_PIPE "|"
#define TOK_PIPE_LEN 1

#define TOK_IN "<"
#define TOK_IN_LEN 1

#define TOK_OUT ">"
#define TOK_OUT_LEN 1

#define TOK_HARDOUT ">>"
#define TOK_HARDOUT_LEN 2

#define TOK_BACKGROUND "&"
#define TOK_BACKGROUND_LEN 1

//3-sized tuple for stores balance of if/then/else/end expressions
typedef int int3[3];

//Map the balance of quotation marks in string, result array stores 1 if character in that
//position is inside quotation marks (not include the marks themselves), 0 otherwise.
int *mapquote(char *str);

//Find the firs occurence of the token inside str that is't part of a string literal.
char *tokfind(char *str,size_t n, char *token);

//Same that tokfind but in reverse.
char *tokfindr(char *str, size_t n, char *token);

//Map the balance of if/then/else/end expressions. Each position of the array stores 3 integers:
//How many if are open until that position.
//How many then are open until that position.
//How many else are open until that position.
int3 *mapcond(char *str);

//Returns TRUE if the balance map given of a condition is valid, FALSE otherwise.
BOOL validcond(int3 *map, size_t n);

//Stores the ranges of the if, then and else expresion with the bigger priority.
//Return value is -1 if is not a valid mapping, 0 if the condition doesn't have an
//else expression and 1 if is valid and complete.
int rangecond(int3 *map, size_t n,range if_range,range then_range, range else_range);

//Find the position of any token avoiding those inside quotation marks or conditional scope. 
//Similar to charfindm.
//size_t truetok(char *str, size_t start, size_t n,char *cs,int order);

//Expands to a condition with value true if the command line skips history.
#define no_history(line) ((line)[0]==' ')

//Gets the length of the token according chain type
#define chainsz(type) ((type)==CMD_CHAIN_OR)? TOK_CHAIN_OR_LEN :\
     ((type)==CMD_CHAIN_AND)? TOK_CHAIN_AND_LEN : TOK_CHAIN_NORMAL_LEN

//Stores in the index the position of the first chain symbol and if not if is found.
//In case to be found stores in type the type of chain symbol finded. If not found
//stores -1 in index.
void chaintoken(char *line, size_t n, char **index, int *type);

//Main function for parsing, returns command information ready to execute.
cmd *parse(char *line);

#endif