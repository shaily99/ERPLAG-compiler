//Group-15
//Dishita Malav 2017A7PS0164P
//Shaily Bhatt 2017A7PS0040P
//Shefali Tripathi 2017A7PS0139P

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbolTable.h"
#include "semanticAnalyser.h"

HashEntry globalSymTable[TABLESIZE];
FuncNode driverFunc;

int switch_ctr = 0;
int while_ctr = 0;

int for_loop=0;

void writeData(FILE* fp, HashEntry* symTable){

	for(int i = 0; i < TABLESIZE; i++){
		if(symTable[i] == NULL)
			continue;
		else{
			HashEntry root = symTable[i];
			while(root != NULL){
				if(root -> nodeType == IS_ID){
					if(root -> node.i -> isArray == false){
						if(root -> node.i -> type == TYPE_INT){
							fprintf(fp, "%s%d_id: RESW 1\n", root -> node.i -> infoAboutNode -> tok -> tokenVal, root -> node.i -> nestingLevel);
						}
						else if(root -> node.i -> type == TYPE_REAL){
							fprintf(fp, "%s%d_id: RESD 1\n", root -> node.i -> infoAboutNode -> tok -> tokenVal, root -> node.i -> nestingLevel);
						}
						else if(root -> node.i -> type == TYPE_BOOL){
							fprintf(fp, "%s%d_id: RESW 1\n", root -> node.i -> infoAboutNode -> tok -> tokenVal, root -> node.i -> nestingLevel);
						}
					}
					else{
						if(root -> node.i -> isStatic == true){
							int width = root -> node.i -> upBound - root -> node.i -> lowBound + 1;
							if(root -> node.i -> type == TYPE_INT){
								fprintf(fp, "%s%d_id: RESW %d\n", root -> node.i -> infoAboutNode -> tok -> tokenVal, root -> node.i -> nestingLevel, width);
							}
							else if(root -> node.i -> type == TYPE_REAL){
								fprintf(fp, "%s%d_id: RESD %d\n", root -> node.i -> infoAboutNode -> tok -> tokenVal, root -> node.i -> nestingLevel, width);
							}
							else if(root -> node.i -> type == TYPE_BOOL){
								fprintf(fp, "%s%d_id: RESW %d\n", root -> node.i -> infoAboutNode -> tok -> tokenVal, root -> node.i -> nestingLevel, width);
							}
						}
						else{
							//Will see later
						}
					}
				}
				else if(root->nodeType == IS_STATEMENT){
					writeData(fp, root->node.s->localSymTable);
				}
				root = root->next;
			}
		}
	}
}

void writeArithmeticOrBoolean(FILE* fp, astNode op, int* labelNo, int offset, p parent, NODETYPE nodeType){

	if(op == NULL)
		return;

	else if(op -> label == TAG_VAR){
		//printf("var\n");
		if(op -> first_child -> label == TAG_IDNODE){
			//printf("id\n");
			//printf("%s\n",op -> first_child -> first_child -> g -> tok -> tokenVal );

			IDNode id = findIdutil(op -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> g -> tok -> rownum);
			if(id -> isArray == false){
				fprintf(fp, "MOV AX, [%s%d_id]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
				fprintf(fp, "MOV [rightOp], AX\n");
			}
			else{
				if(id->isStatic ==1){
					if(op -> first_child -> next_sibling -> label == TAG_NUMNODE){
						int index_1 = ((atoi(op -> first_child -> next_sibling -> g -> tok -> tokenVal) - id -> lowBound) * 2);
						fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel, index_1);
						fprintf(fp, "MOV [rightOp], AX\n");
					}
					else{
						astNode index = op -> first_child -> next_sibling;
						IDNode indexID = findIdutil(index-> g -> tok -> tokenVal, parent, nodeType, index-> g -> tok -> rownum);
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",id->lowBound);
						fprintf(fp, "JL EXIT_LABEL\n");
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",id->upBound);
						fprintf(fp, "JG EXIT_LABEL\n");
						//fprintf(fp, "JG  CONT_%s%d\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "SUB BL, %d\n", id->lowBound);
						fprintf(fp, "MOV AL, 2\n");
						fprintf(fp, "MUL BL\n");
						fprintf(fp, "MOV BX, AX\n");
						fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
						fprintf(fp, "MOV [rightOp], AX\n");
					}

				}
				else{
					//dynamic bounds not done
				}
				
			}
		}
		else if(op -> first_child -> label == TAG_NUMNODE){
			fprintf(fp, "MOV AX, %s\n", op -> first_child -> g -> tok -> tokenVal);
			fprintf(fp, "MOV [rightOp], AX\n");
		}

	}

	else if(op -> label == TAG_PLUSNODE || op->label == TAG_MINUSNODE){
		//printf("+\n");
		if(op -> first_child -> label == TAG_VAR){
			if(op -> first_child -> first_child -> label == TAG_IDNODE){
				IDNode leftid = findIdutil(op -> first_child -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> first_child -> g -> tok -> rownum);
				if(leftid -> isArray == false){
					fprintf(fp, "MOV AX, [%s%d_id]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
					fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);

				}
				else{
					if(op -> first_child -> first_child -> next_sibling -> label == TAG_NUMNODE){
						int index_1 = ((atoi(op -> first_child -> first_child -> next_sibling -> g -> tok -> tokenVal) - leftid -> lowBound) * 2);
						fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel, index_1);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
					else{
						astNode index = op -> first_child -> first_child -> next_sibling;
						IDNode indexID = findIdutil(index-> g -> tok -> tokenVal, parent, nodeType, index-> g -> tok -> rownum);
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->lowBound);
						fprintf(fp, "JL EXIT_LABEL\n");
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->upBound);		

						fprintf(fp, "JG EXIT_LABEL\n");
						//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "SUB BL, %d\n", leftid->lowBound);
						fprintf(fp, "MOV AL, 2\n");
						fprintf(fp, "MUL BL\n");
						fprintf(fp, "MOV BX, AX\n");
						fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
				}

				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);
			}
			else if(op -> first_child -> first_child -> label == TAG_NUMNODE){
				fprintf(fp, "MOV AX, %s\n", op -> first_child -> first_child -> g -> tok -> tokenVal);
				fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);

				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);

			}

			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
				if(op->label == TAG_PLUSNODE)
					fprintf(fp, "ADD AX, [rightOp]\n");
				else 
					fprintf(fp, "SUB AX, [rightOp]\n");
				//fprintf(fp, "MOV [rightOp], AX\n");
				if(op == op->parent->first_child)
					{
						fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
					}
				else
					fprintf(fp, "MOV [rightOp] , AX\n");

		}
		
		else{
			writeArithmeticOrBoolean(fp, op -> first_child, labelNo, offset , parent, nodeType);
			writeArithmeticOrBoolean(fp, op -> first_child->next_sibling, labelNo, offset+2, parent, nodeType);
			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
			if(op->label == TAG_PLUSNODE)
				fprintf(fp, "ADD AX, [rightOp]\n");
			else
				fprintf(fp, "SUB AX, [rightOp]\n");
			//fprintf(fp, "MOV [rightOp], AX\n");
			if(op == op->parent->first_child)
					{
						fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
					}
			else
				fprintf(fp, "MOV [rightOp] , AX\n");
		}
	}
	



	else if(op -> label == TAG_MULNODE || op->label == TAG_DIVNODE){
		if(op->first_child->label == TAG_VAR){
			if(op->first_child->first_child->label == TAG_IDNODE){
				IDNode leftid = findIdutil(op -> first_child -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> first_child -> g -> tok -> rownum);
				if(leftid -> isArray == false){
					fprintf(fp, "MOV AX, [%s%d_id]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
					fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
			    }
			    else{
					if(op -> first_child -> first_child -> next_sibling -> label == TAG_NUMNODE){
						int index_1 = ((atoi(op -> first_child -> first_child -> next_sibling -> g -> tok -> tokenVal) - leftid -> lowBound) * 2);
						fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel, index_1);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
					else{
						astNode index = op -> first_child -> first_child -> next_sibling;
						IDNode indexID = findIdutil(index -> g -> tok -> tokenVal, parent, nodeType, index -> g -> tok -> rownum);
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->lowBound);
						fprintf(fp, "JL EXIT_LABEL\n");
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->upBound);
						fprintf(fp, "JG EXIT_LABEL\n");
						//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "SUB BL, %d\n", leftid->lowBound);
						fprintf(fp, "MOV AL, 2\n");
						fprintf(fp, "MUL BL\n");
						fprintf(fp, "MOV BX, AX\n");
						fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					
					}
				}

				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);

			}
			   
		    else if(op -> first_child -> first_child -> label == TAG_NUMNODE){
				fprintf(fp, "MOV AX, %s\n", op -> first_child -> first_child -> g -> tok -> tokenVal);
				fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);

				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);

				
			}

			fprintf(fp, "MOV AX, [leftOp+%d]\n",offset);

					//fprintf(fp, "IMUL word [rightOp]\n");

			if(op->label==TAG_MULNODE)
				fprintf(fp, "IMUL word [rightOp]\n");
			else 
				fprintf(fp, "CWD\nIDIV word [rightOp]\n");

			if(op == op->parent->first_child)
			{
				fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
			}
			else
				fprintf(fp, "MOV [rightOp] , AX\n"); 

				//if this is second child of assignment stmt, then instead of saving to AX, gotta save to memory space of lhs id
				//confirm if what shef wrote is correct(can redundancy be removed?)
		}

		else{
			writeArithmeticOrBoolean(fp, op -> first_child, labelNo, offset, parent, nodeType);
			writeArithmeticOrBoolean(fp, op -> first_child->next_sibling, labelNo, offset+2, parent, nodeType);
			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
			if(op->label==TAG_MULNODE)
				fprintf(fp, "IMUL word [rightOp]\n");
			else 
				fprintf(fp, "CWD\nIDIV word [rightOp]\n");
			//fprintf(fp, "MOV [rightOp], AX\n");
			if(op == op->parent->first_child)
					{
						fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
					}
			else
				fprintf(fp, "MOV [rightOp] , AX\n"); 
		}

	}



	else if(op -> label == TAG_ANDNODE || op->label == TAG_ORNODE){
		if(op->first_child->label == TAG_VAR){
			if(op->first_child->first_child->label == TAG_IDNODE){
				IDNode leftid = findIdutil(op -> first_child -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> first_child -> g -> tok -> rownum);
				if(leftid -> isArray == false){
					fprintf(fp, "MOV AX, [%s%d_id]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
					fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
			    }
			    else{
					if(op -> first_child -> first_child -> next_sibling -> label == TAG_NUMNODE){
						int index_1 = ((atoi(op -> first_child -> first_child -> next_sibling -> g -> tok -> tokenVal) - leftid -> lowBound) * 2);
						fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel, index_1);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
					else{
						astNode index = op -> first_child -> first_child -> next_sibling;
						IDNode indexID = findIdutil(index-> g -> tok -> tokenVal, parent, nodeType, index-> g -> tok -> rownum);
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->lowBound);
						fprintf(fp, "JL EXIT_LABEL\n");
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->upBound);
						fprintf(fp, "JG EXIT_LABEL\n");
						//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "SUB BL, %d\n", leftid->lowBound);
						fprintf(fp, "MOV AL, 2\n");
						fprintf(fp, "MUL BL\n");
						fprintf(fp, "MOV BX, AX\n");
						fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
				}

				if(op->first_child->next_sibling->label==TAG_TRUENODE || op->first_child->next_sibling->label==TAG_FALSENODE){
						if(op->first_child->next_sibling->label==TAG_TRUENODE)
							fprintf(fp, "MOV [rightOp], 1\n");
						else
							fprintf(fp, "MOV [rightOp], 0\n");
				}
				else
					writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);
			}
			   
			else if(op -> first_child -> first_child -> label == TAG_NUMNODE){
				fprintf(fp, "MOV AX, %s\n", op -> first_child -> first_child -> g -> tok -> tokenVal);
				fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);

				if(op->first_child->next_sibling->label==TAG_TRUENODE || op->first_child->next_sibling->label==TAG_FALSENODE){
					if(op->first_child->next_sibling->label==TAG_TRUENODE)
						fprintf(fp, "MOV [rightOp], 1\n");
					else
						fprintf(fp, "MOV [rightOp], 0\n");
				}
				else
					writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);

			}

			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
			if(op->label == TAG_ANDNODE)
				fprintf(fp, "AND AX,[rightOp]\n");
			else
				fprintf(fp, "OR AX, [rightOp]\n");
			if(op == op->parent->first_child)
			{
				fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
			}
			else
				fprintf(fp, "MOV [rightOp] , AX\n"); 


		}

		else if(op->first_child->label == TAG_TRUENODE || op->first_child->label == TAG_FALSENODE)
		{
			//printf("yes\n");
			if(op->first_child->label==TAG_TRUENODE)
				fprintf(fp, "MOV [leftOp+%d], 1\n",offset);
			else
				fprintf(fp, "MOV [leftOp+%d], 0\n",offset);


			if(op->first_child->next_sibling->label==TAG_TRUENODE ||op->first_child->next_sibling->label==TAG_FALSENODE)
			{
				if(op->first_child->label==TAG_TRUENODE)
				 	fprintf(fp, "MOV [rightOp], 1\n");
				else
					fprintf(fp, "MOV [rightOp], 0\n");

			}

			else
			{
				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);
			}


			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
			if(op->label == TAG_ANDNODE)
				fprintf(fp, "AND AX,[rightOp]\n");
			else
				fprintf(fp, "OR AX, [rightOp]\n");
			if(op == op->parent->first_child)
			{
				fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
			}
			else
				fprintf(fp, "MOV [rightOp] , AX\n"); 

		}

		else{
			writeArithmeticOrBoolean(fp, op -> first_child, labelNo, offset, parent, nodeType);
			writeArithmeticOrBoolean(fp, op -> first_child->next_sibling, labelNo, offset+2, parent, nodeType);
			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
			if(op->label == TAG_ANDNODE)
				fprintf(fp, "AND AX,[rightOp]\n");
			else
				fprintf(fp, "OR AX, [rightOp]\n");
			if(op == op->parent->first_child)
			{
				fprintf(fp, "MOV [leftOp+%d], AX\n",offset);
			}
			else
				fprintf(fp, "MOV [rightOp] , AX\n"); 
		}


	}


	// else if(op -> label == TAG_ORNODE){
	// }



	else if(op -> label == TAG_GTNODE|| op->label == TAG_LTNODE || op->label == TAG_LENODE || op->label == TAG_GENODE|| op->label == TAG_EQNODE||op->label==TAG_NENODE){
		if(op->first_child->label == TAG_VAR){
			if(op->first_child->first_child->label == TAG_IDNODE){
				IDNode leftid = findIdutil(op -> first_child -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> first_child -> g -> tok -> rownum);
				if(leftid -> isArray == false){
					fprintf(fp, "MOV AX, [%s%d_id]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
					fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
			    }
			    else{
					if(op -> first_child -> first_child -> next_sibling -> label == TAG_NUMNODE){
						int index_1 = ((atoi(op -> first_child -> first_child -> next_sibling -> g -> tok -> tokenVal) - leftid -> lowBound) * 2);
						fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel, index_1);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
					else{
						astNode index = op -> first_child -> first_child -> next_sibling;
						IDNode indexID = findIdutil(index -> g -> tok -> tokenVal, parent, nodeType, index -> g -> tok -> rownum);
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->lowBound);
						fprintf(fp, "JL EXIT_LABEL\n");
						fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "CMP AX, %d\n",leftid->upBound);
						fprintf(fp, "JG EXIT_LABEL\n");
						//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
						int calv = leftid->lowBound;
						fprintf(fp, "SUB BL, %d\n", calv);
						fprintf(fp, "MOV AL, 2\n");
						fprintf(fp, "MUL BL\n");
						fprintf(fp, "MOV BX, AX\n");
						fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", leftid -> infoAboutNode -> tok -> tokenVal, leftid -> nestingLevel);
						fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);
					}
				}

				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);
			}
			   
			else if(op -> first_child -> first_child -> label == TAG_NUMNODE){
				fprintf(fp, "MOV AX, %s\n", op -> first_child -> first_child -> g -> tok -> tokenVal);
				fprintf(fp, "MOV [leftOp+%d], AX\n",  offset);

				writeArithmeticOrBoolean(fp, op -> first_child -> next_sibling, labelNo, offset + 2, parent, nodeType);

			}

			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
			fprintf(fp, "CMP AX, [rightOp]\n");
				
			if(op->label==TAG_GTNODE)
					fprintf(fp, "JG TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo); // greater than
			else if(op->label==TAG_LTNODE)
					fprintf(fp, "JL TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);  //less than
			else if(op->label==TAG_GENODE)
					fprintf(fp, "JGE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);  //greater than or equal too
			else if(op->label==TAG_LENODE)
					fprintf(fp, "JLE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);	 //less than or equal to
			else if(op->label==TAG_EQNODE)
					fprintf(fp, "JE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);   //equal to
			else if(op->label==TAG_NENODE)
					fprintf(fp, "JNE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);   //not equal
				

			if(op== op->parent->first_child)
			{
				fprintf(fp, "CONTINUE%d: MOV [leftOp+%d], AX\n",*labelNo,offset);
			}
			else 
				fprintf(fp, "CONTINUE%d: MOV [rightOp], AX\n",*labelNo);

			(*labelNo)++;

		}
		
		else{
			writeArithmeticOrBoolean(fp, op -> first_child, labelNo, offset, parent, nodeType);
			writeArithmeticOrBoolean(fp, op -> first_child->next_sibling, labelNo, offset+2, parent, nodeType);
			fprintf(fp, "MOV AX, [leftOp+%d]\n",  offset);
				fprintf(fp, "CMP AX, [rightOp]\n");
				
				if(op->label==TAG_GTNODE)
					fprintf(fp, "JG TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\nCONTINUE%d: MOV [rightOp], AX\n",*labelNo,*labelNo,*labelNo,*labelNo); // greater than
				else if(op->label==TAG_LTNODE)
					fprintf(fp, "JL TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);  //less than
				else if(op->label==TAG_GENODE)
					fprintf(fp, "JGE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);  //greater than or equal too
				else if(op->label==TAG_LENODE)
					fprintf(fp, "JLE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);	 //less than or equal to
				else if(op->label==TAG_EQNODE)
					fprintf(fp, "JE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);   //equal to
				else if(op->label==TAG_NENODE)
					fprintf(fp, "JNE TRUE%d\nMOV AX,0\nJMP CONTINUE%d\nTRUE%d: MOV AX,1\n",*labelNo,*labelNo,*labelNo);   //not equal
				

				if(op== op->parent->first_child)
				{
					fprintf(fp, "CONTINUE%d: MOV [leftOp+%d], AX\n",*labelNo,offset);
				}
				else 
					fprintf(fp, "CONTINUE%d: MOV [rightOp], AX\n",*labelNo);

				(*labelNo)++;
		}

	}
	

}   



void writeStmts(FILE* fp, astNode root, NODETYPE nodeType, p parent){

	while(root != NULL){
		if(root -> label == TAG_START || root -> label == TAG_END);
			//printf("Start or End\n");

		else if(root -> label == TAG_DECLARESTMT);
			//printf("Declare\n");

		else if (root -> label == TAG_GETVALUESTMT){
			

			IDNode id = findIdutil(root -> first_child -> g -> tok -> tokenVal, parent, nodeType, root -> first_child -> g -> tok -> rownum);

			if(id->isArray == 0){
				fprintf(fp,"call readVal\n");
				fprintf(fp, "MOV [%s%d_id], AX\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
			}
			else{
				int lo = id->lowBound;
				int hi = id->upBound;
				int x = hi-lo+1;
				for (int i = 0; i<x; i++){
					fprintf(fp,"call readVal\n");
					int calcval = i*2;
					fprintf(fp, "MOV [%s%d_id + %d], AX\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel,calcval);
				}
			}
		}

		else if ( root -> label == TAG_PRINTSTMT){
			astNode var = root->first_child;

			if(var -> first_child -> label == TAG_IDNODE){
				IDNode id = findIdutil(var -> first_child -> g -> tok -> tokenVal, parent, nodeType, var -> first_child -> g -> tok -> rownum);
				if(id -> isArray == false){
					//fprintf(fp, "XOR EAX, EAX\n");
					fprintf(fp, "MOV AX, [%s%d_id]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
					fprintf(fp,"call printVal\n");	

				}
				else{
					astNode index = var->first_child->next_sibling;
					if(index == NULL){ //print whole array case
						int lo = id->lowBound;
						int hi = id->upBound;
						int x = hi-lo+1;
						for (int i = 0; i<x; i++){
							//fprintf(fp,"call readVal\n");
							int calcval = i*2;
							fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel, calcval);
							fprintf(fp,"call printVal\n");							
						}
					}
					else{
						if(index->label == TAG_NUMNODE){
							int i = atoi(index->g->tok->tokenVal);
							i = i-id->lowBound;
							fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel, i*2 );
							fprintf(fp,"call printVal\n");	
						}
						else{
							//boundchecking so not doing
						}
					}
					
				}
			}

			else if(var -> first_child -> label == TAG_NUMNODE || var -> first_child -> label == TAG_RNUMNODE){
				//fprintf(fp, "XOR AX, AX\n");
				fprintf(fp, "MOV AX, %s\n",  var -> first_child -> g -> tok -> tokenVal);
				fprintf(fp,"call printVal\n");
			}

			else if(var -> first_child -> label == TAG_TRUENODE){
				//fprintf(fp, "XOR AX, AX\n");
				fprintf(fp, "MOV AX, 1\n");
				fprintf(fp,"call printVal\n");
			}

			else if(var -> first_child -> label == TAG_FALSENODE){
				//fprintf(fp, "XOR AX, AX\n");
				fprintf(fp, "MOV AX, 0\n");
				fprintf(fp,"call printVal\n");
			}
				
		}

		else if(root -> label == TAG_ASSIGNMENTSTMT){

			//printf(":=\n");

			bool isNeg = false;

			IDNode lhs = findIdutil(root -> first_child -> g -> tok -> tokenVal, parent, nodeType, root -> first_child -> g -> tok -> rownum);
			//printf("%s\n", lhs -> infoAboutNode -> tok -> tokenVal );

			if(lhs -> isArray == false){
				//printf("Not array\n");
				astNode op;
				if((root -> first_child -> next_sibling -> label == TAG_MINUSNODE || root -> first_child -> next_sibling -> label == TAG_PLUSNODE ) && root -> first_child -> next_sibling -> first_child -> next_sibling == NULL){
					// NT_U
					//printf("U\n");
					if(root -> first_child -> next_sibling -> label == TAG_MINUSNODE)
						isNeg = true;
					op = root -> first_child -> next_sibling -> first_child; 
				}
				else{
					//NT_arithmeticOrBooleanExpr
					//printf("AorB\n");
					op = root -> first_child -> next_sibling;
				}

				int labelNo = 0;
				if(op -> label == TAG_VAR){
					//Direct assignment
					if(op -> first_child -> label == TAG_IDNODE){
						//printf("id\n");
						//printf("%s\n",op -> first_child -> first_child -> g -> tok -> tokenVal );
						IDNode id = findIdutil(op -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> g -> tok -> rownum);
						if(id -> isArray == false){
							fprintf(fp, "MOV AX, [%s%d_id]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
							//fprintf(fp, "MOV [rightOp], AX\n");
						}
						else{
							if(op -> first_child -> next_sibling -> label == TAG_NUMNODE){
								int index_1 = ((atoi(op -> first_child -> next_sibling -> g -> tok -> tokenVal) - id -> lowBound) * 2);
								fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel, index_1);
							}
							else{
								IDNode indexID = findIdutil(op -> first_child-> next_sibling -> g -> tok -> tokenVal, parent, nodeType, op -> first_child-> next_sibling -> g -> tok -> rownum);
								fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
								fprintf(fp, "CMP AX, %d\n",id->lowBound);
								fprintf(fp, "JL EXIT_LABEL\n");
								fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
								fprintf(fp, "CMP AX, %d\n",id->upBound);
								fprintf(fp, "JG EXIT_LABEL\n");
								//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
								fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
								//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
								fprintf(fp, "SUB BL, %d\n", id->lowBound);
								fprintf(fp, "MOV AL, 2\n");
								fprintf(fp, "MUL BL\n");
								fprintf(fp, "MOV BX, AX\n");
								fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
							}
						}
					}
					else if(op -> first_child -> label == TAG_NUMNODE){
						fprintf(fp, "MOV AX, %s\n", op -> first_child -> g -> tok -> tokenVal);
						//fprintf(fp, "MOV [rightOp], AX\n");
					}
				}
				else if(op->label == TAG_TRUENODE){
					fprintf(fp, "MOV AX, %d\n", 1);
				}
				else if(op->label == TAG_FALSENODE){
					fprintf(fp, "MOV AX, %d\n", 0);
				}
				else
					writeArithmeticOrBoolean(fp, op, &labelNo, 0, parent, nodeType);

				if(isNeg){
					fprintf(fp, "MOV AX, -1\n");
					fprintf(fp, "IMUL word [temp]\n");
				}

				fprintf(fp, "MOV [%s%d_id], AX\n",  lhs -> infoAboutNode -> tok -> tokenVal, lhs -> nestingLevel);
			}

			
		
			else{
				if(root -> first_child -> next_sibling -> label == TAG_NUMNODE){

					int index = ((atoi(root -> first_child -> next_sibling -> g -> tok -> tokenVal) - lhs -> lowBound) * 2);
					astNode op;

					if((root -> first_child -> next_sibling -> next_sibling -> label == TAG_MINUSNODE || root -> first_child -> next_sibling -> next_sibling -> label == TAG_PLUSNODE ) && root -> first_child -> next_sibling -> next_sibling -> first_child -> next_sibling == NULL){
							// NT_U
						//printf("U\n");
						if(root -> first_child -> next_sibling -> next_sibling -> label == TAG_MINUSNODE)
							isNeg = true;
						op = root -> first_child -> next_sibling -> next_sibling -> first_child; 
					}
					else{
						//NT_arithmeticOrBooleanExpr
						//printf("AorB\n");
						op = root -> first_child -> next_sibling -> next_sibling;
					}
					//astNode op = root -> first_child -> next_sibling -> next_sibling;
					int labelNo = 0;

					if(op -> label == TAG_VAR){
						//Direct assignment
						if(op -> first_child -> label == TAG_IDNODE){
							//printf("id\n");
							//printf("%s\n",op -> first_child -> first_child -> g -> tok -> tokenVal );
							IDNode id = findIdutil(op -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> g -> tok -> rownum);
							if(id -> isArray == false){
								fprintf(fp, "MOV AX, [%s%d_id]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
								//fprintf(fp, "MOV [rightOp], AX\n");
							}
							else{
								if(op -> first_child -> next_sibling -> label == TAG_NUMNODE){
									int index_1 = ((atoi(op -> first_child -> next_sibling -> g -> tok -> tokenVal) - id -> lowBound) * 2);
									fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel, index_1);
								}
								else{
									IDNode indexID = findIdutil(op -> first_child-> next_sibling -> g -> tok -> tokenVal, parent, nodeType, op -> first_child-> next_sibling -> g -> tok -> rownum);
									fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "CMP AX, %d\n",id->lowBound);
									fprintf(fp, "JL EXIT_LABEL\n");
									fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "CMP AX, %d\n",id->upBound);
									fprintf(fp, "JG EXIT_LABEL\n");
									//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "SUB BL, %d\n", id->lowBound);
									fprintf(fp, "MOV AL, 2\n");
									fprintf(fp, "MUL BL\n");
									fprintf(fp, "MOV BX, AX\n");
									fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
								

								}
							}
						}
						else if(op -> first_child -> label == TAG_NUMNODE){
							fprintf(fp, "MOV AX, %s\n", op -> first_child -> g -> tok -> tokenVal);
							//fprintf(fp, "MOV [rightOp], AX\n");
						}
					}
					else if(op->label == TAG_TRUENODE){
						fprintf(fp, "MOV AX, %d\n", 1);
					}
					else if(op->label == TAG_FALSENODE){
						fprintf(fp, "MOV AX, %d\n", 0);
					}

					else
						writeArithmeticOrBoolean(fp, op, &labelNo, 0, parent, nodeType);

					if(isNeg){
						fprintf(fp, "MOV AX, -1\n");
						fprintf(fp, "IMUL word [temp]\n");
					}

					fprintf(fp, "MOV [%s%d_id + %d], AX\n",  lhs -> infoAboutNode -> tok -> tokenVal, lhs -> nestingLevel, index);
				}

				else{

					astNode op;// = root -> first_child -> next_sibling -> next_sibling;

					if((root->first_child->next_sibling->next_sibling->label == TAG_MINUSNODE || root->first_child->next_sibling->next_sibling->label == TAG_PLUSNODE ) && root -> first_child -> next_sibling -> next_sibling -> first_child -> next_sibling == NULL){
						// NT_U
						//printf("U\n");
						if(root -> first_child -> next_sibling -> next_sibling -> label == TAG_MINUSNODE)
							isNeg = true;
						op = root -> first_child -> next_sibling -> next_sibling -> first_child; 
					}
					else{
						//NT_arithmeticOrBooleanExpr
						//printf("AorB\n");
						op = root -> first_child -> next_sibling -> next_sibling;
					}

					int labelNo = 0;

					if(op -> label == TAG_VAR){
						//Direct assignment
						if(op -> first_child -> label == TAG_IDNODE){
							//printf("id\n");
							//printf("%s\n",op -> first_child -> first_child -> g -> tok -> tokenVal );
							IDNode id = findIdutil(op -> first_child -> g -> tok -> tokenVal, parent, nodeType, op -> first_child -> g -> tok -> rownum);
							if(id -> isArray == false){
								fprintf(fp, "MOV AX, [%s%d_id]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
								//fprintf(fp, "MOV [rightOp], AX\n");
							}
							else{
								if(op -> first_child -> next_sibling -> label == TAG_NUMNODE){
									int index_1 = ((atoi(op -> first_child -> next_sibling -> g -> tok -> tokenVal) - id -> lowBound) * 2);
									fprintf(fp, "MOV AX, [%s%d_id + %d]\n",  id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel, index_1);
								}
								else{
									IDNode indexID = findIdutil(op -> first_child-> next_sibling -> g -> tok -> tokenVal, parent, nodeType, op -> first_child-> next_sibling -> g -> tok -> rownum);
									fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "CMP AX, %d\n",id->lowBound);
									fprintf(fp, "JL EXIT_LABEL\n");
									fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "CMP AX, %d\n",id->upBound);
									fprintf(fp, "JG EXIT_LABEL\n");
									//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
									//fprintf(fp, "SUB BL, 1\n");
									fprintf(fp, "SUB BL, %d\n", id->lowBound);
									fprintf(fp, "MOV AL, 2\n");
									fprintf(fp, "MUL BL\n");
									fprintf(fp, "MOV BX, AX\n");
									fprintf(fp, "MOV AX, [%s%d_id+EBX]\n", id -> infoAboutNode -> tok -> tokenVal, id -> nestingLevel);
								
								}
							}
						}
						else if(op -> first_child -> label == TAG_NUMNODE){
							fprintf(fp, "MOV AX, %s\n", op -> first_child -> g -> tok -> tokenVal);
							//fprintf(fp, "MOV [rightOp], AX\n");
						}
					}
					else if(op->label == TAG_TRUENODE){
						fprintf(fp, "MOV AX, %d\n", 1);
					}
					else if(op->label == TAG_FALSENODE){
						fprintf(fp, "MOV AX, %d\n", 0);
					}
					else
						writeArithmeticOrBoolean(fp, op, &labelNo, 0, parent, nodeType);

					if(isNeg){
						fprintf(fp, "MOV AX, -1\n");
						fprintf(fp, "IMUL word [temp]\n");
					}

					fprintf(fp, "MOV [temp], AX\n");
					
					astNode index = root -> first_child -> next_sibling;
					IDNode indexID = findIdutil(index -> g -> tok -> tokenVal, parent, nodeType, index -> g -> tok -> rownum);
					
					fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
					fprintf(fp, "CMP AX, %d\n",lhs->lowBound);
					fprintf(fp, "JL EXIT_LABEL\n");
					fprintf(fp, "MOV AX,[%s%d_id]\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
					fprintf(fp, "CMP AX, %d\n",lhs ->upBound);
					fprintf(fp, "JG EXIT_LABEL\n");
					//fprintf(fp, "EXIT_%s%d: call exit\n",indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
					fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
					//fprintf(fp, "MOV BL, [%s%d_id]\n", indexID -> infoAboutNode -> tok -> tokenVal, indexID -> nestingLevel);
					fprintf(fp, "SUB BL, %d\n", lhs->lowBound);
					fprintf(fp, "MOV AL, 2\n");
					fprintf(fp, "MUL BL\n");
					fprintf(fp, "MOV BX, AX\n");
					fprintf(fp, "MOV AX, [temp]\n");
					fprintf(fp, "MOV [%s%d_id+EBX], AX \n", lhs -> infoAboutNode -> tok -> tokenVal, lhs -> nestingLevel);
				}
			}

		}

		else if(root->label == TAG_CONDITIONALSTMT){

			p conditional;
			if(nodeType == IS_STATEMENT){
				//printf("ifffff\n");
				conditional.stmt = (StmtNode) retrieve(root->g->str_name, hash(root->g->str_name), parent.stmt->localSymTable, IS_STATEMENT);

			}
			else {
				//printf("elseeee\n");
				conditional.stmt = (StmtNode) retrieve(root->g->str_name, hash(root->g->str_name), parent.func->localSymTable, IS_STATEMENT);
				//printf("elsee yesss\n");

			}

			astNode switch_var_node = root->first_child;
			astNode cases = root->first_child->next_sibling->next_sibling;
			astNode cases_next = cases->next_sibling;

			//char switch_var_name[200];
			//int switch_var_nesting_level;

			IDNode switch_var_idnode = (IDNode) findIdutil(switch_var_node->g->tok->tokenVal, parent, nodeType, switch_var_node->g->tok->rownum);

			//strcpy(switch_var_name, switch_var_node->g->tok->tokenVal);
			//switch_var_nesting_level = switch_var_idnode->nestingLevel;

			fprintf(fp,"MOV CX, [%s%d_id]\n", switch_var_node->g->tok->tokenVal, switch_var_idnode->nestingLevel);

			int switchstmt_num = switch_ctr;
			switch_ctr++;

			int casenum = 2;

			token casetoken;
			astNode case_match_node;
			if(cases->label == TAG_CASESTMT){
				case_match_node = cases->first_child;
				if(case_match_node->label == TAG_TRUENODE){
					fprintf(fp, "CMP CX, 1\n" );
				}
				else if(case_match_node->label == TAG_FALSENODE){
					fprintf(fp, "CMP CX, 0\n" );
				}
				else{
					int case_match_value = atoi(case_match_node->g->tok->tokenVal);
					fprintf(fp, "CMP CX, %d\n", case_match_value);

				}
			}
			while (cases->next_sibling != NULL && cases->next_sibling->label == TAG_CASESTMT){
				fprintf(fp, "JNZ CASE%d%d\n", switch_ctr, casenum);
				astNode casestmts = cases->first_child->next_sibling;

				writeStmts(fp, casestmts, IS_STATEMENT, conditional);

				fprintf(fp, "JMP END_SWITCH%d\n", switch_ctr);
				fprintf(fp, "CASE%d%d: ", switch_ctr, casenum++);

				cases = cases->next_sibling;

				astNode next_match_value = cases->first_child;


				if(next_match_value->label == TAG_TRUENODE){
					fprintf(fp, "CMP CX, 1\n" );
				}
				else if (next_match_value->label == TAG_FALSENODE){
					fprintf(fp, "CMP CX, 0\n" );
				}
				else{
					int case_match_value = atoi(next_match_value->g->tok->tokenVal);
					fprintf(fp, "CMP CX, %d\n", case_match_value);


				}

				
			}

			fprintf(fp, "JNZ DEFAULT%d\n", switch_ctr );
			//cases = cases->next_sibling; // last case
			astNode defaultstmt = cases->next_sibling;
			astNode casestmts = cases->first_child->next_sibling;
			writeStmts(fp, casestmts, IS_STATEMENT, conditional);
			fprintf(fp, "JMP END_SWITCH%d\n", switch_ctr);
			if(defaultstmt != NULL && defaultstmt->label == TAG_DEFAULTSTMT){
				fprintf(fp, "DEFAULT%d: ", switch_ctr);
				writeStmts(fp, defaultstmt->first_child, IS_STATEMENT, conditional);
			}
			else{
				fprintf(fp, "DEFAULT%d: ", switch_ctr);
			}
			fprintf(fp, "END_SWITCH%d: ", switch_ctr);
			//printf("Done with switch\n");
		}
		// else {
		// 	printf("else\n");
		// 	break;
		// }

		else if(root->label == TAG_FORSTMT){

			p iterative_for;
			if(nodeType == IS_STATEMENT){
				//printf("ifffff\n");
				iterative_for.stmt = (StmtNode) retrieve(root->g->str_name, hash(root->g->str_name), parent.stmt->localSymTable, IS_STATEMENT);

			}
			else {
				//printf("elseeee\n");
				iterative_for.stmt = (StmtNode) retrieve(root->g->str_name, hash(root->g->str_name), parent.func->localSymTable, IS_STATEMENT);
				//printf("elsee yesss\n");

			}

			int x = for_loop++;

			IDNode for_var = (IDNode) findIdutil(root -> first_child -> g -> tok -> tokenVal, parent,nodeType, root->first_child->g->tok->rownum);
			astNode l = root->first_child->next_sibling->first_child;
			astNode h = l->next_sibling;
			int low = atoi(l->g->tok->tokenVal);
			int high = atoi(h->g->tok->tokenVal);
			fprintf(fp, "MOV word [%s%d_id] , %d\n", for_var->infoAboutNode->tok->tokenVal,for_var->nestingLevel,low);
			fprintf(fp, "MOV CX, %d\n",high);
			fprintf(fp, "MOV [for_temp+%d], CX\n",x*2);
			fprintf(fp, "FOR%d: ",x);
			
			writeStmts(fp, root->first_child->next_sibling ,IS_STATEMENT, iterative_for);
			//next statement;
			//till statements over
			fprintf(fp, "INC word [%s%d_id]\n",for_var->infoAboutNode->tok->tokenVal,for_var->nestingLevel);
			fprintf(fp, "MOV CX,[for_temp+%d]\n",x*2);  //OFFSET?????
			fprintf(fp, "CMP CX, [%s%d_id]\n",for_var->infoAboutNode->tok->tokenVal,for_var->nestingLevel);
			fprintf(fp, "JGE FOR%d\n",x);

		}
		//end of for

		else if(root->label == TAG_WHILESTMT){

			p whilescope;
			if(nodeType == IS_STATEMENT){
				//printf("ifffff\n");
				whilescope.stmt = (StmtNode) retrieve(root->g->str_name, hash(root->g->str_name), parent.stmt->localSymTable, IS_STATEMENT);

			}
			else {
				//printf("elseeee\n");
				whilescope.stmt = (StmtNode) retrieve(root->g->str_name, hash(root->g->str_name), parent.func->localSymTable, IS_STATEMENT);
				//printf("elsee yesss\n");

			}

			int labelNo =0;

			astNode while_exp_root = root->first_child;
			astNode while_stmts = root->first_child->next_sibling;
			//printf("whileExp %s\n", tag_names[while_exp_root->label]);
			

			int while_num = while_ctr;
			while_ctr++;

			fprintf(fp, "WHILE%d:", while_num);

			//writeStmts(fp, while_exp_root, nodeType, parent);
			writeArithmeticOrBoolean(fp, while_exp_root, &labelNo, 0, parent, nodeType);

			printf("whilestmr %s\n", tag_names[while_stmts->label]);

			fprintf(fp, "MOV CX, AX\n");
			fprintf(fp, "CMP CX, 1\n");
			fprintf(fp, "JNZ END_WHILE%d\n", while_num);

			writeStmts(fp, while_stmts, IS_STATEMENT, whilescope);

			fprintf(fp, "JMP WHILE%d\n", while_num);
			fprintf(fp, "END_WHILE%d:", while_num);

		}



		root = root -> next_sibling;
	}

}

void callPrintVal(FILE* fp){
	fprintf(fp, "printVal:\n");
	fprintf(fp, "    push eax\n");
	fprintf(fp, "    push ebx\n");
	fprintf(fp, "    push ecx\n");
	fprintf(fp, "    push edx\n");
	fprintf(fp, "    push esi\n");
	fprintf(fp, "    movsx eax, ax\n");
	fprintf(fp, "    mov esi, myBuffer\n");
	fprintf(fp, "    add esi, myBufSize\n");
	fprintf(fp, "    mov ecx, 10\n");
	fprintf(fp, "    mov edx, eax\n");
	fprintf(fp, "    cmp eax, 0\n");
	fprintf(fp, "    jge .isNonNeg\n");
	fprintf(fp, "    mov eax, 0\n");
	fprintf(fp, "    sub eax, edx\n");
	// fprintf(fp, "    push 'O'\n");
	// fprintf(fp, "    mov ecx, esp\n");
	// fprintf(fp, "    mov edx, 1\n");
	// fprintf(fp, "    call printString\n");
	// fprintf(fp, "    push 'u'\n");
	// fprintf(fp, "    mov ecx, esp\n");
	// fprintf(fp, "    mov edx, 1\n");
	// fprintf(fp, "    call printString\n");
	// fprintf(fp, "    push 't'\n");
	// fprintf(fp, "    mov ecx, esp\n");
	// fprintf(fp, "    mov edx, 1\n");
	// fprintf(fp, "    call printString\n");
	// fprintf(fp, "    push 'p'\n");
	// fprintf(fp, "    mov ecx, esp\n");
	// fprintf(fp, "    mov edx, 1\n");
	// fprintf(fp, "    call printString\n");
	// fprintf(fp, "    push 'u'\n");
	// fprintf(fp, "    mov ecx, esp\n");
	// fprintf(fp, "    mov edx, 1\n");
	// fprintf(fp, "    call printString\n");
	// fprintf(fp, "    push 't'\n");
	// fprintf(fp, "    mov ecx, esp\n");
	// fprintf(fp, "    mov edx, 1\n");
	// fprintf(fp, "    call printString\n");
	fprintf(fp, "    push '-'\n");
	fprintf(fp, "    mov ecx, esp\n");
	fprintf(fp, "    mov edx, 1\n");
	fprintf(fp, "    call printString\n");
	fprintf(fp, "    add esp, 4\n");
	fprintf(fp, "    mov ecx, 10\n");

}
void callPrintString(FILE* fp){

	fprintf(fp, "printString:\n");

	fprintf(fp,"    push eax\n");
    fprintf(fp,"    push ebx\n");
    fprintf(fp,"    mov ebx, 1\n");
    fprintf(fp,"    mov eax, 4\n");
    fprintf(fp,"    int 80H\n");
    fprintf(fp,"    pop ebx\n");
    fprintf(fp,"    pop eax\n");
    fprintf(fp,"    ret\n");

}

void callIsNonNeg(FILE* fp){

	fprintf(fp, ".isNonNeg:\n");
    fprintf(fp, "    mov edx, 0\n");
    fprintf(fp, "    idiv ecx\n");
    fprintf(fp, "    add dl, 0x30\n");
    fprintf(fp, "    mov [esi], dl\n");
    fprintf(fp, "    dec esi\n");
    fprintf(fp, "    cmp eax, 0\n");
    fprintf(fp, "    jnz .isNonNeg\n");
    fprintf(fp, "    mov edx, myBuffer\n");
    fprintf(fp, "    add edx, myBufSize\n");
    fprintf(fp, "    mov ecx, esi\n");
    fprintf(fp, "    sub edx, ecx\n");
    fprintf(fp, "    inc ecx\n");
    fprintf(fp, "    call printString\n");
    fprintf(fp, "    push 10\n");
    fprintf(fp, "    mov ecx, esp\n");
    fprintf(fp, "    mov edx, 1\n");
    fprintf(fp, "    call printString\n");
    fprintf(fp, "    add esp, 4\n");
    fprintf(fp, "    pop esi\n");
    fprintf(fp, "    pop edx\n");
    fprintf(fp, "    pop ecx\n");
    fprintf(fp, "    pop ebx\n");
    fprintf(fp, "    pop eax\n");
    fprintf(fp, "    ret\n");

}

void callReadVal(FILE* fp){

	fprintf(fp, "readVal:\n");
	fprintf(fp, "    push ecx\n");
	fprintf(fp, "    push edx\n");
	fprintf(fp, "    push esi\n");
	fprintf(fp, "    mov ecx, myBuffer\n");
	fprintf(fp, "    mov edx, myBufSize\n");
	fprintf(fp, "    call readString\n");
	fprintf(fp, "    mov esi, eax\n");
	fprintf(fp, "    mov byte [myBuffer + esi], 0\n");
	fprintf(fp, "    mov eax, myBuffer\n");
	fprintf(fp, "    call atoi\n");
	fprintf(fp, "    pop esi\n");
	fprintf(fp, "    pop edx\n");
	fprintf(fp, "    pop ecx\n");
	fprintf(fp, "    ret\n");
}

void callReadString(FILE* fp){

	fprintf(fp, "readString:\n");
    fprintf(fp, "	push ebx\n");
    fprintf(fp, "	mov ebx, 0\n");
    fprintf(fp, "	mov eax, 3\n");
    fprintf(fp, "	int 80H\n");
    fprintf(fp, "	pop ebx\n");
    fprintf(fp, "	ret\n");

}

void callAtoi(FILE* fp){
	fprintf(fp, "atoi:\n");
    fprintf(fp, "	push ebx\n");
    fprintf(fp, "	push ecx\n");
    fprintf(fp, "	push edx\n");
    fprintf(fp, "	push esi\n");
    fprintf(fp, "	mov esi, eax\n");      
    fprintf(fp, "	mov eax, 0\n");       
    fprintf(fp, "	mov ecx, 0\n");       
    fprintf(fp, "	mov bl, [esi+ecx]\n");  
    fprintf(fp, "	cmp bl, '-'\n");          
    fprintf(fp, "	jne .mulLoop\n");
    fprintf(fp, "	inc ecx\n");

}

void callMulLoop(FILE* fp){

	fprintf(fp, ".mulLoop:\n");
    fprintf(fp, "mov ebx, 10\n");         
    fprintf(fp, "mul ebx\n");             
    fprintf(fp, "mov bl, [esi+ecx]\n");   
    fprintf(fp, "cmp bl, 48\n");          
    fprintf(fp, "jl .doneCode\n");       
    fprintf(fp, "cmp bl, 57\n");          
    fprintf(fp, "jg .doneCode\n");
    fprintf(fp, "sub bl, 48\n");          
    fprintf(fp, "add eax, ebx\n");        
    fprintf(fp, "inc ecx\n");             
    fprintf(fp, "jmp .mulLoop\n");

}

void callDoneCode(FILE* fp){
	fprintf(fp, ".doneCode:\n");
    fprintf(fp, "	mov ebx,10\n");
    fprintf(fp, "	div ebx\n");
    fprintf(fp, "	mov bl, [esi]\n");
    fprintf(fp, "	cmp bl,'-'\n");
    fprintf(fp, "	jne .fin\n");
    fprintf(fp, "	mov ebx,eax\n");
    fprintf(fp, "	mov eax,0\n");
    fprintf(fp, "	sub eax,ebx\n");
}

void callFin(FILE* fp){
	fprintf(fp, ".fin:\n");
    fprintf(fp, "	pop esi\n");             
    fprintf(fp, "	pop edx\n");
    fprintf(fp, "	pop ecx\n");             
    fprintf(fp, "	pop ebx\n");             
    fprintf(fp, "	ret\n");
}

void writeExit(FILE* fp){
	fprintf(fp, "exit:\n"); 
    fprintf(fp, "mov eax,1\n");
    fprintf(fp, "xor ebx,ebx\n");
    fprintf(fp, "int 80H\n");
}

void writeHelperFuncs(FILE* fp){

	callPrintString(fp);
	callReadString(fp);
	callPrintVal(fp);
	callIsNonNeg(fp);
	callReadVal(fp);
	callAtoi(fp);
	callMulLoop(fp);
	callDoneCode(fp);
	callFin(fp);
	writeExit(fp);
}

void writeMain(FILE* fp, astNode root){

	astNode stmts = root -> first_child;

	p parent;
	parent.func = driverFunc;

	writeStmts(fp, stmts -> first_child, IS_FUNCTION, parent);

	fprintf(fp, "call exit\n\n");
	fprintf(fp, "EXIT_LABEL: MOV AX, -1\n\n");
	fprintf(fp, "call printVal\n\n");
	fprintf(fp, "call exit\n\n");
	writeHelperFuncs(fp);
}

void doCodeGen(FILE* fp, astNode root){

	HashEntry* symTable = driverFunc -> localSymTable;

	writeData(fp, symTable);

	fprintf(fp,"\nSECTION .text\nglobal _start\n");
	fprintf(fp, "\n_start:\n");

	root = root -> first_child;

	if(root != NULL && root -> label == TAG_MODULEDEC)
	    root = root->next_sibling;
	    
	if( root != NULL && root -> label == TAG_OTHERMODULES)
	    root = root->next_sibling;

	//starting in driver
	writeMain(fp, root);

}