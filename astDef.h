//Group-15
//Dishita Malav 2017A7PS0164P
//Shaily Bhatt 2017A7PS0040P
//Shefali Tripathi 2017A7PS0139P

#ifndef _ASTDEF
#define _ASTDEF

#include <stdio.h>
#include "parser.h"
#include "lexer.h"

#define NO_TAGS 44

static char* tag_names[NO_TAGS] = {
	"TAG_PROGRAM",//0
	"TAG_MODULEDEC",//1
	"TAG_MODULE",//2
	"TAG_DRIVERMODULE",//3
	"TAG_OTHERMODULES",//4
	"TAG_INPUTPARAMETER",//5
	"TAG_OUTPUTPARAMETER",//6
	"TAG_DATATYPE",//7
	"TAG_ARRAY",//8
	"TAG_RANGEARRAY",//9
	"TAG_GETVALUESTMT",//10
	"TAG_PRINTSTMT",//11
	"TAG_ASSIGNMENTSTMT",//12
	"TAG_MODULEREUSESTMT",//13
	"TAG_DECLARESTMT",//14
	"TAG_CONDITIONALSTMT",//15
	"TAG_CASESTMT",//16
	"TAG_FORSTMT",//17
	"TAG_WHILESTMT",//18
	"TAG_BOOLEXPR",//19
	"TAG_ARITHMETICEXPR",//20
	"TAG_VAR",//21
	"TAG_IDNODE",//22
	"TAG_NUMNODE",//23
	"TAG_RNUMNODE",//24
	"TAG_TRUENODE",//25
	"TAG_FALSENODE",//26
	"TAG_PLUSNODE",//27
	"TAG_MINUSNODE",//28
	"TAG_MULNODE",//29
	"TAG_DIVNODE",//30
	"TAG_ANDNODE",//31
	"TAG_ORNODE",//32
	"TAG_LTNODE",//33
	"TAG_LENODE",//34
	"TAG_GTNODE",//35
	"TAG_GENODE",//36
	"TAG_EQNODE",//37
	"TAG_NENODE",//38
	"TAG_MODULEDEF",//39
	"TAG_OPTIONAL",//40
	"TAG_DEFAULTSTMT",//41
	"TAG_START",//42
	"TAG_END"//43
};

typedef enum{
	TAG_PROGRAM, //0
	TAG_MODULEDEC, //1
	TAG_MODULE, //2
	TAG_DRIVERMODULE, //3
	TAG_OTHERMODULES,//4
	TAG_INPUTPARAMETER,//5
	TAG_OUTPUTPARAMETER,//6
	TAG_DATATYPE,//7
	TAG_ARRAY,//8
	TAG_RANGEARRAY,//9
	TAG_GETVALUESTMT,//10
	TAG_PRINTSTMT,//11
	TAG_ASSIGNMENTSTMT,//12
	TAG_MODULEREUSESTMT,//13
	TAG_DECLARESTMT,//14
	TAG_CONDITIONALSTMT,//15
	TAG_CASESTMT,//16
	TAG_FORSTMT,//17
	TAG_WHILESTMT,//18
	TAG_BOOLEXPR,//19 //not used
	TAG_ARITHMETICEXPR,//20 // not used
	TAG_VAR,//21
	TAG_IDNODE,//22
	TAG_NUMNODE,//23
	TAG_RNUMNODE,//24
	TAG_TRUENODE,//25
	TAG_FALSENODE,//26
	TAG_PLUSNODE,//27
	TAG_MINUSNODE,//28
	TAG_MULNODE,//29
	TAG_DIVNODE,//30
	TAG_ANDNODE,//31
	TAG_ORNODE,//32
	TAG_LTNODE,//33
	TAG_LENODE,//34
	TAG_GTNODE,//35
	TAG_GENODE,//36
	TAG_EQNODE,//37
	TAG_NENODE,//38
	TAG_MODULEDEF,//39
	TAG_OPTIONAL,//40
	TAG_DEFAULTSTMT, //41
	TAG_START,//42
	TAG_END//43
}astLabel; //Please complete this as well

typedef enum{
    IS_ID,
    IS_FUNCTION,
    IS_STATEMENT
} NODETYPE;

typedef enum{
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL
}TYPE;

struct hashEntry{
    NODETYPE nodeType;
    union {
        struct iDNode* i;
        struct funcNode* f;
        struct stmtNode* s;
    }node;
    struct hashEntry* next;
};

typedef struct hashEntry* HashEntry;



struct astStruct{
	grammar_block g;
	//token tok; //change this to grammarblock or whatever else is appropriate //done
	astLabel label;
	struct astStruct* parent;
	struct astStruct* next_sibling;
	struct astStruct* first_child;
	HashEntry* table;

};

typedef struct astStruct* astNode;

typedef TYPE type_enum;

extern int AST_SIZE;

#endif