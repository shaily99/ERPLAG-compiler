//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#ifndef SYMBOLTABLEDEF
    #define SYMBOLTABLEDEF
    
    #include <stdlib.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <string.h>
    #include "ast.h"
    #include "parser.h" 
    
    
    #define TABLESIZE 211
    /*
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
    */
    
    // struct hashKey{
    //     int key;
    //     HashEntry head;
    // };
    
    // typedef struct hashKey* HashKey;
    
    // struct symTable{
    //     HashEntry head;
    //     int noOfElements;
    // };
    
    // typedef struct symTable* SymbolTable;

    typedef union{
        struct stmtNode* stmt;
        struct funcNode* func;
    }p;
    
    struct iDNode{
        grammar_block infoAboutNode;
        TYPE type;
        int width;
        int offset;
        bool isArray;
        bool isStatic;
        grammar_block startIndex;
        grammar_block endIndex;
        int upBound;
        int lowBound;
        bool isAssigned;
        bool isUsed; //Not sure if required, will see
        NODETYPE scopeType;
        p scope;
        char* moduleName;
        int nestingLevel;
    };
    
    typedef struct iDNode* IDNode;
    
    struct linkedList{
    
        IDNode parameter;
        struct linkedList* next;
    };
    
    typedef struct linkedList* LinkedList;
    
    struct funcNode{
        grammar_block infoAboutNode;
        bool isDriver;
        HashEntry localSymTable[TABLESIZE];
        //HashEntry inputPList[TABLESIZE];
        LinkedList inputPList;
        //HashEntry outputPList[TABLESIZE];
        LinkedList outputPList;
        int order;
        bool isDeclared;
        bool isDefined;
        bool isUsed;
        int width;
        int offset;
        int start;
        int end;
    };
    
    typedef struct funcNode* FuncNode;
    
    struct stmtNode{
        grammar_block infoAboutNode;
        HashEntry localSymTable[TABLESIZE];
        //HashEntry* parent;
        //HashEntry* parentSymTable;

        NODETYPE parentType;

        p parent;
        LinkedList iterators;
        int width;
        int offset;
        int start;
        int end;
        
    };
    
    typedef struct stmtNode* StmtNode;
    
    extern HashEntry globalSymTable[TABLESIZE];
    extern FuncNode driverFunc;

    extern int symbolTableErrors;
    
    #endif