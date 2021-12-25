#include "utils.h"

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
