#include "builtin.h"
#include "main.h"

int cd(int argc, char **argv) {
    return chdir(argv[1])!=0;
}
int cmd_exit(int argc, char**argv){
    exit(EXIT_SUCCESS);
}
int cmd_true(int argc, char**argv) {
    return EXIT_SUCCESS;
}
int cmd_false(int argc, char**argv) {
    return EXIT_FAILURE;
}
void __print_history1(size_t index, node * n) {
    if(index!=0)
        printf("%d:%s\n",index,(char *)n->data);
}
void __print_history2(size_t index, node * n) {
    printf("%d:%s\n",index+1,(char*)n->data);
}
int cmd_history(int argc, char**argv) {
    lkfor(&history,((lklen(&history)<=10)?__print_history2 : __print_history1) );
    return EXIT_SUCCESS;
}
int cmd_again(int argc, char**argv) {
    size_t len=lklen(&history);
    size_t c_i=len-2;

    free(lkrm(&history,len-1));
    len--;
    if (c_i==0) {
        return EXIT_FAILURE;
    }
    if (argc>1) {
        errno=0;
        int val=strtol(argv[1],NULL,10);
        if (errno==0) {
            c_i=MAX(MIN(c_i,val-1),0) ;
        }
    }
    char *copy=NULL;
    char *old_line=lkndat(&history,c_i)->data;

    strpeck(old_line,&copy,NULL,strlen(old_line));

    lkappend(&history,copy);

    //NOTE:This code beneath is copied from man.c
    //Parse the line of the input and get information
    //of how to execute
    cmd *main_cmd=parse(copy);

    //Execute the command line
    cmd_handler *h=NEW(cmd_handler,1);
    h->pid=0;
    h->complex=0;
    h->cmd_data=NULL;
    *h=execute(main_cmd, STDIN_FILENO, STDOUT_FILENO);

    //Check if errored
    if (h->pid==0) {
        printf("Invalid command line.\n");
    } else {

        //Check if is builtin
        if (h->pid>0) {
            //Check if is ment to be background
            if (h->cmd_data->options & CMD_BACKGROUND) {
                //TODO: Implement addjob(cmd_handler*)
                //addjob(h);
            } else {
                waitpid(h->pid, NULL, 0);
                //fflush(stdout);
                freecmd(main_cmd);
            }
        } else {
        }
    }
    int stat=good_exit(*h);

    free(h);

    return stat;
}
char *builtin_name[BI_COUNT]={
    "cd",
    "exit",
    "true",
    "false",
    "history",
    "again"
};
bi_cmd builtin[BI_COUNT]={
    cd,
    cmd_exit,
    cmd_true,
    cmd_false,
    cmd_history,
    cmd_again
};

int get_bi_index(char *cmd_name) {
    for (size_t i = 0; i < BI_COUNT; i++)
    {
        if (strcmp(cmd_name,builtin_name[i])==0)
        {
            return i;
        }
    }
    return -1;    
}
