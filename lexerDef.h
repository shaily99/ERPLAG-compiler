//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef _LEXER
#define _LEXER

#include <stdio.h>
#include <stdlib.h>

struct token{
	char tokenType[100];
	char tokenVal[200]; 
	int rownum;
};

typedef struct token* token;


extern char ipStr[50000]; // buffer for whatever is read from file

extern char currStr[1000]; //buffer for whatever I have read so far


extern int isError; // whether I detected an error
extern int state; //current dfa state
extern int currStrStart; //pointer in current string buffer
extern int row; // line number 
extern int lookahead; //pointer in input string

#endif