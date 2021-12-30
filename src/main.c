#include "main.h"

void freecmd(cmd *c) {
    if (c->conditioncmd!=NULL) freecmd(c->conditioncmd);
    if (c->thencmd!=NULL) freecmd(c->thencmd);
    if (c->elsecmd!=NULL) freecmd(c->elsecmd);

    if(c->chain!=NULL) freecmd(c->chain);
    else if(c->pipe!=NULL) freecmd(c->pipe);

    if(c->extoutput!=NULL) free(c->extoutput);
    if(c->extinput!=NULL) free(c->extinput);

    if(c->argv!=NULL) {
        for (size_t i = 0; i < c->argc; i++) {
            if (c->argv[i]!=NULL) {
                free((c->argv)[i]);
            }
        }
        free (c->argv);
    }

    free(c);
}
void loop() {
    while (TRUE)
    {
        fflush(stdin);
        fflush(stdout);
        char dir[512]; 
        getcwd(dir,512); //Get working directory

        printf ("\e[1;32m" "%s \e[1;0m $ " ,dir); //Print wd at start of the line	
        //Read the input
        char *line=readline(stdin);

        update_history(line);

        //Parse the line of the input and get information
        //of how to execute
        cmd *main_cmd=parse(line);

        if (!(main_cmd->options&CMD_CONDITIONAL) && main_cmd->argv[0]==NULL) {
            free(line),
            freecmd(main_cmd);
            continue;
        }

        //Execute the command line
        cmd_handler *h=NEW(cmd_handler,1);
        h->pid=0;
        h->complex=0;
        h->cmd_data=NULL;
        *h=execute(main_cmd, STDIN_FILENO, STDOUT_FILENO);

        //Check if errored
        if (h->pid==0) {
            fprintf(stderr, "Invalid command: \"%s\".\n",line);
        } else {
            //Check if is builtin
            if (h->pid>0) {
                //Check if is ment to be background
                if (h->cmd_data->options & CMD_BACKGROUND) {
                    char*copy_line=NULL;
                    strpeck(line,&copy_line,NULL,strlen(line));
                    
                    job *j=NEW(job,1);
                    j->pid=h->pid;
                    j->line=copy_line;
                    
                    addjob(j);
                } else {
                    waitpid(h->pid, NULL, 0);
                    freecmd(main_cmd);
                }
            } else {
            }
        }
        free(h);

        free(line);
        fflush(stdin);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]) {
    loop();
}