/*
Common use macros and types for projects. C language.
 */
#ifndef COMMONS_H
#define COMMONS_H

//Alocate memory
#define NEW(type,count) (type *)malloc(sizeof(type) * (count))
#define NEW_DRAMATIC(var,type,count) var= NEW(type,count) ; if(! var) { fprintf(stderr,"error: allocation error"); exit(1); }

//Default TRUE and FALSE values
#define TRUE 1
#define FALSE 0

//BOOL as a macro for conditional and variables
#define BOOL int

//Fast max and min
#define MAX(i, j) (((i) < (j)) ? (j) : (i) )
#define MIN(i, j) (((i) > (j)) ? (j) : (i) )

//Avoid NULL
#define DIFNULL(value,_default) ((value)!=NULL)? (value) : (_default)

#endif