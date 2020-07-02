//Group-15
//Dishita Malav 2017A7PS0164P
//Shaily Bhatt 2017A7PS0040P
//Shefali Tripathi 2017A7PS0139P

#ifndef _ASTFN
#define _ASTFN

#include "astDef.h"
#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern ptnode find_child(ptnode proot, int key, int index);
extern astNode last_child(astNode parent);
extern astNode createNewASTnode(astNode temp, astLabel l, astNode p, astNode fc, astNode ns, grammar_block gb);
//extern void handle_inh(astNode* node, astNode* ptr1, astNode* ptr2);
//extern void handle_inh_2(astNode* node, astNode* ptr1, astNode* ptr2);
extern astNode make_ast(ptnode parseTreeRoot, astNode parent);
extern void print_ast(astNode root);
extern astNode ast_main(ptnode parseTreeRoot);

#endif