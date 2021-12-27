#include "cmdexecute.h"

cmd_handler execute(cmd *command, int in_fd, int out_fd) {
    cmd_handler h;
    h.pid=0;

    unsigned char options=command->options;

    //A complex handler is a handler for a forked process that execute two commands (for piping or chain)
    h.complex=(options&CMD_CHAINEDRIGHT) || (options&CMD_PIPEDRIGHT);
    h.cmd_data=command;
    
    //If complex fork into a 2-command executor
    if (h.complex) {
        h.pid=fork();
        if (h.pid==-1) //Error in fork
        {
            fprintf(stderr,"Failed child process creation. \n");
        } else if (h.pid==0) { //Child process
            printf("Complex Child pid:%d with parent: %d.\n",getpid(),getppid());
            printf("Info:\n\tinput fd:%d\n\toutput fd:%d\n\tcmd:%s\n",in_fd,out_fd,command->argv[0]);

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
                
                waitpid(h1.pid,&ex_status,0);

                if (command->chain_type==CMD_CHAIN_NORMAL || //If the chain is normal (uses ';')
                (command->chain_type==CMD_CHAIN_OR && (!(WIFEXITED(ex_status)&&(!WEXITSTATUS(ex_status)) ) ) ) || //If the chain is an OR and the the first command did not succeed
                (command->chain_type==CMD_CHAIN_AND && (WIFEXITED(ex_status)&&(!WEXITSTATUS(ex_status)) ) ) //If the chain is an AND and the first command succeed
                )  {
                    cmd_handler h2=execute(&right,in_fd,out_fd); //Execute the second part of the chain
                    waitpid(h2.pid,&ex_status,0);
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
                waitpid(h1.pid,&ex_status,0);

                close(pipe_fd[1]);

                cmd_handler h2=execute(&right,pipe_fd[0],out_fd);
                waitpid(h2.pid,&ex_status,0);

                close(pipe_fd[0]);

                exit(ex_status);
            }
        }
    } else {
        h.pid=fork();
        if (h.pid==-1) //Error in fork
        {
            fprintf(stderr,"Failed child process creation. \n");
        } else if (h.pid==0) { //Child process
            printf("Simple Child pid:%d with parent: %d.\n",getpid(),getppid());
            printf("Info:\n\tinput fd:%d\n\toutput fd:%d\n\tcmd:%s\n",in_fd,out_fd,command->argv[0]);

            int fi=-1,fo=-1;

            if (command->options&CMD_EXTINPUT) {
                fi=open(command->extinput, O_RDONLY);
                dup2(fi,STDIN_FILENO);
                close(fi);
            } else {
                dup2(in_fd,STDIN_FILENO);
            }
            if (command->options&CMD_EXTOUTPUT) { //TODO: Check why the file is locked on finish
                fo=open(command->extoutput, O_WRONLY|((command->options&CMD_HARDEXTOUTPUT)?O_CREAT : 0));
                dup2(fo,STDOUT_FILENO);
                close(fo);
            } else {
                dup2(out_fd,STDOUT_FILENO);
            }

            if (execvp(command->argv[0],command->argv)<0) { //Execute the simple command
                fprintf(stderr, "Could not execute the command %s. \n",command->argv[0]);
            }
        }
    }
    return h; //Return the handler, complex or not
}
