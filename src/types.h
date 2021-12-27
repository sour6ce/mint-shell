#ifndef _TYPES_H
#define _TYPES_H

#include <sys/types.h>

//Constant values of command line options
#define CMD_PIPEDRIGHT 16

#define CMD_EXTINPUT 1
#define CMD_EXTOUTPUT 2
#define CMD_HARDEXTOUTPUT 4
#define CMD_BACKGROUND 8

#define CMD_CHAINEDRIGHT 32

#define CMD_CONDITIONAL 64
#define CMD_ELSECOND 128

//Chain type
#define CMD_CHAIN_NORMAL 1
#define CMD_CHAIN_AND 2
#define CMD_CHAIN_OR 3

//Command info
typedef struct __cmd {
    unsigned char options;
    int argc;
    char **argv;

    //File path of output/input redirection. NULL if is not define
    char *extoutput;
    char *extinput;

    //Next in pipe. NULL if not piped or last
    struct __cmd *pipe;

    //Chain
    short chain_type;
    struct __cmd *chain;

    //Conditional
    struct __cmd *conditioncmd;
    struct __cmd *thencmd;
    struct __cmd *elsecmd;
} cmd;

//Macro for initialize cmd struct
#define CMD(name) cmd name = {0,0,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL}

//Macro for initialize cmd struct and get pointer
#define CMDP(name) cmd *name=NEW(cmd,1); CMD(__##name); (* name)=__##name


//Handler for a running process
typedef struct __cmd_handler
{
    //Complex handler are for multi-command shell lines
    int complex;

    //Process id of the command executing in case of simple handler, the first to execute in case of complex 
    pid_t pid;

    //Initial data of the shell line
    cmd *cmd_data;

} cmd_handler;

//Pointer to command function for built-ins
typedef int(*bi_cmd)(int,char**);

#endif