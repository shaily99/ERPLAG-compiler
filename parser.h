//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef _PARSER
#define _PARSER

#include "lexer.h"
#include "parserDef.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

;
extern ptnode parser_main(char *filename);
extern void print_parseTree (ptnode parse_root);

extern hash_node createHashNode(hash_node h);
extern int hashFunc(char* pattern);
extern void insertHashKey ( char* pattern, int enum_value);
extern void hash_ntandt(char** to_hash);
extern int find_in_hash(char* word);
extern int find_enum_val(char* word);
extern void display_hash_table(hash_node* hash_table);

extern grammar_block createGrammarNode(grammar_block gb);
extern void displayGrammarTable();
extern void populateGrammarTable(char* filename);

extern void computeFirstAndFollow ();
extern void computeFirst( int x );
extern bool* computeFirstUtil( int i );
extern void computeFollow( int x);
extern void printFirst();
extern void printFollow();
extern void populateMapTableFirst();
extern bool* createUnion(bool* A, bool* B);

extern parse_table initialiseParseTable();
extern parse_table createParseTable(FirstAndFollow *ff);
extern void printParseTable(parse_table pt);

extern Stack createStack();
extern stack_node top(Stack s);
extern stack_node createStackNode(	ptnode pnode);
extern stack_node pop(Stack stack);
extern void push(Stack stack, ptnode stack_data);
extern void pushAllChildren(Stack stack, ptnode stack_data);
extern void printStack(Stack stack);

extern ptnode createParseTreeNode(ptnode parent, NTANDT t, char *str, int flag, int rule_no);
extern ptnode createlistofchildren(grammar_block ruleRHShead, ptnode parent, int rule);
extern ptnode addChild(int rule, ptnode parent);
extern ptnode initializeParseTree();

extern void printParseTreeNode(ptnode node);
extern void printParseTree(ptnode r);

extern void isparsingover(Stack stack, int* errors);
extern void usetokenforparsing(token ptr, Stack stack, int *errors, bool* reUseToken);
extern ptnode parseInputSourceCode(char *filename, int** ptable, int *synataxerrors);


#endif