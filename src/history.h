#ifndef _HISTORY_H
#define _HISTORY_H

#include "commons.h"
#include "utils.h"

#include "lklist.h"

#define HISTORY_SIZE 10

//List with the last 10 commands typed.
extern lklist history;

//Expands to a condition with value true if the command line skips history.
#define no_history(line) ( ((line)[0]==' ') || ((line)[0]=='\0') )

//Function to add a line to the history.
void update_history(char *line);

#endif