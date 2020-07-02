//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "symbolTableDef.h"
#include "symbolTable.h"
#include "ast.h"
#include "parser.h" 
	
	//Symtable ke saare elements ko NULL???
	
#define SIZEOFINT 2
#define SIZEOFREAL 4
#define SIZEOFBOOL 1
#define DEFAULT_DYNAMIC_SIZE 8

int FUNCTIONORDER = 1;
int GLOBALOFFSET = 0;
	
HashEntry globalSymTable[TABLESIZE];

FuncNode driverFunc;

int symbolTableErrors = 0;
	
	int hash(char* lexeme){
	    int p = 29;
		int m =  TABLESIZE;
		int hashVal = 0;
	
		int mult = 1;
	
		for(int i=0; i<strlen(lexeme); i++){
		    hashVal = (hashVal + lexeme[i]*mult)%m;
		    mult = (mult*p)%m;
		}
		return hashVal;
	}
	
	void* retrieve(char* lexeme, int hashVal, HashEntry symTable[TABLESIZE], NODETYPE inputNodeType){
	
	    if(symTable[hashVal] == NULL){
	        //printf("No entry found\n");
	        return NULL;
	    }
	
	    HashEntry head = symTable[hashVal];
	
	    while(head != NULL){
	        if(head -> nodeType != inputNodeType)
	            head = head->next;
	        else{
	            if(inputNodeType == IS_ID){
	                if(strcmp(head -> node.i -> infoAboutNode -> tok -> tokenVal,lexeme) == 0)
	                    return head -> node.i;
	                else
	                    head = head ->next;
	            }
	            else if(inputNodeType == IS_FUNCTION){
	                if(strcmp(head -> node.f -> infoAboutNode -> tok -> tokenVal,lexeme) == 0)
	                    return head -> node.f;
	                else
	                    head = head ->next;
	            }
	            else if(inputNodeType == IS_STATEMENT){ // this won't occur
	                if(strcmp(head -> node.s -> infoAboutNode -> str_name,lexeme) == 0)
	                    return head -> node.s;
	                else
	                    head = head ->next;
	            }
	        }
	    }
	
	    //printf("No entry found\n");
	    return NULL;
	
	}
	
	void insertID(IDNode root, HashEntry symTable[TABLESIZE]){
	
	    //printf("%s      %d\n", root->infoAboutNode->tok->tokenVal, root->type);
	
	    int hashVal = hash(root->infoAboutNode->tok->tokenVal);
	
	    HashEntry entry = (HashEntry)malloc(sizeof(struct hashEntry));
	    entry -> nodeType = IS_ID;
	    entry -> node.i = root;
	    entry -> next = NULL;
	    
	    if(symTable[hashVal] == NULL){
	        symTable[hashVal] = entry;
	    }
	    else{
	        HashEntry it = symTable[hashVal];
	        while(it->next){
				it = it->next;
		    }
	        it->next = entry;
	    }
	
	    return;
	
	}
	
	void insertDeclarationFunction(astNode root, HashEntry symTable[TABLESIZE], bool displayErrors){
	
	    int hashVal = hash(root->g->tok->tokenVal);
	    //printf("entered %s\n",root->g->tok->tokenVal);
	
	    FuncNode temp = (FuncNode)retrieve(root->g->tok->tokenVal, hashVal, symTable, IS_FUNCTION);
		

	    if(temp != NULL && temp->isDeclared == true){
	    	if(displayErrors)
	        	printf("ERROR: LINE NO: %d: The Function %s has already been declared\n", root->g->tok->rownum, root->g->tok->tokenVal);
	        symbolTableErrors ++;
	        return;
	    }



        //new
	    if(temp!=NULL && temp->isDefined == true)
	    {
	    	if(displayErrors)
	    		printf("ERROR: LINE NO: %d: The function %s has already been defined\n", root->g->tok->rownum, root->g->tok->tokenVal);
	    	symbolTableErrors ++;
	    	return;
	    }
	    //new

	    //printf("not retrieved %s\n",root->g->tok->tokenVal);
	
	    temp = (FuncNode)malloc(sizeof(struct funcNode));
	    temp -> infoAboutNode = root -> g;
	    //temp -> localSymTable = NULL;
	    //temp -> inputPList = NULL;
	    //temp -> outputPList = NULL;
	    temp -> isDriver = false;
	    temp -> order = FUNCTIONORDER++;
	    temp -> isDeclared = true;
	    temp -> isDefined = false;
	    temp->isUsed = false;
	    temp -> width = 0;
	    temp -> offset = -1;
	
	    HashEntry entry = (HashEntry)malloc(sizeof(struct hashEntry));
	    entry -> nodeType = IS_FUNCTION;
	    entry -> node.f = temp;
	    entry -> next = NULL;
	
	    if(symTable[hashVal] == NULL){
	        symTable[hashVal] = entry;
	    }
	    else{
	        HashEntry it = symTable[hashVal];
	        while(it->next){
				it = it->next;
		    }
	        it->next = entry;
	    }
	    //adding
	    //root->table = symTable;
	    //done
	
	    //printf("%s\n", symTable[hashVal]->node.f->infoAboutNode -> tok -> tokenVal);
	
	    return;
	}
	
	void insertStmtNode(StmtNode root, HashEntry symTable[TABLESIZE], int orderOfStmt){
	
	    char str[100];
	
	    //printf("insertStmtNode\n");
	
	    snprintf(str, 100, "%d", orderOfStmt); //converting line no of the statement to a string
	
	    //printf("%s\n", str );
	    strcat(root -> infoAboutNode -> str_name, str); // concatenating it onto the it's token val
	
	    //printf("%s\n", root -> infoAboutNode -> str_name);
	
	    int hashVal = hash(root->infoAboutNode->str_name);
	
	    HashEntry entry = (HashEntry)malloc(sizeof(struct hashEntry));
	    entry -> nodeType = IS_STATEMENT;
	    entry -> node.s = root;
	    entry -> next = NULL;
	    
	    if(symTable[hashVal] == NULL){
	        symTable[hashVal] = entry;
	    }
	    else{
	        HashEntry it = symTable[hashVal];
	        while(it->next){
				it = it->next;
		    }
	        it->next = entry;
	    }
	
	    return;
	
	}

	IDNode findID(char* lexeme, int hashVal, p node, NODETYPE nodeType){

	//printf("INSIDE FINDID ");
	if(nodeType == IS_STATEMENT){
		 //printf("%s\n", node.stmt -> infoAboutNode -> str_name);
		IDNode id = retrieve(lexeme, hashVal, node.stmt -> localSymTable, IS_ID);
		if(id == NULL){
			return findID(lexeme, hashVal, node.stmt -> parent, node.stmt -> parentType);
		}
		else 
			return id;
	}
	else if(nodeType == IS_FUNCTION){

		//printf("%s\n", node.func-> infoAboutNode -> tok -> tokenVal);

		IDNode id = retrieve(lexeme, hashVal, node.func -> localSymTable, IS_ID);
		if(id == NULL){
			id = retrieveFromLL(lexeme, node.func -> inputPList);
			if(id == NULL)
				return NULL;
			else 
				return id;
		}
		else return id;

	}
	else 
		return NULL;
	
}

	int insertStmtAdv(astNode child, StmtNode sNode, char* lexeme, int localOffSet, bool isDriver, int nest, bool displayErrors){
		int orderOfStmt = 1;

		HashEntry* symTable = sNode -> localSymTable;
	    
	    int width = 0;
	
	    while(child != NULL){
	        //printf("%s hey\n", child -> g -> str_name);
	        if(child -> label == TAG_END){
	        	sNode -> end = child -> g -> tok -> rownum;
	        }
	        if(child -> label == TAG_PRINTSTMT){
	            //child = child -> next_sibling;
	            //continue;
	            //DO NOTHING FOR NOW
	            //printf("%s\n", child -> g -> str_name);
	        }
	        else if(child -> label == TAG_GETVALUESTMT){

	        	//printf("sheffy\n");
	        	//printf("%s\n",child -> first_child -> g -> tok -> tokenVal);
	        	if(retrieveFromLL(child -> first_child -> g -> tok -> tokenVal, sNode -> iterators) != NULL){
	        		//printf("sheffy\n");
	                if(displayErrors)
	                	printf("ERROR: LINE NO: %d: Cannot redefine Iterator Variable %s \n", child -> first_child  -> g -> tok -> rownum, child -> first_child  -> g -> tok -> tokenVal);
	            
	                symbolTableErrors ++;
	            }
				//printf("sheffy\n");

	        }
	        else if(child -> label == TAG_ASSIGNMENTSTMT){

	        	if(retrieveFromLL(child -> first_child -> g -> tok -> tokenVal, sNode -> iterators) != NULL){
	                if(displayErrors)
	                	printf("ERROR: LINE NO: %d: Cannot redefine Iterator Variable %s \n", child -> first_child  -> g -> tok -> rownum, child -> first_child  -> g -> tok -> tokenVal);
	            	symbolTableErrors ++;

	            }


	        }
	        else if(child -> label == TAG_MODULEREUSESTMT){
	            //printf("%s\n", child -> g -> str_name);

	        	astNode opId = child -> first_child -> first_child;
	        	int error=0;
	        	while(opId){

	        		if(retrieveFromLL(opId-> g -> tok -> tokenVal, sNode -> iterators) != NULL){
	        			if(displayErrors)
	                		printf("ERROR: LINE NO: %d: Cannot redefine Iterator Variable %s \n", opId -> g -> tok -> rownum, opId -> g -> tok -> tokenVal);
	                	symbolTableErrors ++;
	            	}

	        		opId = opId -> next_sibling;
	        	}

	        	
	            if(!isDriver){
	                astNode calledFunc = child -> first_child -> next_sibling;
	                //printf("%s\n", calledFunc -> g -> tok ->tokenVal);
	                if(strcmp(calledFunc -> g -> tok ->tokenVal,lexeme) == 0){
	                	if(displayErrors)
	                    	printf("ERROR: LINE NO: %d: Recursive Call of %s is not Allowed\n", calledFunc -> g -> tok -> rownum, calledFunc -> g -> tok -> tokenVal);
	                    error = 1;
	                    symbolTableErrors ++;
	                }
	            }


	            //new
	            astNode c = child->first_child->next_sibling;
	            
	            int h = hash(c->g->tok->tokenVal);
	            FuncNode f = (FuncNode)retrieve(c->g->tok->tokenVal,h,globalSymTable,IS_FUNCTION);
	            if(f==NULL)
	            {
	            	if(displayErrors)
	            		printf("ERROR: LINE NO: %d: Function %s not declared or defined before use\n",c->g->tok->rownum,c->g->tok->tokenVal);
	            	error=1;
	            	symbolTableErrors ++;
	            	//printf("error is 1\n");
	            }


	            else if(f->isDeclared == true && f->isDefined==true)
	            {
	            	if(f->isUsed==false){
	            		if(displayErrors)
	            			printf("ERROR: LINE NO: %d: Function %s has been both declared and defined before use\n",f->infoAboutNode->tok->rownum,f->infoAboutNode->tok->tokenVal);
	            			//printf("Line no. %d: Function %s has been both declared and defined before use\n",c->g->tok->rownum,f->infoAboutNode->tok->tokenVal);
	            		error =1;
	            		symbolTableErrors ++;
	            	}
	            }
	            //printf("done\n");
				
				if(error==0)
				{
					f->isUsed = true;
				}
				//new


	        }
	        else if(child -> label == TAG_DECLARESTMT){
	            //printf("%s\n", child -> g -> str_name);
	            astNode decID = child -> first_child;
	            astNode dataType = decID;
	
	            while(dataType -> label != TAG_DATATYPE){
	                dataType = dataType -> next_sibling;
	            }
	
	            //finding datatype of all ids in idlist
	            TYPE temp_type;
	            int temp_width, temp_offset;
	            astNode temp_startIndex;
	            astNode temp_endIndex;
	            bool temp_isArray, temp_isAssigned;
	            bool temp_isStatic = true;
	            int lowerBound = 0, upperBound = 0;
	
	            if(dataType->first_child == NULL){ // not an array
	                temp_isArray = false;
	                if(strcmp(dataType -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    temp_type = TYPE_INT;
	                    temp_width =SIZEOFINT;
	                    temp_offset = localOffSet; 
	                }
	                else if(strcmp(dataType -> g -> tok -> tokenType, "T_REAL") == 0){
	                    temp_type = TYPE_REAL;
	                    temp_width = SIZEOFREAL;
	                    temp_offset = localOffSet;  
	                }
	                else if(strcmp(dataType -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    temp_type = TYPE_BOOL;
	                    temp_width = SIZEOFBOOL;
	                    temp_offset = localOffSet;
	                }   
	                
	                temp_isAssigned = false;
	                temp_startIndex = NULL;
	                temp_endIndex = NULL;
	            }
	            
	            else{ //is an array
	
	                //sheffy
	                temp_isArray = true;
	                astNode childOfArr = dataType->first_child;
	                childOfArr = childOfArr -> next_sibling;
	                temp_startIndex = childOfArr -> first_child ;
	                temp_endIndex = childOfArr -> first_child -> next_sibling;
	
	                if(temp_startIndex -> label == TAG_IDNODE || temp_endIndex -> label == TAG_IDNODE){
	                    temp_isStatic = false;
	                    temp_width = DEFAULT_DYNAMIC_SIZE;
	                }
	
	                else{
	                    lowerBound = atoi(temp_startIndex -> g -> tok -> tokenVal);
	                    upperBound = atoi(temp_endIndex -> g -> tok -> tokenVal);
	                    temp_width = (upperBound - lowerBound +1);
	                }
	
	                childOfArr = childOfArr -> next_sibling;
	                if(strcmp(childOfArr -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    temp_type = TYPE_INT;
	                    if(temp_isStatic){
	                        temp_width = SIZEOFINT*temp_width + 1; //*(temp_endIndex - temp_startIndex +1);
	
	
	                    }
	                    temp_offset = localOffSet; 
	                }
	                else if(strcmp(childOfArr -> g -> tok -> tokenType, "T_REAL") == 0){
	                    temp_type = TYPE_REAL;
	                    if(temp_isStatic){
	                        temp_width = SIZEOFREAL*temp_width + 1;//*(temp_endIndex - temp_startIndex +1);
	
	
	                    }
	                    temp_offset = localOffSet;  
	                }
	                else if(strcmp(childOfArr -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    temp_type = TYPE_BOOL;
	                    if(temp_isStatic){
	                        temp_width = SIZEOFBOOL*temp_width + 1;//*(temp_endIndex - temp_startIndex +1);
	
	
	                    }
	                    temp_offset = localOffSet;
	                }   
	                
	                temp_isAssigned = false;
	            }
	            
	            while(decID -> label == TAG_IDNODE){
	
	                int hashVal = hash(decID -> g -> tok -> tokenVal);
			    
			        if(retrieve (decID -> g -> tok -> tokenVal, hashVal, symTable, IS_ID) != NULL){
			        	if(displayErrors)
	                    	printf("ERROR: LINE NO: %d: Variable %s already declared within scope\n", decID -> g -> tok -> rownum, decID -> g -> tok -> tokenVal);
	                    symbolTableErrors ++;
	                }


	                if(retrieveFromLL(decID -> g -> tok -> tokenVal, sNode -> iterators) != NULL){
	                	if(displayErrors)
	                		printf("ERROR: LINE NO: %d: Cannot redeclare Iterator Variable %s\n", decID -> g -> tok -> rownum, decID -> g -> tok -> tokenVal);
	                	symbolTableErrors ++;
	                }

	
	                else{
	                    IDNode newID = (IDNode)malloc(sizeof(struct iDNode));
	                    newID -> infoAboutNode = decID -> g;
	                    newID -> type = temp_type;
	                    newID -> width = temp_width;
	                    newID -> offset = temp_offset;
	                    newID -> isArray = temp_isArray; 
	                    newID -> isAssigned = temp_isAssigned;
	                    newID -> isUsed = false;
	                    newID -> scope.stmt = sNode;
	                    newID -> scopeType = IS_STATEMENT;
	                    newID -> moduleName = lexeme;
	                    newID -> nestingLevel = nest;

	                    if(temp_startIndex)
	                        newID -> startIndex = temp_startIndex -> g;
	                    else
	                        newID -> startIndex = NULL;
	                    if(temp_endIndex)
	                        newID -> endIndex = temp_endIndex -> g;
	                    else
	                        newID -> endIndex = NULL;
	                    newID -> upBound = upperBound;
	                    newID -> lowBound = lowerBound;
	                    newID -> isStatic = temp_isStatic;
	
	                    localOffSet += newID -> width;
	                    width += newID -> width;
	                    insertID(newID,symTable);
	                    //adding
	                    //decID->table = symTable;
	                }
	                //done
	                decID = decID -> next_sibling;
	            }
	        }
	
	        else if(child -> label == TAG_CONDITIONALSTMT){
	
	            //printf("%s\n", child -> g -> str_name);
	
	            int condLocalOffset = 0;
	
	            astNode condChild = child -> first_child -> next_sibling -> next_sibling;
	
	            StmtNode newStmt = (StmtNode)malloc(sizeof(struct stmtNode));
	            newStmt -> infoAboutNode = child -> g;

	            newStmt -> start = child -> first_child -> next_sibling -> g -> tok -> rownum;
	
	
	
	
	            //printf("%d ciao\n", child ->first_child->g -> tok -> rownum);
	            newStmt -> offset = localOffSet;
	            newStmt -> iterators = (LinkedList)malloc(sizeof(struct linkedList));
	            LinkedList temp = sNode -> iterators;
	            while(temp!= NULL && temp -> parameter != NULL){
	            	insertLL(temp -> parameter, newStmt -> iterators);
	            	temp = temp -> next;
	            }
	            //newStmt -> parentSymTable = symTable;
	
	            int w = 0;

	            insertStmtNode(newStmt, symTable, orderOfStmt++);
	            //adding
	            newStmt -> parentType = IS_STATEMENT;
	            newStmt->parent.stmt = sNode;
	
	            while(condChild != NULL && condChild -> label == TAG_CASESTMT){
	
	                //printf("Case\n");
	
	                astNode caseChild = condChild -> first_child -> next_sibling;
	
	                if(caseChild != NULL)
	                    w = insertStmtAdv(caseChild, newStmt, lexeme,localOffSet, isDriver, nest+1, displayErrors);
	
	                newStmt -> width += w;
	                condLocalOffset += w;
	
	                condChild = condChild -> next_sibling;
	
	            }
	
	            
	            if(condChild != NULL && condChild -> label == TAG_DEFAULTSTMT){
	                //printf("Default\n");
	                newStmt -> width += insertStmtAdv(condChild -> first_child, newStmt, lexeme,localOffSet,isDriver,nest+1, displayErrors);
	                condChild = condChild -> next_sibling;
	            }

	            if(condChild != NULL && condChild -> label == TAG_END){
	            	//printf("end cond\n");
	            	newStmt -> start = condChild -> g -> tok -> rownum;
	            }
	
	
	            width += newStmt -> width;
	            localOffSet += newStmt -> width;
	        
	            //printf("%s mewo\n", newStmt -> parentStmt -> infoAboutNode -> str_name);
	            //done
	            //printf("%s Meow\n", newStmt -> infoAboutNode -> str_name);
	        }
	        else if(child -> label == TAG_FORSTMT){
	
	            //printf("%s\n", child -> g -> str_name);
	            //char* forIt = child -> first_child -> g -> tok -> tokenVal;
	            astNode forChild = child -> first_child;
	            StmtNode newStmt = (StmtNode)malloc(sizeof(struct stmtNode));
	            newStmt -> infoAboutNode = child -> g;

	            //newStmt -> iterators = sNode -> iterators;

	            newStmt -> iterators = (LinkedList)malloc(sizeof(struct linkedList));
	            LinkedList temp = sNode -> iterators;
	            while(temp!= NULL && temp -> parameter != NULL){
	            	insertLL(temp -> parameter, newStmt -> iterators);
	            	temp = temp -> next;
	            }


	            newStmt -> start = child -> first_child -> next_sibling -> next_sibling -> g -> tok -> rownum;

	            int hashVal = hash(child -> first_child -> g -> tok -> tokenVal);
	            p node;
	            node.stmt = sNode;
	            IDNode forIt = findID(child -> first_child -> g -> tok -> tokenVal, hashVal, node, IS_STATEMENT); //write find

	            if(forIt == NULL){
	            	//printf("Line no. %d: Iterator %s not declared\n", child -> first_child -> g -> tok -> rownum, child -> first_child -> g -> tok -> tokenVal);
	            	forIt = (IDNode)malloc(sizeof(struct iDNode));
	            	forIt -> infoAboutNode = child -> first_child -> g;
	            	forIt -> width = 0;
					forIt -> offset = 0;
					forIt -> isArray = false;
					forIt -> isStatic = false;
					forIt -> startIndex = 0;
					forIt -> endIndex = 0;
					forIt -> upBound = 0;
					forIt -> lowBound = 0;
					forIt -> isAssigned = true;
					forIt -> isUsed = false;
	            }

	            insertLL(forIt, newStmt -> iterators);
	
	            forChild = forChild -> next_sibling -> next_sibling -> next_sibling; 
	             //printf("%s in for\n", forChild -> g -> str_name);

	            insertStmtNode(newStmt, symTable, orderOfStmt++);
	            //adding
	            newStmt -> parentType = IS_STATEMENT;
	            newStmt->parent.stmt = sNode;
	            // going directly to node corresponding to stmnts
	            if(forChild != NULL)
	                newStmt -> width += insertStmtAdv(forChild, newStmt, lexeme,localOffSet,isDriver,nest+1, displayErrors); 
				
	            newStmt -> offset = localOffSet;
	            //newStmt -> parentSymTable = symTable;
	            width += newStmt -> width;
	
	            localOffSet += newStmt -> width;
	


	            //done
	
	        }
	        else if(child -> label == TAG_WHILESTMT){
	            //printf("%s\n", child -> g -> str_name);

	            astNode whileChild = child -> first_child;
	            StmtNode newStmt = (StmtNode)malloc(sizeof(struct stmtNode));
	            newStmt -> infoAboutNode = child -> g;

	            newStmt -> iterators = (LinkedList)malloc(sizeof(struct linkedList));
	            LinkedList temp = sNode -> iterators;
	            while(temp!=NULL && temp -> parameter != NULL){
	            	insertLL(temp -> parameter, newStmt -> iterators);
	            	temp = temp -> next;
	            }


	            newStmt -> start = child -> first_child -> next_sibling -> g -> tok -> rownum;
	
	            whileChild = whileChild -> next_sibling -> next_sibling;


	            insertStmtNode(newStmt, symTable, orderOfStmt++);
	            //adding
	            newStmt -> parentType = IS_STATEMENT;
	            newStmt->parent.stmt = sNode;
	
	            if(whileChild!= NULL)
	                newStmt -> width += insertStmtAdv(whileChild, newStmt, lexeme,localOffSet,isDriver, nest+1, displayErrors); 
	            newStmt -> offset = localOffSet;
	            //newStmt -> parentSymTable = symTable;
	            width += newStmt -> width;
	
	            localOffSet += newStmt -> width;

	            //done
	        }
	
	        child = child -> next_sibling;
	    }
	
	    return width;

	}
	
	int insertStmt(astNode child, FuncNode fNode, char* lexeme, int localOffSet, bool isDriver, int nest, bool displayErrors){
	
	    int orderOfStmt = 1;

	    HashEntry* symTable = fNode -> localSymTable;
	    
	    int width = 0;
	
	    while(child != NULL ){
	        //printf("%s hey\n", child -> g -> str_name);
	        if(child -> label == TAG_END){
	        	//printf("TAG_ENd\n");
	        	fNode -> end = child -> g -> tok -> rownum;
	        }
	        else if(child -> label == TAG_GETVALUESTMT || child -> label == TAG_PRINTSTMT || child -> label == TAG_ASSIGNMENTSTMT){
	            //child = child -> next_sibling;
	            //continue;
	            //DO NOTHING FOR NOW
	            //printf("%s\n", child -> g -> str_name);
	        }
	        else if(child -> label == TAG_MODULEREUSESTMT){
	            //printf("%s\n", child -> g -> str_name);
	            int error=0;
	            if(!isDriver){
	                astNode calledFunc = child -> first_child -> next_sibling;
	                //printf("%s\n", calledFunc -> g -> tok ->tokenVal);
	                if(strcmp(calledFunc -> g -> tok ->tokenVal,lexeme) == 0){
	                	if(displayErrors)
	                    	printf("ERROR: LINE NO: %d: Recursive Call of %s is not Allowed\n", calledFunc -> g -> tok -> rownum, calledFunc -> g -> tok -> tokenVal);
	                    error = 1;
	                    symbolTableErrors ++;
	                }
	            }


	            //new
	            astNode c = child->first_child->next_sibling;
	            
	            int h = hash(c->g->tok->tokenVal);
	            //printf("were here\n");
	            FuncNode f = (FuncNode)retrieve(c->g->tok->tokenVal,h,globalSymTable,IS_FUNCTION);
	            if(f==NULL)
	            {
	            	if(displayErrors)
	            		printf("ERROR: LINE NO: %d: Function %s not declared or defined before use\n",c->g->tok->rownum,c->g->tok->tokenVal);
	            	error=1;
	            	symbolTableErrors ++;
	            	//printf("error is 1\n");
	            }
	            

	            else if((f->isDeclared == true) && (f->isDefined==true))
	            {
	            	if(f->isUsed==false){
	            		if(displayErrors)
	            			printf("ERROR: LINE NO: %d: Function %s has been both declared and defined before use\n",f->infoAboutNode->tok->rownum,f->infoAboutNode->tok->tokenVal);
	            			//printf("Line no. %d: Function %s has been both declared and defined before use\n",c->g->tok->rownum,f->infoAboutNode->tok->tokenVal);
	            		error =1;
	            		symbolTableErrors ++;
	            	}
	            }

	            //else printf("else\n");

	            
				
				if(error==0)
				{
					f->isUsed = true;
				}
				//new




	        }
	        else if(child -> label == TAG_DECLARESTMT){
	            //printf("%s\n", child -> g -> str_name);
	            astNode decID = child -> first_child;
	            astNode dataType = decID;
	
	            while(dataType -> label != TAG_DATATYPE){
	                dataType = dataType -> next_sibling;
	            }
	
	            //finding datatype of all ids in idlist
	            TYPE temp_type;
	            int temp_width, temp_offset;
	            astNode temp_startIndex;
	            astNode temp_endIndex;
	            bool temp_isArray, temp_isAssigned;
	            bool temp_isStatic = true;
	            int lowerBound = 0, upperBound = 0;
	
	            if(dataType->first_child == NULL){ // not an array
	                temp_isArray = false;
	                if(strcmp(dataType -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    temp_type = TYPE_INT;
	                    temp_width =SIZEOFINT;
	                    temp_offset = localOffSet; 
	                }
	                else if(strcmp(dataType -> g -> tok -> tokenType, "T_REAL") == 0){
	                    temp_type = TYPE_REAL;
	                    temp_width = SIZEOFREAL;
	                    temp_offset = localOffSet;  
	                }
	                else if(strcmp(dataType -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    temp_type = TYPE_BOOL;
	                    temp_width = SIZEOFBOOL;
	                    temp_offset = localOffSet;
	                }   
	                
	                temp_isAssigned = false;
	                temp_startIndex = NULL;
	                temp_endIndex = NULL;
	            }
	            
	            else{ //is an array
	
	                //sheffy
	                temp_isArray = true;
	                astNode childOfArr = dataType->first_child;
	                childOfArr = childOfArr -> next_sibling;
	                temp_startIndex = childOfArr -> first_child ;
	                temp_endIndex = childOfArr -> first_child -> next_sibling;
	
	                if(temp_startIndex -> label == TAG_IDNODE || temp_endIndex -> label == TAG_IDNODE){
	                    temp_isStatic = false;
	                    temp_width = DEFAULT_DYNAMIC_SIZE;
	                }
	
	                else{
	                    lowerBound = atoi(temp_startIndex -> g -> tok -> tokenVal);
	                    upperBound = atoi(temp_endIndex -> g -> tok -> tokenVal);
	                    temp_width = (upperBound - lowerBound +1);
	                }
	
	                childOfArr = childOfArr -> next_sibling;
	                if(strcmp(childOfArr -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    temp_type = TYPE_INT;
	                    if(temp_isStatic){
	                        temp_width = SIZEOFINT*temp_width + 1; //*(temp_endIndex - temp_startIndex +1);
	
	
	                    }
	                    temp_offset = localOffSet; 
	                }
	                else if(strcmp(childOfArr -> g -> tok -> tokenType, "T_REAL") == 0){
	                    temp_type = TYPE_REAL;
	                    if(temp_isStatic){
	                        temp_width = SIZEOFREAL*temp_width + 1;//*(temp_endIndex - temp_startIndex +1);
	
	
	                    }
	                    temp_offset = localOffSet;  
	                }
	                else if(strcmp(childOfArr -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    temp_type = TYPE_BOOL;
	                    if(temp_isStatic){
	                        temp_width = SIZEOFBOOL*temp_width + 1;//*(temp_endIndex - temp_startIndex +1);
	
	
	                    }
	                    temp_offset = localOffSet;
	                }   
	                
	                temp_isAssigned = false;
	            }
	            
	            while(decID -> label == TAG_IDNODE){
	
	                int hashVal = hash(decID -> g -> tok -> tokenVal);
			    
			        if(retrieve (decID -> g -> tok -> tokenVal, hashVal, symTable, IS_ID) != NULL){
			        	if(displayErrors)
	                    	printf("ERROR: LINE NO: %d: Variable %s already declared within scope\n", decID -> g -> tok -> rownum, decID -> g -> tok -> tokenVal);
	                    symbolTableErrors ++;
	                }
	
	                else{
	                    IDNode newID = (IDNode)malloc(sizeof(struct iDNode));
	                    newID -> infoAboutNode = decID -> g;
	                    newID -> type = temp_type;
	                    newID -> width = temp_width;
	                    newID -> offset = temp_offset;
	                    newID -> isArray = temp_isArray; 
	                    newID -> isAssigned = temp_isAssigned;
	                    newID -> isUsed = false;
	                    newID -> scope.func = fNode;
	                    newID -> scopeType = IS_FUNCTION;
	                    newID -> moduleName = lexeme;
	                    newID -> nestingLevel = nest; 
	                    if(temp_startIndex)
	                        newID -> startIndex = temp_startIndex -> g;
	                    else
	                        newID -> startIndex = NULL;
	                    if(temp_endIndex)
	                        newID -> endIndex = temp_endIndex -> g;
	                    else
	                        newID -> endIndex = NULL;
	                    newID -> upBound = upperBound;
	                    newID -> lowBound = lowerBound;
	                    newID -> isStatic = temp_isStatic;
	
	                    localOffSet += newID -> width;
	                    width += newID -> width;
	                    insertID(newID,symTable);
	                    //adding
	                    //decID->table = symTable;
	                }
	                //done
	                decID = decID -> next_sibling;
	            }
	        }
	
	        else if(child -> label == TAG_CONDITIONALSTMT){
	
	           // printf("%s\n", child -> g -> str_name);
	
	            int condLocalOffset = 0;
	
	            astNode condChild = child -> first_child -> next_sibling -> next_sibling;
	
	            StmtNode newStmt = (StmtNode)malloc(sizeof(struct stmtNode));
	            newStmt -> infoAboutNode = child -> g;
	            newStmt -> iterators = (LinkedList)malloc(sizeof(struct linkedList));
	            newStmt -> start = child -> first_child -> next_sibling -> g -> tok -> rownum;
	
	
	
	
	            //printf("%d ciao\n", child ->first_child->g -> tok -> rownum);
	            newStmt -> offset = localOffSet;
	            //newStmt -> parentSymTable = symTable;
	
	            int w = 0;

	            insertStmtNode(newStmt, symTable, orderOfStmt++);
	            //adding
	            newStmt -> parentType = IS_FUNCTION;
	            newStmt->parent.func = fNode;
	
	            while(condChild != NULL && condChild -> label == TAG_CASESTMT){
	
	                //printf("Case\n");
	
	                astNode caseChild = condChild -> first_child -> next_sibling;
	
	                if(caseChild != NULL)
	                    w = insertStmtAdv(caseChild, newStmt, lexeme,localOffSet, isDriver, nest+1, displayErrors);
	
	                newStmt -> width += w;
	                condLocalOffset += w;
	
	                condChild = condChild -> next_sibling;
	
	            }
	
	            
	            if(condChild != NULL && condChild -> label == TAG_DEFAULTSTMT){
	                //printf("Default\n");
	                newStmt -> width += insertStmtAdv(condChild -> first_child, newStmt, lexeme,localOffSet,isDriver, nest+1, displayErrors);
	            }

	            if(condChild != NULL && condChild -> label == TAG_END){
	            	newStmt -> end = condChild-> g -> tok -> rownum;
	            }
	
	
	            width += newStmt -> width;
	            localOffSet += newStmt -> width;

	            //done
	            
	        }
	        else if(child -> label == TAG_FORSTMT){
	
	            //printf("%s\n", child -> g -> str_name);

	            //getting IDNode for the iterator of for loop
	            int hashVal = hash(child -> first_child -> g -> tok -> tokenVal);
	            IDNode forIt = retrieve(child -> first_child -> g -> tok -> tokenVal, hashVal, symTable, IS_ID);

	            if(forIt == NULL){
	            	//printf("Line no. %d: Iterator %s not declared\n", child -> first_child -> g -> tok -> rownum, child -> first_child -> g -> tok -> tokenVal);
	            	forIt = (IDNode)malloc(sizeof(struct iDNode));
	            	forIt -> infoAboutNode = child -> first_child -> g;
	            	forIt -> width = 0;
					forIt -> offset = 0;
					forIt -> isArray = false;
					forIt -> isStatic = false;
					forIt -> startIndex = 0;
					forIt -> endIndex = 0;
					forIt -> upBound = 0;
					forIt -> lowBound = 0;
					forIt -> isAssigned = true;
					forIt -> isUsed = false;

	            }

	            //child -> first_child -> g -> tok -> tokenVal;
	            astNode forChild = child -> first_child;
	            StmtNode newStmt = (StmtNode)malloc(sizeof(struct stmtNode));
	            newStmt -> infoAboutNode = child -> g;
	            newStmt -> iterators = (LinkedList)malloc(sizeof(struct linkedList));
	            newStmt -> start = child -> first_child -> next_sibling -> next_sibling -> g -> tok -> rownum;

	            insertLL(forIt, newStmt->iterators);

	
	            forChild = forChild -> next_sibling -> next_sibling; 
	            // printf("%s in for\n", forChild -> g -> str_name);
	            // going directly to node corresponding to stmnts

	            insertStmtNode(newStmt, symTable, orderOfStmt++);
	            //adding
	            newStmt -> parentType = IS_FUNCTION;
	            newStmt-> parent.func = fNode;

	            if(forChild != NULL)
	                newStmt -> width += insertStmtAdv(forChild, newStmt, lexeme,localOffSet,isDriver, nest+1, displayErrors); 
	
	            newStmt -> offset = localOffSet;
	            //newStmt -> parentSymTable = symTable;
	            width += newStmt -> width;
	
	            localOffSet += newStmt -> width;
	
	            

	            //done
	
	        }
	        else if(child -> label == TAG_WHILESTMT){
	            //printf("%s\n", child -> g -> str_name);
	            astNode whileChild = child -> first_child;
	            StmtNode newStmt = (StmtNode)malloc(sizeof(struct stmtNode));
	            newStmt -> infoAboutNode = child -> g;
	            newStmt -> iterators = (LinkedList)malloc(sizeof(struct linkedList));
	            newStmt -> start = child -> first_child -> next_sibling -> g -> tok -> rownum;
	
	            whileChild = whileChild -> next_sibling;

	            insertStmtNode(newStmt, symTable, orderOfStmt++);
	            //adding
	            newStmt -> parentType = IS_FUNCTION;
	            newStmt-> parent.func = fNode;
	
	            if(whileChild!= NULL)
	                newStmt -> width += insertStmtAdv(whileChild, newStmt, lexeme,localOffSet,isDriver, nest+1, displayErrors); 
	            newStmt -> offset = localOffSet;
	            //newStmt -> parentSymTable = symTable;
	            width += newStmt -> width;
	
	            localOffSet += newStmt -> width;
	

	            //done
	        }
	
	        child = child -> next_sibling;
	    }
	
	    return width;
	}
	
	void insertLL(IDNode id, LinkedList list){
	
	    if(list == NULL)
	        return;
	
	    //printf("Inserting in List\n");
	
	    if(list -> parameter == NULL){
	        list -> parameter = id;
	        list -> next = NULL;
	    }
	    else{
	        LinkedList temp = list;
	        while(temp -> next){
	            temp = temp -> next;
	        }
	        temp -> next = (LinkedList)malloc(sizeof(struct linkedList));
	        temp -> next -> parameter = id;
	        temp -> next -> next = NULL;
	
	    }
	
	}
	
	IDNode retrieveFromLL(char* lexeme , LinkedList list){
	    if(list == NULL)
	        return NULL;
	
	    if(list -> parameter == NULL)
	        return NULL;
	    else{
	        LinkedList temp = list;
	        while(temp){
	            //printf("%s\n", temp -> parameter -> infoAboutNode -> tok -> tokenVal);
	            if(strcmp(temp -> parameter -> infoAboutNode -> tok -> tokenVal, lexeme) == 0)
	                return temp -> parameter;
	            temp = temp -> next;
	        }
	    }
	
	    return NULL;
	    
	
	}
	
	void insertDefinitionFunction(astNode root, HashEntry symTable[TABLESIZE], bool displayErrors){
	    int hashVal = hash(root->g->tok->tokenVal);
	
	   // printf("%s\n", root->g->tok->tokenVal);
	    //printf("%d\n", hashVal);
	
	    FuncNode temp = (FuncNode)retrieve(root->g->tok->tokenVal, hashVal, symTable, IS_FUNCTION);
	
	    if(temp == NULL){ //not declared before
	
	        //printf("Function has not been declared\n");
	
	        temp = (FuncNode)malloc(sizeof(struct funcNode));
	        temp -> infoAboutNode = root -> g;
	        //temp -> localSymTable = NULL;
	        //temp -> inputPList = NULL;
	        //temp -> outputPList = NULL;
	        temp -> isDriver = false;
	        temp -> order = FUNCTIONORDER++;
	        temp -> isDeclared = false;
	        temp -> isDefined = true;
	        temp->isUsed = false;
	        temp -> width = 0;
	        temp -> offset = GLOBALOFFSET;
	        //temp -> inputPList = (LinkedList)malloc(sizeof(struct linkedList));
	        //temp -> outputPList = (LinkedList)malloc(sizeof(struct linkedList));
	
	        HashEntry entry = (HashEntry)malloc(sizeof(struct hashEntry));
	        entry -> nodeType = IS_FUNCTION;
	        entry -> node.f = temp;
	        entry -> next = NULL;
	
	        //printf("%d\n",entry->node.f->order);
	
	        if(symTable[hashVal] == NULL){
	            symTable[hashVal] = entry;
	        }
	        else{
	            HashEntry it = symTable[hashVal];
	            while(it->next){
	                it = it->next;
	            }
	            it->next = entry;
	        }
	        //adding
	        //root->table = symTable;
	        //done
	    }
	    else if(temp != NULL && temp->isDefined == true){
	    	if(displayErrors)
	    		printf("ERROR: LINE NO: %d:The Function %s has already been defined\n", root -> g -> tok -> rownum, root -> g -> tok -> tokenVal);
	        	//printf("Line no. %d:The Function %s has already been defined\n", temp -> infoAboutNode -> tok -> rownum, temp -> infoAboutNode -> tok -> tokenVal);
	        symbolTableErrors ++;
	        return;
	    }
	
	    else if(temp != NULL && temp->isDefined == false){ //declared but not defined
	        //printf("Declared not defined yet \n");
	        temp -> infoAboutNode = root -> g; //not really required
	        temp -> isDefined = true;
	        temp -> offset = GLOBALOFFSET;
	        //temp -> inputPList = (LinkedList)malloc(sizeof(struct linkedList));
	        //temp -> outputPList = (LinkedList)malloc(sizeof(struct linkedList));
	        //printf("%d OFFSEt of func\n", temp->offset);
	    }
	
	    root = root->next_sibling;
	
	    //printf("%d\n",root -> label);
	
	    int localOffset = 0;
	    while(root != NULL){
	        if(root -> label == TAG_INPUTPARAMETER){

	        	if(temp -> inputPList == NULL){
	        		temp -> inputPList = (LinkedList)malloc(sizeof(struct linkedList));
	        	}
	
	
	            astNode child = root -> first_child;
	            int hashVal = hash(child -> g -> tok -> tokenVal);
	
	            //printf("%s\n",child -> g -> tok -> tokenVal); 
	
	            if(retrieveFromLL(child -> g -> tok -> tokenVal, temp -> inputPList) != NULL){
	            	if(displayErrors)
	                	printf("ERROR: LINE NO: %d: The input variable %s has already been declared\n",  child-> g -> tok -> rownum, child-> g -> tok -> tokenVal);
	                symbolTableErrors ++;
	            }
	
	            //change for linkedlist
	            // if(retrieve(child -> g -> tok -> tokenVal, hashVal, temp -> inputPList, IS_ID) != NULL){
	            //     printf("Line no %d: The input variable has already been declared\n",  child-> g -> tok -> rownum);
	            // }
	
	            astNode typeOfChild = child -> next_sibling;
	
	            //printf("%d\n",typeOfChild -> label);
	
	            IDNode inputID = (IDNode)malloc(sizeof(struct iDNode));
	            inputID -> infoAboutNode = child -> g;
	
	            if(typeOfChild->first_child == NULL){ // not an array
	                inputID -> isArray = false;
	                if(strcmp(typeOfChild -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    inputID -> type = TYPE_INT;
	                    inputID -> width = SIZEOFINT;
	                    inputID -> offset = localOffset; 
	                }
	                else if(strcmp(typeOfChild -> g -> tok -> tokenType, "T_REAL") == 0){
	                    inputID -> type = TYPE_REAL;
	                    inputID -> width = SIZEOFREAL;
	                    inputID -> offset = localOffset;  
	                }
	                else if(strcmp(typeOfChild -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    inputID -> type = TYPE_BOOL;
	                    inputID -> width = SIZEOFBOOL;
	                    inputID -> offset = localOffset;   
	                }
	
	                inputID -> isAssigned = true;
	                inputID -> startIndex = NULL;
	                inputID -> endIndex = NULL;
	                inputID -> upBound = 0;
	                inputID -> lowBound = 0;
	                    
	            }
	            else{ 
	
	                //int lowerBound = 0, upperBound = 0;
	
	                inputID -> isArray = true;
	                astNode childOfArr = typeOfChild->first_child;
	                childOfArr = childOfArr -> next_sibling;
	                astNode temp_startIndex = childOfArr -> first_child;
	                astNode temp_endIndex = childOfArr -> first_child -> next_sibling;
	                inputID -> startIndex = temp_startIndex -> g;
	                inputID -> endIndex = temp_endIndex -> g;
	
	                if(temp_startIndex -> label == TAG_IDNODE || temp_endIndex -> label == TAG_IDNODE){
	                    inputID -> isStatic = false;
	                    inputID -> width = DEFAULT_DYNAMIC_SIZE;
	                }
	
	                else{
	                    inputID -> isStatic = true;
	                    inputID -> lowBound = atoi(temp_startIndex -> g -> tok -> tokenVal);
	                    inputID -> upBound = atoi(temp_endIndex -> g -> tok -> tokenVal);
	                    inputID -> width = (inputID -> upBound - inputID -> lowBound +1);
	                }
	
	                childOfArr = childOfArr -> next_sibling;
	                if(strcmp(childOfArr -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    inputID -> type = TYPE_INT;
	                    if(inputID -> isStatic)
	                        inputID -> width = SIZEOFINT*inputID->width + 1;//(temp_endIndex - temp_startIndex +1);
	                    inputID -> offset = localOffset; 
	                }
	                else if(strcmp(childOfArr -> g -> tok -> tokenType, "T_REAL") == 0){
	                    inputID -> type = TYPE_REAL;
	                    if(inputID -> isStatic)
	                        inputID -> width = SIZEOFREAL*inputID->width + 1;//*(temp_endIndex - temp_startIndex +1);
	                    inputID -> offset = localOffset;  
	                }
	                else if(strcmp(childOfArr -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    inputID -> type = TYPE_BOOL;
	                    if(inputID -> isStatic)
	                        inputID -> width = SIZEOFBOOL*inputID->width + 1;//*(temp_endIndex - temp_startIndex +1);
	                    inputID -> offset = localOffset;   
	                }
	
	                //printf("Width = %d\n", inputID -> width);
	
	                inputID -> isAssigned = true;
	                inputID -> isUsed = false;
	                
	                //sheffy
	            }
				
				inputID -> scope.func = temp;
	            inputID -> scopeType = IS_FUNCTION;
	            inputID -> moduleName = temp -> infoAboutNode -> tok -> tokenVal;
	            inputID -> nestingLevel = 0;  
	            //temp -> width += inputID -> width;
	            //insertID(inputID,temp -> inputPList);
	            insertLL(inputID,temp -> inputPList);
	            //adding
	            //child->table = temp->inputPList;
	            //done
	            localOffset += inputID -> width;            
	        }
	
	        else if( root -> label == TAG_OUTPUTPARAMETER){


	        	if(temp -> outputPList == NULL){
	        		temp -> outputPList = (LinkedList)malloc(sizeof(struct linkedList));
	        	}
	            astNode child = root -> first_child;
	
	            //change for linkedlist
	
	            if(retrieveFromLL(child -> g -> tok -> tokenVal, temp -> outputPList) != NULL){
	            	if(displayErrors)
	                	printf("ERROR: LINE NO:%d: The output variable %s has already been declared\n",  child-> g -> tok -> rownum, child-> g -> tok -> tokenVal);
	                symbolTableErrors ++;
	            }
	
	            // if(retrieve(child -> g -> tok -> tokenVal, hashVal, temp -> outputPList, IS_ID) != NULL){
	            //     printf("Line no %d: The input variable has already been declared\n",  child-> g -> tok -> rownum);
	            // }
	
	            while(child != NULL){    
	                astNode typeOfChild = child -> next_sibling;
	
	                IDNode outputID = (IDNode)malloc(sizeof(struct iDNode));
	                outputID -> infoAboutNode = child -> g;
	                outputID -> isArray = false;
	                if(strcmp(typeOfChild -> g -> tok -> tokenType, "T_INTEGER") == 0){
	                    outputID -> type = TYPE_INT;
	                    outputID -> width = SIZEOFINT;
	                    outputID -> offset = localOffset; 
	                }
	                else if(strcmp(typeOfChild -> g -> tok -> tokenType, "T_REAL") == 0){
	                    outputID -> type = TYPE_REAL;
	                    outputID -> width = SIZEOFREAL;
	                    outputID -> offset = localOffset;  
	                }
	                else if(strcmp(typeOfChild -> g -> tok -> tokenType, "T_BOOLEAN") == 0){
	                    outputID -> type = TYPE_BOOL;
	                    outputID -> width = SIZEOFBOOL;
	                    outputID -> offset = localOffset;   
	                }
	                outputID -> isAssigned = false;
	                outputID -> isUsed = false;
	                outputID -> startIndex = NULL;
	                outputID -> endIndex = NULL;
	                outputID -> scope.func = temp;
	                outputID -> scopeType = IS_FUNCTION;
	                outputID -> moduleName = temp -> infoAboutNode -> tok -> tokenVal;
	                outputID -> nestingLevel = 0;
	
	                //insert in the output hash table
	                //insertID(outputID, temp -> outputPList);
	                insertLL(outputID, temp -> outputPList);
	                insertID(outputID, temp -> localSymTable);
	                //adding
	                //child->table = temp->outputPList;
	                //done
	                //temp -> width += outputID -> width;
	                localOffset += outputID -> width;
	
	                child = child -> next_sibling -> next_sibling;
	            }
	        }
	
	        else if( root->label == TAG_MODULEDEF){ // please include this in case 24 as a new node

	        	temp -> start = root -> first_child -> g -> tok -> rownum;
	            if(root -> first_child -> next_sibling -> label != TAG_END){
	                //printf("Statements exist\n");
	                astNode child = root -> first_child -> next_sibling; // other wise this won't work
	                temp -> width += insertStmt(child, temp, temp -> infoAboutNode -> tok -> tokenVal, 0,temp -> isDriver,1, displayErrors);
	            }
	        }
	
	        root = root ->next_sibling;
	    }
	
	    GLOBALOFFSET += temp->width;
	
	}
	
	// SymbolTable initialiseSymbolTable(SymbolTable s){
	//     s = (SymbolTable)malloc(sizeof(struct symTable));
	//     s -> head = (HashEntry)malloc(sizeof(struct hashEntry));
	//     s -> noOfElements = 0;
	//     return s;
	// }
	
	void insertDriver(astNode root, HashEntry symTable[TABLESIZE], bool displayErrors){
	
	
	    //int hashVal = root->label;
	
	   //printf("%d\n",hashVal);
	
	    FuncNode temp = (FuncNode)malloc(sizeof(struct funcNode));
	    temp -> infoAboutNode = root -> g; //drivermodule ka grammar block
	    temp -> isDriver = true;
	    temp -> order = FUNCTIONORDER++;
	    temp -> isDeclared = true;
	    temp -> isDefined = true;
	    temp->isUsed = false;
	    temp -> width = 0;
	    temp -> offset = GLOBALOFFSET;
	    temp->start= root->first_child->first_child->g->tok->rownum;
	    driverFunc = temp;
	
	    //printf("%d", GLOBALOFFSET);
	
	    // HashEntry entry = (HashEntry)malloc(sizeof(struct hashEntry));
	    // entry -> nodeType = IS_FUNCTION;
	    // entry -> node.f = temp;
	    // entry -> next = NULL;
	
	    // if(symTable[hashVal] == NULL){
	            
	    //         symTable[hashVal] = entry;
	    //         //printf("First\n");
	    // }
	    // else{
	    //     HashEntry it = symTable[hashVal];
	    //     while(it->next){
	    //         it = it->next;
	    //     }
	    //     it->next = entry;
	    // }


	    //adding
	    //root->table = symTable;
	    //done
	
	
	    //printf("hello\n");
	    //printf("%s\n",symTable[hashVal] -> node.f -> infoAboutNode -> str_name);
	
	    astNode stmts = root->first_child; //moduledef node

	    //driverFunc -> start = root -> first_child -> g -> tok -> rownum;
	
	    int localOffset = 0;
	
	    if(stmts -> first_child){
	        //printf("Child\n");
	        stmts = stmts -> first_child->next_sibling;
	        temp -> width += insertStmt(stmts, temp, "Driver", localOffset, temp -> isDriver,1, displayErrors);
	    }
	
	    GLOBALOFFSET += temp -> width;
	
	    //printf("%d", GLOBALOFFSET);
	
	}

	void printID(IDNode id){

	printf("%s 	%s 	", id -> infoAboutNode -> tok -> tokenVal, id -> moduleName);

	if(id -> scopeType == IS_FUNCTION){
		printf("%d-%d 	%d 	", id -> scope.func -> start,  id -> scope.func -> end, id -> width);
	}
	else if(id -> scopeType == IS_STATEMENT){
		printf("%d-%d 	%d 	", id -> scope.stmt -> start,  id -> scope.stmt -> end, id -> width);
	}

	if(id -> isArray){
		printf("Yes  ");

		if(id -> isStatic){
			printf("Static Array  ");
		}
		else{
			printf("Dynamic Array  ");
		}

		printf("[%s,%s]  ", id -> startIndex -> tok -> tokenVal, id -> endIndex -> tok -> tokenVal);
	}
	else{
		printf("No  --  --  ");
	}

	if(id -> type == TYPE_INT)
		printf("Integer  ");
	else if(id -> type == TYPE_REAL)
		printf("Real  ");
	else if(id -> type == TYPE_BOOL)
		printf("Boolean  ");

	printf("%d  %d\n", id -> offset, id-> nestingLevel );
}

void printInputPList(LinkedList list){

	if(list == NULL)
		return;
	LinkedList temp = list;

	while(temp){
		if(temp->parameter) printID(temp -> parameter);
		temp = temp -> next;
	}
}



void printSymbolTable(HashEntry symTable[TABLESIZE]){

	for(int i = 0; i<TABLESIZE ;i ++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry temp = symTable[i];
			while(temp){
				if(temp -> nodeType == IS_ID){
					printID(temp -> node.i);
				}
				temp = temp -> next;
			}
		}
	}

	for(int i = 0; i<TABLESIZE ;i ++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry temp = symTable[i];
			while(temp){
				if(temp -> nodeType == IS_FUNCTION){
					printInputPList(temp -> node.f -> inputPList);
					printSymbolTable(temp -> node.f -> localSymTable);
				}
				else if(temp -> nodeType == IS_STATEMENT){
					printSymbolTable(temp -> node.s -> localSymTable);
				}
				temp = temp -> next;
			}
		}
	}

}

void printDriver(){

	HashEntry* symTable = driverFunc -> localSymTable;

	for(int i = 0; i<TABLESIZE ;i ++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry temp = symTable[i];
			while(temp){
				if(temp -> nodeType == IS_ID){
					printID(temp -> node.i);
				}
				temp = temp -> next;
			}
		}
	}

	for(int i = 0; i<TABLESIZE ;i ++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry temp = symTable[i];
			while(temp){
				if(temp -> nodeType == IS_FUNCTION){
					printSymbolTable(temp -> node.f -> localSymTable);
				}
				else if(temp -> nodeType == IS_STATEMENT){
					printSymbolTable(temp -> node.s -> localSymTable);
				}
				temp = temp -> next;
			}
		}
	}

}

void printArr(IDNode id){

	if(id -> isArray){

		if(id -> scopeType == IS_STATEMENT)
			printf("%s  %d-%d  %s  ", id -> moduleName, id -> scope.stmt -> start,  id -> scope.stmt -> end, id -> infoAboutNode -> tok -> tokenVal);
		else if (id -> scopeType == IS_FUNCTION)
			printf("%s  %d-%d  %s  ", id -> moduleName, id -> scope.func -> start,  id -> scope.func -> end, id -> infoAboutNode -> tok -> tokenVal);

		if(id -> isStatic){
			printf("Static Array  ");
		}
		else{
			printf("Dynamic Array  ");
		}

		printf("[%s,%s]  ", id -> startIndex -> tok -> tokenVal, id -> endIndex -> tok -> tokenVal);

		if(id -> type == TYPE_INT)
			printf("Integer\n");
		else if(id -> type == TYPE_REAL)
			printf("Real\n");
		else if(id -> type == TYPE_BOOL)
			printf("Boolean\n");
	}
}

void printInputPListArr(LinkedList list){

	if(list == NULL)
		return;
	LinkedList temp = list;

	while(temp){
		if(temp->parameter) printArr(temp -> parameter);
		temp = temp -> next;
	}
}

void printArrays(HashEntry symTable[TABLESIZE]){

	for(int i = 0; i<TABLESIZE ;i ++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry temp = symTable[i];
			while(temp){
				if(temp -> nodeType == IS_ID){
					printArr(temp -> node.i);
				}
				temp = temp -> next;
			}
		}
	}

	for(int i = 0; i<TABLESIZE ;i ++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry temp = symTable[i];
			while(temp){
				if(temp -> nodeType == IS_FUNCTION){
					printInputPListArr(temp -> node.f -> inputPList);
					printArrays(temp -> node.f -> localSymTable);
				}
				else if(temp -> nodeType == IS_STATEMENT){
					printArrays(temp -> node.s -> localSymTable);
				}
				temp = temp -> next;
			}
		}
	}

}

void printFunctions(){

	for(int i = 0; i<TABLESIZE ;i ++){
		if(globalSymTable[i] == NULL)
			continue;
		else{
			HashEntry temp = globalSymTable[i];
			while(temp){
				if(temp -> nodeType == IS_FUNCTION){
					printf("%s  %d\n", temp -> node.f -> infoAboutNode -> tok -> tokenVal, temp -> node.f -> width);
				}
				temp = temp -> next;
			}
		}
	}

	printf("Driver  %d\n", driverFunc-> width);

}
	
	void createSymbolTable(astNode root, bool displayErrors){
	
	    printf("Starting creation of SymbolTable\n");
	
	    root = root -> first_child;
	
	    if(root != NULL && root -> label == TAG_MODULEDEC ){
	
	        if(root -> first_child == NULL){
	            //printf("No Declaration\n");
	            // root = root->next_sibling;
	            // break;
	        }
	        else{
	            astNode dec = root -> first_child;
	            while(dec != NULL){
	                insertDeclarationFunction(dec,globalSymTable, displayErrors);
	                dec = dec -> next_sibling;
	            }
	        }
	        root = root->next_sibling;
	    }
	
	    
	    //printf("Done with Declarations\n");
	    
	    if( root != NULL && root -> label == TAG_OTHERMODULES  ){
	
	        // if(root -> first_child == NULL){
	        //     printf("No Definition\n");
	        //     root = root->next_sibling;
	        //     break;
	        // }
	
	        // printf("Definition %d", FUNCTIONORDER);
	
	        // insertDefinitionFunction(root->first_child->first_child, globalSymTable);
	        // root = root->next_sibling;
	
	        astNode mod = root->first_child;
	
	        while(mod != NULL){
	            //printf("Definition %d\n", FUNCTIONORDER);
	
	            insertDefinitionFunction(mod->first_child, globalSymTable, displayErrors);
	
	            mod = mod -> next_sibling;
	        }
	
	        root = root->next_sibling;
	
	    }
	
	    //printf("Done with Definitions\n");
	
	    if(root -> label == TAG_DRIVERMODULE  && root != NULL){
	
	        //printf("Driver\n");
	        //printf("%d\n",root->label);
	        insertDriver(root , globalSymTable, displayErrors);
	        root = root->next_sibling;
	    }
	
	    //printf("Driver done\n");
	
	    if(root -> label == TAG_OTHERMODULES  && root != NULL){
	
	        // if(root -> first_child == NULL){
	        //     printf("No Definition\n");
	        //     root = root->next_sibling;
	        //     break;
	        // }
	
	        // printf("Definition %d", FUNCTIONORDER);
	
	        // insertDefinitionFunction(root->first_child->first_child, globalSymTable);
	        // root = root->next_sibling;
	
	        astNode mod = root->first_child;
	
	        while(mod != NULL){
	            //printf("Definition %d\n", FUNCTIONORDER);
	
	            insertDefinitionFunction(mod->first_child, globalSymTable, displayErrors);
	
	            mod = mod -> next_sibling;
	        }
	
	        root = root->next_sibling;
	
	    }
	    //printf("Done with Definitions\n");

	    //printSymbolTable(globalSymTable);
	    //printDriver();
	    //printFunctions();
	    //printArrays(globalSymTable);
	
	    return;
	}
