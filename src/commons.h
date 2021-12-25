//Common use macros and types for projects. C language.
#ifndef COMMONS_H
#define COMMONS_H

#define NEW(type,count) (type *)malloc(sizeof(type) * count)
#define NEW_DRAMATIC(var,type,count) var= NEW(type,count) ; if(! var) { fprintf(stderr,"error: allocation error"); exit(1); }

#define TRUE 1
#define FALSE 0

#define BOOL int

#define MAX(i, j) (((i) < (j)) ? j : i)
#define MIN(i, j) (((i) > (j)) ? j : i)

#endif