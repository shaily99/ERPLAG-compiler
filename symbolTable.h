//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef SYMBOLTABLE
#define SYMBOLTABLE

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "symbolTableDef.h"
#include "ast.h"
#include "parser.h" 

int hash(char* lexeme); // lexeme = g->tok->tokenVal

//SymbolTable initialiseSymbolTable(SymbolTable s); //does malloc wagera
void* retrieve(char* lexeme, int hashVal, HashEntry symTable[TABLESIZE], NODETYPE inputNodeType);

void insertID(IDNode root, HashEntry symTable[TABLESIZE]);
void insertDeclarationFunction(astNode root, HashEntry symTable[TABLESIZE], bool displayErrors);
void insertStmtNode(StmtNode root, HashEntry symTable[TABLESIZE], int orderOfStmt);

IDNode findID(char* lexeme, int hashVal, p node, NODETYPE nodeType);

int insertStmtAdv(astNode child, StmtNode sNode, char* lexeme, int localOffset, bool isDriver, int nest, bool displayErrors);
int insertStmt(astNode child, FuncNode fNode , char* lexeme, int localOffSet, bool isDriver, int nest, bool displayErrors);

void insertLL(IDNode id, LinkedList list);
IDNode retrieveFromLL(char* lexeme , LinkedList list);

void insertDefinitionFunction(astNode root, HashEntry symTable[TABLESIZE], bool displayErrors);
void insertDriver(astNode root, HashEntry symTable[TABLESIZE], bool displayErrors);

void printID(IDNode id);
void printInputPList(LinkedList list);
void printSymbolTable(HashEntry symTable[TABLESIZE]);
void printDriver();

void printArr(IDNode id);
void printInputPListArr(LinkedList list);
void printArrays(HashEntry symTable[TABLESIZE]);
void printFunctions();

void createSymbolTable(astNode root, bool displayErrors);

#endif
