#include "cmdreadline.h"

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
