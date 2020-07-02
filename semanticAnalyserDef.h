//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef _SAD
#define _SAD

#include "symbolTable.h"
#include "ast.h"

extern int sym_type_errors;

struct whileVar{
	astNode ast_node;
	int hasChanged;
	struct whileVar *next_var;
};

typedef struct whileVar* whileList;

#endif