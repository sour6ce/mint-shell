#include "main.h"

#pragma region Help Functions
size_t charfind(char *str,size_t start,size_t n,char c,BOOL order) {
    for (size_t i = (order)? n-1 : start; (order)? i>start : i < n; (order)? i-- : i++) {
        if (str[i]==c) {
            return i;
        }
    }
    return n;
}
char * strpaste(char *first,char*second, size_t *len) {
    size_t sz=0;

    size_t f_size=strlen(first);
    size_t s_size=strlen(second);

    sz=f_size+s_size;

    char *result=malloc(sz+1);
    result[sz]='\00';
    
    for (size_t i = 0; i < f_size; i++)
    {
        result[i]=first[i];
    }
    for (size_t i = f_size; i < sz; i++) {
        result[i]=second[i-f_size];
    }
    
    if (len !=NULL) {
        *len=sz;
    }

    return result;
}
void strpeck(char*str,char**first,char**second,size_t pos) {
    size_t total_size=strlen(str);

    if (first!=NULL) {
        *first=malloc(pos+1);
        for (size_t i = 0; i < pos; i++)
        {
            (*first)[i]=str[i];
        }
        (*first)[pos]='\00';
    }
    if (second!=NULL)
    {
        *second=malloc(total_size-pos+1);
        for (size_t i = pos; i < total_size; i++)
        {
            (*second)[i-pos]=str[i];
        }
        (*second)[total_size-pos]='\00';
    }
}
char *strinsert(char*first,char*second,size_t pos) {
    char *s1,*s2;

    strpeck(first,&s1,&s2,pos);

    char *s3=strpaste(s1,second,NULL);
    char *s4=strpaste(s3,s2,NULL);

    free(s1);
    free(s2);
    free(s3);

    return s4;
}
void strrep(char*str,size_t n,char c, char r) {
    for (size_t i = 0; i < n; i++) {
        if (str[i]==c) {
            str[i]=r;
        }
    }
}

int checkp(cmd_handler h, BOOL hang) {
    int status;
    waitpid(h.pid,&status,(hang)? 0 : WNOHANG);
    return (!(WIFEXITED(status)&&(!WEXITSTATUS(status))));
}
#pragma endregion

#pragma region Built-In
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
char *builtin_name[BI_COUNT]={
    "cd",
    "exit",
    "true",
    "false"
};
bi_cmd builtin[BI_COUNT]={
    &cd,
    &cmd_exit,
    &cmd_true,
    &cmd_false
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
#pragma endregion

#pragma region Main Functions
void init() {
    
}
char *readline(FILE *input) {
    char *line="";
    char *temp=NULL;
    int first=1;
    size_t line_size=0;
    size_t temp_size=0;

    do {
        if (!first && input==stdin)
        {
            printf("->");
        }
        
        getline(&temp,&temp_size,input);
        char *newline=strpaste(line,temp,&line_size);

        free(temp);
        temp=NULL;

        if (!first)
        {
            free(line);
        }
        first=0;

        line=newline;
        
        size_t coms=charfind(line,0,line_size,'#',1);

        strpeck(line,&newline,NULL,coms);

        free(line);
        line=newline;

        size_t slash=charfind(line,0,line_size,'\\',1);

        if (slash==line_size) {
            break;
        } else {
            line[slash]=' ';
        }
    } while (1);
    strrep(line,strlen(line),'\n',' ');
    return line;
}
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
#pragma endregion

int main(int argc, char *argv[]) {
    #pragma region Old Execute Tests
    // cmd test1;
    // char *test1_args[]={"ls",NULL};
    // test1.argv=test1_args;
    // test1.argc=1;
    // test1.options=CMD_HARDEXTOUTPUT | CMD_EXTOUTPUT;
    // test1.extoutput="./a.txt";
    // test1.chain=NULL;
    // test1.pipe=NULL;

    // cmd right;
    // char *right_args[]={"cat",NULL};
    // right.argv=right_args;
    // right.argc=1;
    // right.options=CMD_PIPEDLEFT;
    // right.chain=NULL;
    // right.pipe=NULL;

    // cmd right1;
    // char *right1_args[]={"cat",NULL};
    // right1.argv=right1_args;
    // right1.argc=1;
    // right1.options=CMD_PIPEDLEFT;
    // right1.pipe=NULL;
    // right1.chain=NULL;
    // right1.pipe=NULL;

    // //test1.pipe=&right;
    // //right.pipe=&right1;

    // printf("Executing test1\n");
    // cmd_handler h=execute(&test1,STDIN_FILENO,STDOUT_FILENO);

    // int status=0;

    // printf("Before Wait\n");

    // waitpid(h.pid,&status,0);

    // printf("After Wait\n");

    // return 0;
    #pragma endregion

    #pragma region Old Input Tests
    // char s1[]="Hello ";
    // char s2[]="Wouddo";

    // size_t s3len=0;

    // char *s3=strpaste(s1,s2,&s3len);

    // printf("The result string is: \"%s\" and it has a size of: %d bytes.\n",s3,s3len);

    // char s4[]="Semato";

    // char *s5,*s6;

    // strpeck(s4,&s5,&s6,2);

    // printf("\"%s\" was pecked into: \"%s\" and \"%s\".\n",s4,s5,s6);

    // char*s7=strinsert(s4," Mr. ",0);

    // printf("Insert at the start: \"%s\".\n",s7);

    // char*s8=strinsert(s3,s7,5);

    // printf("Insert in middle: \"%s\".\n",s8);

    // char*s9=strinsert(s8,"!",strlen(s8));

    // printf("Insert at last position: \"%s\".\n",s9);

    // char*s10;
    
    // strpeck(s4,&s10,NULL,strlen(s4));

    // printf("Failed peck?: \"%s\".\n",s10);

    // int pass=3;

    // for (;pass>0; pass--) {
    //     char*line;
    //     line=readline(stdin);
    //     printf("Writed: \"%s\".\n",line);
    // }
    #pragma endregion
}