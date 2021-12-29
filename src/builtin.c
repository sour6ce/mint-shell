#include "builtin.h"
#include "main.h"

int cd(int argc, char **argv) {
    return chdir(argv[1])!=0;
}
void __kill_jobs(size_t *index, node *n) {
    job j=*(job*)(n->data);
    pid_t ended=waitpid(j.pid,NULL,WNOHANG);
    if (ended) {
        free(j.line);
        lkrm(&jobs,*index);
        (*index)--;
    } else {
        kill(j.pid,SIGKILL);
    }
}
int cmd_exit(int argc, char**argv){
    lkfor(&jobs,__kill_jobs);
    exit(EXIT_SUCCESS);
}
int cmd_true(int argc, char**argv) {
    return EXIT_SUCCESS;
}
int cmd_false(int argc, char**argv) {
    return EXIT_FAILURE;
}
void __print_history1(size_t *index, node * n) {
    if(*index!=0)
        printf("%d:%s\n",*index,(char *)n->data);
}
void __print_history2(size_t *index, node * n) {
    printf("%d:%s\n",(*index)+1,(char*)n->data);
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
        long val=strtol(argv[1],NULL,10);
        if (errno==0) {
            c_i=MAX(MIN(c_i,val-1),0) ;
        }
    }
    char *copy=NULL;
    char *old_line=lkndat(&history,c_i)->data;

    strpeck(old_line,&copy,NULL,strlen(old_line));

    lkappend(&history,copy);

    //NOTE:This code beneath is copied from main.c
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
        fprintf(stderr, "Invalid command: \"%s\".\n",copy);
    } else {
        //Check if is builtin
        if (h->pid>0) {
            //Check if is ment to be background
            if (h->cmd_data->options & CMD_BACKGROUND) {
                char*copy_line=NULL;
                strpeck(copy,&copy_line,NULL,strlen(copy));
                
                job *j=NEW(job,1);
                j->pid=h->pid;
                j->line=copy_line;
                
                addjob(j);
            } else {
                waitpid(h->pid, NULL, 0);
                //fflush(stdout);
                freecmd(main_cmd);
            }
        } else {
        }
    }
    free(h);

    return stat;
}
int cmd_fg(int argc, char**argv) {
    size_t jlen=lklen(&jobs);
    //size_t i=jlen-1;
    pid_t p=0;
    // if (argc>1){
    //     errno=0;
    //     long a=strtol(argv[1],NULL,10);
    //     if (errno==0) {
    //         p=MAX(MIN(jlen-1,a-1),0) ;
    //     }
    // }

    // job *j=lkndat(&jobs,i)->data;
    // lkrm(&jobs,i);
    // free(j->line);
    // int status;
    // cmd_handler temp={0,j->pid,NULL};
    // return good_exit(temp);

    p=((job*)jobs.last->data)->pid;
    if(argc>1) {
        errno=0;
        long a=strtol(argv[1],NULL,10);
        if (errno==0) {
            p=a;
        }
    }

    node *actual=jobs.first;
    size_t ind=0;
    do {
        if (((job*)actual->data)->pid==p)
        {
            free(((job*)actual->data)->line);
            cmd_handler temp={0,((job*)actual->data)->pid,NULL};
            lkrm(&jobs,ind);
            ind--;
            return good_exit(temp);
        }
        actual=actual->next;
        ind++;
    } while (actual!=NULL);
    return EXIT_FAILURE;
}
// void __print_jobs(size_t *index, node *n) {
//     job j=*(job*)(n->data);
//     pid_t ended=waitpid(j.pid,NULL,WNOHANG);
//     if (ended) {
//         printf("[%d]:pid %d: DONE! %s\n",(*index)+1,j.pid,j.line);
//         free(j.line);
//         lkrm(&jobs,index);
//         (*index)--;
//     } else {
//         printf("[%d]:pid %d: %s\n",(*index)+1,j.pid,j.line);
//     }
// }
int cmd_jobs(int argcm, char**argv) {
    node *n=jobs.first;
    job *j;
    if (n!=NULL)
        j=(job*)n->data;
    size_t i=0;
    size_t o_i=1;
    while(n!=NULL) {
        pid_t ended=waitpid(j->pid,NULL,WNOHANG);
        n=n->next;
        if (ended) {
            printf("[%d]:pid %d: DONE! %s\n",o_i,ended,j->line);
            free(j->line);
            lkrm(&jobs,i);
            free(j);
        } else {
            printf("[%d]:pid %d: %s\n",o_i,j->pid,j->line);
            i++;
        }
        o_i++;
        if (n!=NULL)
            j=(job*)n->data;
    }
    return EXIT_SUCCESS;
}
int cmd_help(int argc, char**argv) {
    char *help_name="main";

    if (argc>1) {
        help_name=argv[1];
    }

    help_name=strpaste(help_name,".txt",NULL);

    char*help_dir;
    char*filename=NULL;

    char bin_path[512];
    for (size_t i=0; i<512 ;i++)
        bin_path[i]=0;
    struct stat st;

    readlink("/proc/self/exe",bin_path,512);
    stat("/proc/self/exe",&st);

    char *temp_dir=dirname(bin_path);
    char *dir=dirname(temp_dir);

    help_dir=strpaste(dir,"/help/",NULL);
    filename=strpaste(help_dir,help_name,NULL);

    free(help_dir);

    FILE *text=fopen(filename,"r");
    free(filename);
    free(help_name);
    if (text ==NULL) {
        printf("There is no help for \"%s\".\n",help_name);
        return EXIT_FAILURE;
    } else {
        char c=fgetc(text);
        while(c!=EOF) {
            printf("%c",c);
            c=fgetc(text);
        }
        fclose(text);
        printf("\n");
        return 0;
    }
}
char *builtin_name[BI_COUNT]={
    "cd",
    "exit",
    "true",
    "false",
    "history",
    "again",
    "fg",
    "jobs",
    "help"
};
bi_cmd builtin[BI_COUNT]={
    cd,
    cmd_exit,
    cmd_true,
    cmd_false,
    cmd_history,
    cmd_again,
    cmd_fg,
    cmd_jobs,
    cmd_help
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
