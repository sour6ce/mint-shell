#include "cmdexecute.h"

void __enfile(char *path) {
    FILE *fd=fopen(path, "w");
    fclose(fd);
}

BOOL good_exit(cmd_handler h) {
    if (h.pid<0)
        return !(h.pid+2);
    else {
        int ex_status;
        waitpid(h.pid,&ex_status,0);
        return (WIFEXITED(ex_status)&&(!WEXITSTATUS(ex_status)) );
    }
}

int wait_status(cmd_handler h) {
    if (h.pid<0)
        return !(h.pid+2);
    else {
        int ex_status;
        waitpid(h.pid,&ex_status,0);
        return ex_status;
    }
}

cmd_handler execute(cmd *command, int in_fd, int out_fd) {
    cmd_handler h;
    h.pid=0;

    unsigned char options=command->options;

    //A complex handler is a handler for a forked process that execute two commands (for piping or chain)
    h.complex=(options&CMD_CHAINEDRIGHT) || (options&CMD_PIPEDRIGHT) || (options&CMD_CONDITIONAL);
    h.cmd_data=command;
    
    //If complex fork into a 2-command executor
    if (h.complex) {
        h.pid=fork();
        if (h.pid==-1) //Error in fork
        {
            fprintf(stderr,"Failed child process creation. \n");
        } else if (h.pid==0) { //Child process
            //printf("Complex Child pid:%d with parent: %d.\n",getpid(),getppid());
            //printf("Info:\n\tinput fd:%d\n\toutput fd:%d\n\tcmd:%s\n",in_fd,out_fd,command->argv[0]);

            int ex_status=0;
            if (options&CMD_CHAINEDRIGHT) { //If is chained

                //First command of the chain without the connection to the chain. This is to be seen by
                //execute as a unchained process
                cmd left=*command;
                left.options=options ^ CMD_CHAINEDRIGHT;
                left.chain=NULL;

                //The rest of the chain
                cmd right=*(command->chain);

                cmd_handler h1=execute(&left,in_fd, out_fd); //Execute first command in chain (first part)
                
                ex_status=wait_status(h1);

                if (command->chain_type==CMD_CHAIN_NORMAL || //If the chain is normal (uses ';')
                (command->chain_type==CMD_CHAIN_OR && (!(WIFEXITED(ex_status)&&(!WEXITSTATUS(ex_status)) ) ) ) || //If the chain is an OR and the the first command did not succeed
                (command->chain_type==CMD_CHAIN_AND && (WIFEXITED(ex_status)&&(!WEXITSTATUS(ex_status)) ) ) //If the chain is an AND and the first command succeed
                )  {
                    cmd_handler h2=execute(&right,in_fd,out_fd); //Execute the second part of the chain
                    ex_status=wait_status(h2);
                }
                exit(ex_status);
            }
            if (options&CMD_PIPEDRIGHT) {// If is piped
                int pipe_fd[2];
                if (pipe(pipe_fd)==-1) {
                    fprintf(stderr,"Error piping. \n");
                }

                cmd left=*command;
                left.pipe=NULL;
                left.options=options ^ CMD_PIPEDRIGHT;

                cmd right=*(command->pipe);

                cmd_handler h1=execute(&left,in_fd,pipe_fd[1]);
                ex_status=wait_status(h1);

                close(pipe_fd[1]);

                cmd_handler h2=execute(&right,pipe_fd[0],out_fd);
                ex_status=wait_status(h2);

                close(pipe_fd[0]);

                exit(ex_status);
            }
            if (options&CMD_CONDITIONAL) {// If is a condition
                cmd *_cond=command->conditioncmd;
                cmd *_then=command->thencmd;
                cmd *_else=command->elsecmd;

                cmd_handler ch=execute(_cond,in_fd,out_fd);
                int ex_status;

                BOOL good=good_exit(ch);
                if (good) {
                // waitpid(ch.pid,&ex_status,0);
                // if ((WIFEXITED(ex_status)&&(!WEXITSTATUS(ex_status)) )) {
                    cmd_handler thenh=execute(_then,in_fd,out_fd);

                    ex_status=wait_status(thenh);
                } else {
                    if (options&CMD_ELSECOND) {
                        cmd_handler elseh=execute(_else,in_fd,out_fd);
                    
                        ex_status=wait_status(elseh);
                    }
                }
                exit(ex_status);
            }
        }
    } else {
        if(command->argv[0]==NULL) return h;
        int bi=get_bi_index(command->argv[0]);
        if (bi!=-1) {
            int es=builtin[bi](command->argc,command->argv);
            h.pid=es-2;

            return h;
        }
        
        h.pid=fork();
        if (h.pid==-1) //Error in fork
        {
            fprintf(stderr,"Failed child process creation. \n");
        } else if (h.pid==0) { //Child process
            //printf("Simple Child pid:%d with parent: %d.\n",getpid(),getppid());
            //printf("Info:\n\tinput fd:%d\n\toutput fd:%d\n\tcmd:%s\n",in_fd,out_fd,command->argv[0]);

            int fi=-1,fo=-1;

            if (command->options&CMD_EXTINPUT) {
                fi=open(command->extinput, O_RDONLY);
                dup2(fi,STDIN_FILENO);
                close(fi);
            } else {
                dup2(in_fd,STDIN_FILENO);
            }
            if (command->options&CMD_EXTOUTPUT) { //TODO: Check why the file is locked on finish
                if (!(options&CMD_HARDEXTOUTPUT) ) {
                    __enfile(command->extoutput);
                    fo=open(command->extoutput, O_WRONLY);
                    dup2(fo,STDOUT_FILENO);
                    close(fo);
                } else {
                    fo=open(command->extoutput, O_WRONLY | O_APPEND);
                    dup2(fo,STDOUT_FILENO);
                    close(fo);
                }
            } else {
                dup2(out_fd,STDOUT_FILENO);
            }

            if (execvp(command->argv[0],command->argv)<0) { //Execute the simple command
                fprintf(stderr, "Could not execute the command %s. \n",command->argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }
    return h; //Return the handler, complex or not
}
