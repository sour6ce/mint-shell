#include "builtin.h"

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
