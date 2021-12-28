#include "main.h"

void init() {
    
}
void loop() {
    while (TRUE)
    {
        char dir[512]; 
        getcwd(dir,512); //Get working directory

        printf ("%s $ ",dir); //Print wd at start of the line

        //Read the input
        char *line=readline(stdin);

        //TODO: Implement update_history(char*)
        //update_history(line)

        //Parse the line of the input and get information
        //of how to execute
        cmd *main_cmd=parse(line);

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
                    //TODO: Implement freecmd(cmd*)
                    //freecmd(main_cmd)
                    free(h);
                }
            } else {
                free(h);
            }
        }

        free(line);
    }
}


void __print_values(size_t i, node *n) {
    printf("[%d]=%d\n",i,*((int *)(n->data)));
}

void __dupby2(size_t i, node *n) {
    if (i%2) return;
    *(int*)(n->data)*=2;
}

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

    #pragma region Linked List Tests
    // LKLIST(list);

    // for (int i=1; i<=10 ;i++) {
    //     int *val=NEW(int,1);
    //     *val=i;
    //     lkins(&list,i,val);
    // }
    // printf("List created.\n");
    // lkfor(&list,print_values);
    // lkrm(&list,2);
    // printf("Third value removed.\n");
    // lkfor(&list,print_values);
    // int start=25,mid=81;
    // lkins(&list,0,&start);
    // printf("Added %d at start.\n",start);
    // lkfor(&list,print_values);
    // lkins(&list,9,&mid);
    // printf("Added %d at 9th position (should be the previous to the last one).\n",mid);
    // lkfor(&list,print_values);
    // lkrm(&list,25);
    // printf("The last value should be deleted.\n");
    // lkfor(&list,print_values);
    // lkrm(&list,-1);
    // printf("The first value should be deleted.\n");
    // lkfor(&list,print_values);
    // lkfor(&list,dupby2);
    // printf("Final changes.\n");
    // lkfor(&list,print_values);
    #pragma endregion

    #pragma region ParsingTest
    // LKLIST(prstr);

    // char test_rev[]="Mirella";
    // strrev(test_rev,strlen(test_rev));

    // printf("In reverse turns out to be: \"%s\".\n",test_rev);

    // lkappend(&prstr,"H\"elp Tutmosis");
    // int *bal=mapquote(prstr.first->data);

    // printf("%s\n",prstr.first->data);
    // for (size_t i = 0; i < strlen(prstr.first->data); i++)
    // {
    //     printf("%d ",bal[i]);
    // }
    // printf("\n");

    // free(bal);
    // lkappend(&prstr," if true then ls | grep \"if\" else ls | grep \"end\" end");

    // int3 *cond=mapcond(prstr.last->data);
    // printf("%s\n",prstr.last->data);
    // for (size_t i = 0; i < strlen(prstr.last->data)+1; i++)
    // {
    //     printf("%d ",cond[i][0]);
    // }
    // printf("\n");
    // for (size_t i = 0; i < strlen(prstr.last->data)+1; i++)
    // {
    //     printf("%d ",cond[i][1]);
    // }
    // printf("\n");
    // for (size_t i = 0; i < strlen(prstr.last->data)+1; i++)
    // {
    //     printf("%d ",cond[i][2]);
    // }
    // printf("\n");

    // printf("Next the ranges in \"%s\":\n",prstr.last->data);
    // range _if={0,0};
    // range _else={0,0};
    // range _then={0,0};
    // rangecond(cond,strlen(prstr.last->data)+1,_if,_then,_else);
    // printf("If range:%d,%d starting at: \"%s\".\n :\n",
    //     _if[0],_if[1],prstr.last->data+_if[0]);
    // printf("Then range:%d,%d starting at: \"%s\".\n :\n",
    //     _then[0],_then[1],prstr.last->data+_then[0]);
    // printf("Else range:%d,%d starting at: \"%s\".\n :\n",
    //     _else[0],_else[1],prstr.last->data+_else[0]);
    
    // printf("\n");
    
    // lkquit(&prstr); lkquit(&prstr);
    // char s1[]="This is \"unfair & unintended\" & I am going to make it.\n";

    // printf("catch the &: %s\n",tokfind(s1,strlen(s1),"&"));

    // char s2[]="Find my mom who is also your mom \"you bitch\".";

    // printf("Finded \"you\" in reverse in the sentence:\n\"%s\"\n->\"%s\"\n",s2,tokfindr(s2,strlen(s2),"you"));
    // printf("Finded \"mom\" in reverse in the sentence:\n\"%s\"\n->\"%s\"\n",s2,tokfindr(s2,strlen(s2),"mom"));

    #pragma endregion

    loop();
}