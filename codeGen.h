//Group-15
//Dishita Malav 2017A7PS0164P
//Shaily Bhatt 2017A7PS0040P
//Shefali Tripathi 2017A7PS0139P

#ifndef CODEGEN
#define CODEGEN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbolTable.h"

void writeData(FILE* fp, HashEntry* symTable);
void writeArithmeticOrBoolean(FILE* fp, astNode op, int* labelNo, int offset, p parent, NODETYPE nodeType);
void writeStmts(FILE* fp, astNode root, NODETYPE nodeType, p parent);

void callPrintVal(FILE* fp);
void callPrintString(FILE* fp);
void callIsNonNeg(FILE* fp);
void callReadVal(FILE* fp);
void callReadString(FILE* fp);
void callAtoi(FILE* fp);
void callMulLoop(FILE* fp);
void callDoneCode(FILE* fp);
void callFin(FILE* fp);
void writeExit(FILE* fp);
void writeHelperFuncs(FILE* fp);
void writeMain(FILE* fp, astNode root);
void doCodeGen(FILE* fp, astNode root);


#endif