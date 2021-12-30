#include "history.h"

lklist history={NULL,NULL};

void update_history(char *line) {
    if (no_history(line)) return;


    char *copy;
    strpeck(line,&copy,NULL,strlen(line));

    lkappend(&history,copy);
    size_t len=lklen(&history);

    //History stores a first dummy command that repeats the next
    //unless there are other 10 commands
    if (len>11)
        free(lkquit(&history));
}