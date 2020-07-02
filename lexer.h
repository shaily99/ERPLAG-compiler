//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef _LEXERFUNCTIONS
#define _LEXERFUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"

extern void clearString(char *str, int size);
extern int isKeyword(char *tokstr);
extern void insertToken(char *val, char *type);
extern token acceptState(char *str, int s);
extern void printTokenList();
extern token getNextToken();
extern void getStream(FILE *fp);
//extern void removeComments(char* inputFile, char* outputFile);
extern void removeComments(char* inputFile);

#endif