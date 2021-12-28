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
    // if (len==1 || len>=12) {
    //     char *dum_copy=NULL;
    //     char *first_cmd=lkndat(&history,0)->data;
    //     strpeck(first_cmd,&dum_copy,NULL,strlen(first_cmd));
    //     if (len>=11) free(lkquit(&history));
    //     lkins(&history,0,dum_copy);
    // }
}