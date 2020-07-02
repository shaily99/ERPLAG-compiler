//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef _PARSERDEF
#define _PARSERDEF

#include "lexer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//defining constants to be used throughout the parser
#define TOTAL_TERMINALS 59
#define TOTAL_NON_TERMINALS 56
#define MAX_NTANDT 115
#define NO_OF_RULES 107
#define HASH_SIZE 211


//datastructure for parse table
typedef int** parse_table;
extern parse_table pt;


//enum datastructures for ease of access of terminals and non-terminals
static char* names_ntandt[MAX_NTANDT] = {
	"NT_program",
	"NT_moduleDeclarations",
	"NT_moduleDeclaration",
	"NT_otherModules",
	"NT_driverModule",
	"NT_module",
	"NT_ret",
	"NT_input_plist",
	"NT_input_plist_1",
	"NT_output_plist",
	"NT_output_plist_1",
	"NT_dataType",
	"NT_type",
	"NT_moduleDef",
	"NT_statements",
	"NT_statement",
	"NT_ioStmt",
	"NT_var",
	"NT_whichID",
	"NT_simpleStmt",
	"NT_assignmentStmt",
	"NT_whichStmt",
	"NT_lvalueIDStmt",
	"NT_lvalueARRStmt",
	"NT_index",
	"NT_moduleReuseStmt",
	"NT_optional",
	"NT_idList",
	"NT_idList_1",
	"NT_expression",
	"NT_U",
	"NT_sign",
	"NT_new_NT",
	"NT_var_id_num",
	"NT_arithmeticOrBooleanExpr",
	"NT_booleanExpr_1",
	"NT_booleanTerm",
	"NT_booleanTerm_1",
	"NT_arithmeticExpr",
	"NT_arithmeticExpr_1",
	"NT_term","NT_term_1",
	"NT_factor","NT_op_1",
	"NT_op_2",
	"NT_logicalOp",
	"NT_relationalOp",
	"NT_declareStmt",
	"NT_conditionalStmt",
	"NT_caseStmt",
	"NT_caseStmts",
	"NT_value",
	"NT_defaultStmt",
	"NT_iterativeStmt",
	"NT_range",
	"NT_range_arrays",
	"T_INTEGER",
	"T_REAL",
	"T_BOOLEAN",
	"T_OF",
	"T_ARRAY",
	"T_START",
	"T_END",
	"T_DECLARE",
	"T_MODULE",
	"T_DRIVER",
	"T_PROGRAM",
	"T_GET_VALUE",
	"T_PRINT",
	"T_USE",
	"T_WITH",
	"T_PARAMETERS",
	"T_TRUE",
	"T_FALSE",
	"T_TAKES",
	"T_INPUT",
	"T_RETURNS",
	"T_AND",
	"T_OR",
	"T_FOR",
	"T_IN",
	"T_SWITCH",
	"T_CASE",
	"T_BREAK",
	"T_DEFAULT",
	"T_WHILE",
	"T_PLUS",
	"T_MINUS",
	"T_MUL",
	"T_DIV",
	"T_LT",
	"T_LE",
	"T_GE",
	"T_GT",
	"T_EQ",
	"T_NE",
	"T_DEF",
	"T_ENDDEF",
	"T_DRIVERDEF",
	"T_DRIVERENDDEF",
	"T_COLON",
	"T_RANGEOP",
	"T_SEMICOL",
	"T_COMMA",
	"T_ASSIGNOP",
	"T_SQBO",
	"T_SQBC",
	"T_BO",
	"T_BC",
	"T_COMMENTMARK",
	"T_ID",
	"T_NUM",
	"T_RNUM",
	"epsilon",
	"T_DOLLAR"
};

typedef enum {
	NT_program,
	NT_moduleDeclarations,
	NT_moduleDeclaration,
	NT_otherModules,
	NT_driverModule,
	NT_module,
	NT_ret,
	NT_input_plist,
	NT_input_plist_1,
	NT_output_plist,
	NT_output_plist_1,
	NT_dataType,
	NT_type,
	NT_moduleDef,
	NT_statements,
	NT_statement,
	NT_ioStmt,
	NT_var,
	NT_whichID,
	NT_simpleStmt,
	NT_assignmentStmt,
	NT_whichStmt,
	NT_lvalueIDStmt,
	NT_lvalueARRStmt,
	NT_index,
	NT_moduleReuseStmt,
	NT_optional,
	NT_idList,
	NT_idList_1,
	NT_expression,
	NT_U,
	NT_sign,
	NT_new_NT,
	NT_var_id_num,
	NT_arithmeticOrBooleanExpression,
	NT_booleanExpr_1,
	NT_booleanTerm,
	NT_booleanTerm_1,
	NT_arithmeticExpr,
	NT_arithmeticExpr_1,
	NT_term,
	NT_term_1,
	NT_factor,
	NT_op_1,
	NT_op_2,
	NT_logicalOp,
	NT_relationalOp,
	NT_declareStmt,
	NT_conditionalStmt,
	NT_caseStmt,
	NT_caseStmts,
	NT_value,
	NT_defaultStmt,
	NT_iterativeStmt,
	NT_range,
	NT_range_arrays,
	T_INTEGER, 
	T_REAL, 
	T_BOOLEAN,
	T_OF, 
	T_ARRAY, 
	T_START, 
	T_END, 
	T_DECLARE, 
	T_MODULE, 
	T_DRIVER, 
	T_PROGRAM,
	T_GET_VALUE, 
	T_PRINT, 
	T_USE, 
	T_WITH, 
	T_PARAMETERS, 
	T_TRUE, 
	T_FALSE, 
	T_TAKES, 
	T_INPUT, 
	T_RETURNS, 
	T_AND, 
	T_OR, 
	T_FOR, 
	T_IN, 
	T_SWITCH,
	T_CASE, 
	T_BREAK, 
	T_DEFAULT, 
	T_WHILE, 
	T_PLUS, 
	T_MINUS, 
	T_MUL, 
	T_DIV,
	T_LT, 
	T_LE, 
	T_GE, 
	T_GT, 
	T_EQ, 
	T_NE, 
	T_DEF,
	T_ENDDEF, 
	T_DRIVERDEF,
	T_DRIVERENDDEF, 
	T_COLON, 
	T_RANGEOP, 
	T_SEMICOL, 
	T_COMMA, 
	T_ASSIGNOP, 
	T_SQBO, 
	T_SQBC, 
	T_BO, 
	T_BC, 
	T_COMMENTMARK,
	T_ID, 
	T_NUM,
	T_RNUM,
	epsilon,
	T_DOLLAR 
} NTANDT;


//data structures for hash table
struct HASH_NODE{
	int enum_val;
	char name[50];
	struct HASH_NODE* next;
};
typedef struct HASH_NODE* hash_node;
extern hash_node hash_table[HASH_SIZE];


//data structures for populating grammar table
struct g{
	NTANDT name;
	char str_name[50];
	int flag_ntandt; //flag_ntandt = 0 for terminals and 1 for non terminals
	struct g* next;
	token tok;
};
typedef struct g* grammar_block;
extern grammar_block grammar_table[NO_OF_RULES];

//data structures for first and follow sets
typedef struct mtfirst{
	int start;
	int end;
} mapTableFirstBlock;
extern mapTableFirstBlock firstSetMT[TOTAL_NON_TERMINALS];

typedef struct f{
	NTANDT name;
	bool flag_first;
	bool flag_follow;
	bool* first;
	bool* follow;
} FirstAndFollow;
extern FirstAndFollow F[TOTAL_NON_TERMINALS];


//data structures for parse tree 
struct parseTreeNode{
	grammar_block g;
	struct parseTreeNode* first_child;
	struct parseTreeNode* next_sibling;
	struct parseTreeNode* parent;
	int no_of_children;
	int rule_no;
};
typedef struct parseTreeNode* ptnode;


//data structure for stack
struct stackNode{
	ptnode p;
	struct stackNode* next;
};
typedef struct stackNode* stack_node;


struct s{
	stack_node head;
	stack_node tail;
	int size;
};
typedef struct s* Stack;

extern int syntaxErr;
extern int PARSETREE_SIZE
#endif