//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef _SA
#define _SA

#include "parser.h"
#include "symbolTable.h"
#include "ast.h"
#include "semanticAnalyserDef.h"

extern void checkSemantics(astNode ast_root, int *sem_type_errors, p scope, NODETYPE scopetype);
extern IDNode findIdutil(char* lexeme, p node, NODETYPE nodeType, int row);
extern char* returnTypeName(int t);
extern int whichType(astNode node, p pnode, NODETYPE nodeType, int* sem_type_errors);
extern void checkArrayBounds(astNode root, p pnode, NODETYPE nodeType, int* sem_type_errors);
extern int calculateType(astNode rhsRoot, p pnode, NODETYPE nodeType, int* sem_type_errors);
extern void check_assignment_type(astNode rootnode, p pnode, NODETYPE nodeType, int *sem_type_errors);
extern void checkAllOuputParametersAssigned(LinkedList outputPList, int rown, int* sem_type_errors);
extern void check_for_semantics(astNode root, int *sem_type_errors, p forparent, NODETYPE fp, p forscope, NODETYPE fs);
extern void check_get_value_semantics(astNode node, int *sem_type_errors, p scope, NODETYPE scopetype);
extern void check_print_semantics(astNode ast_root, int *sem_type_errors, p scope, NODETYPE scopetype);
extern void check_module_reuse_semantics(astNode root, int *sem_type_errors, p scope, NODETYPE scopetype);


#endif
