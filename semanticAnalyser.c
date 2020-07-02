//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#include "ast.h"
#include "symbolTable.h"
#include "semanticAnalyser.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

void checkSemantics(astNode ast_root, int *sem_type_errors, p scope, NODETYPE scopetype);

// int sem_type_errors = 0;

IDNode findIdutil(char* lexeme, p node, NODETYPE nodeType, int row){
	int hashVal = hash(lexeme);
	//printf(" %s %d\n",lexeme, hashVal);
	if(nodeType == IS_STATEMENT){
		 //printf("%s\n", node.stmt -> infoAboutNode -> str_name);
		IDNode id = (IDNode)retrieve(lexeme, hashVal, node.stmt -> localSymTable, IS_ID);
		if(id == NULL){
			//printf("ek\n");
			return findIdutil(lexeme, node.stmt->parent, node.stmt->parentType, row);
		}
		else{
			//printf("ek else\n");
			if(row < id->infoAboutNode->tok->rownum){
				//printf("do\n");
				return findIdutil(lexeme, node.stmt->parent, node.stmt->parentType, row);
			}
			else return id;
		}
		
			
	}
	else if(nodeType == IS_FUNCTION){

		//printf("%s\n", node.func-> infoAboutNode -> tok -> tokenVal);
		IDNode id = retrieve(lexeme, hashVal, node.func -> localSymTable, IS_ID);
		if(id == NULL){
			//printf("chaar\n");
			id = retrieveFromLL(lexeme, node.func -> inputPList);
			if(id == NULL)
				return NULL;
			else {
				//printf("chaar 4\n");
				if(row < id->infoAboutNode->tok->rownum){
					//printf("444\n");
					return NULL;
				}
				else return id;
				//printf("bahut chaar\n");
			}
		}
		else return id;

	}
	else 
		return NULL;
}

char* returnTypeName(int t){
	if(t==0) return "INTEGER";
	if(t==1) return "REAL";
	if(t==2) return "BOOLEAN";
	if(t==100) return "ARRAY OF INTEGER";
	if(t==101) return "ARRAY OF REAL";
	if(t==102) return "ARRAYOF BPPLEAN";
	else return "WRONG TYPE";
}

int whichType(astNode node, p pnode, NODETYPE nodeType, int* sem_type_errors){
	if(node == NULL) return -1;
	if (node->label == TAG_IDNODE){
		IDNode temp = findIdutil(node->g->tok->tokenVal, pnode, nodeType, node->g->tok->rownum);
		if(temp == NULL){
			printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", node->g->tok->rownum, node->g->tok->tokenVal);
			(*sem_type_errors)++;
			return -7;
		}
		return temp->type; //assuming it returns basic data type of the node in question. 
	}
	if(node->label == TAG_NUMNODE) return TYPE_INT;
	if(node->label == TAG_RNUMNODE) return TYPE_REAL;
	if(node->label == TAG_TRUENODE || node->label == TAG_FALSENODE) return TYPE_BOOL;
	else{
		printf("not passing a correct node\n");
		return -2;
	}
}

void checkArrayBounds(astNode root, p pnode, NODETYPE nodeType, int* sem_type_errors){
	astNode arrayName = root->first_child;
	astNode indexNode = arrayName->next_sibling;

	if (indexNode == NULL){
		//printf("np ondex only array\n");
		return;
	}

	//IDNode arrayidnode = findID(arrayName->g->tok->tokenVal, hash(arrayName->g->tok->tokenVal), pnode, nodeType);
	IDNode arrayidnode = findIdutil(arrayName->g->tok->tokenVal, pnode, nodeType, arrayName->g->tok->rownum);
	if(arrayidnode == NULL){
		printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", arrayName->g->tok->rownum, arrayName->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;	
	}
	//printf("heree\n");

	if(arrayidnode->isArray != 1) {
		//printf("in hereq\n");
		return;
	}
	if(arrayidnode->isStatic != 1){
		//bound checking dynamic nothing to be done
		return;
	}
	int lowerBound = arrayidnode->lowBound;
	int upperBound = arrayidnode->upBound;

	if (lowerBound == -1 || upperBound == -1){
		printf("something is wrong\n");
		return;
	}

	if(indexNode->label == TAG_NUMNODE){
		int ind = atoi(indexNode->g->tok->tokenVal);
		if (ind>upperBound || ind<lowerBound){
			(*sem_type_errors)++;
			printf("ERROR: LINE NO: %d: Array Index Out of Bounds\n", indexNode->g->tok->rownum);
			return;
		}
	}
	if(indexNode->label == TAG_IDNODE){
		IDNode indexid = findIdutil(indexNode->g->tok->tokenVal, pnode, nodeType, indexNode->g->tok->rownum );
		//IDNode indexid = findID(indexNode->g->tok->tokenVal, hash(indexNode->g->tok->tokenVal), pnode, nodeType);
		if(indexid == NULL){
			printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", indexNode->g->tok->rownum, indexNode->g->tok->tokenVal);
			(*sem_type_errors)++;
			return;
		}
		if(indexid->isArray == 1 ){
			printf("ERROR: LINE NO: %d: Array Index expected to be of type INTEGER but found of type ARRAY of %s\n", indexNode->g->tok->rownum, returnTypeName(indexid->type));
			(*sem_type_errors)++;
			return;
		}
		if(indexid->type != TYPE_INT){
			printf("ERROR: LINE NO: %d: Type Error. Array Index expected to be of type INTEGER but found of type %s\n",indexNode->g->tok->rownum, returnTypeName(indexid->type));
			(*sem_type_errors)++;
			return;
		}
		// if(isAssigned != 1){
		// 	printf("ERROR: LINE NO: %d: Array Index is not initialised before use\n", indexNode->g->tok->rownum);
		// 	(*sem_type_errors)++;
		// 	return;
		// }

	}
}

int calculateType(astNode rhsRoot, p pnode, NODETYPE nodeType, int* sem_type_errors){
	//printf("enter calculate type rhsroot = %s and rhsroot first_child = %s and value %s\n", tag_names[rhsRoot->label], tag_names[rhsRoot->first_child->label], rhsRoot->first_child->g->tok->tokenVal);
	if(rhsRoot == NULL){
		//printf("no rhs root in calculate type\n");
		return -10;
	}
		//printf("enter calculate type rhsroot = %s and rhsroot first_child = %s \n", tag_names[rhsRoot->label], tag_names[rhsRoot->first_child->label]);
    //printf("calculateType\n");

	//unary operators special case

	if(rhsRoot->label == TAG_PLUSNODE || rhsRoot->label == TAG_MINUSNODE){
		if(rhsRoot->first_child->next_sibling == NULL){ //unary case
			//printf("unary\n");
			int t = calculateType(rhsRoot->first_child, pnode, nodeType, sem_type_errors);
			//printf("in unary type: %s\n", returnTypeName(t));
			if(t == TYPE_BOOL){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: Incorrect type for operator %s\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -11;
			}
			else return t;
		}
	}

	//base case: var nodes
	if(rhsRoot->label == TAG_VAR){
		astNode temp = rhsRoot->first_child;
		//printf("base case: variable = %s\n", temp->g->tok->tokenVal);
		if(temp->label == TAG_IDNODE){ //in case id node
			int x = hash(temp->g->tok->tokenVal);
			//IDNode expidnode = findID(temp->g->tok->tokenVal, x, pnode, nodeType);
			IDNode expidnode = findIdutil(temp->g->tok->tokenVal, pnode, nodeType, temp->g->tok->rownum);
			if(expidnode == NULL){ //handle out of scope case
				printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", temp->g->tok->rownum, temp->g->tok->tokenVal);
				(*sem_type_errors)++;
				return -1;
			}
			if(expidnode->isArray){
				//printf("array hai check bounds me jao\n");
				checkArrayBounds(rhsRoot, pnode, nodeType, sem_type_errors); //if it is an array element than we have to check out of bounds
				// if(rhsRoot->first_child->next_sibling == NULL){
				// 	return -15;
				// }
				//printf("idhar\n");
			} 
			return expidnode->type;
		}
		else return whichType(temp, pnode, nodeType, sem_type_errors); //other than ids: this can be num, rnum or bool
	}

	if(rhsRoot->label == TAG_TRUENODE || rhsRoot->label == TAG_FALSENODE) return TYPE_BOOL;

	int t1 = calculateType(rhsRoot->first_child, pnode, nodeType, sem_type_errors);
	//if(t1<0) return t1;
	//printf("t1 of rhsroot %s is: %s\n",rhsRoot->g->tok->tokenVal, returnTypeName(t1));
	int t2 = calculateType(rhsRoot->first_child->next_sibling, pnode, nodeType, sem_type_errors);
	//if(t2<0) return t2;
	//printf("t2 of rhsRoot %s is: %s\n",rhsRoot->g->tok->tokenVal, returnTypeName(t2) );

	if(rhsRoot->label == TAG_PLUSNODE || rhsRoot->label == TAG_MINUSNODE || rhsRoot->label == TAG_MULNODE || rhsRoot->label == TAG_DIVNODE){
		if(t1 == t2 && (t1 == TYPE_INT || t1 == TYPE_REAL)){
			//printf("correct yayy 1\n");
			return t1; //correct type on both lhs and rhs of +-*/, reala or int allowed
		}
		else{
			if((t1>=0 && t1<=2 && t2<= 2 && t2>=0 && t1!=t2)){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: LHS and RHS type of operator %s do not match\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -9;
			}
			if(t1 == TYPE_BOOL || t2 == TYPE_BOOL){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: Incorrect type for operator %s\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -11;
			}

		}
	}

	if(rhsRoot->label == TAG_GTNODE || rhsRoot->label == TAG_GENODE || rhsRoot->label == TAG_LTNODE || rhsRoot->label == TAG_LENODE || rhsRoot->label == TAG_EQNODE || rhsRoot->label == TAG_NENODE){
		if(t1 == t2 && (t1 == TYPE_INT || t1 == TYPE_REAL)){
			//printf("correct yayy 2\n");
			return TYPE_BOOL; //correct type on both lhs and rhs of <><=>=!=== reala or int allowed
		}
		else{
			if((t1>=0 && t1<=2 && t2<= 2 && t2>=0 && t1!=t2) ){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: LHS and RHS type of operator %s do not match\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -9;
			}
			if(t1 == TYPE_BOOL || t2 == TYPE_BOOL){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: Incorrect type for operator %s\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -11;
			}

		}
	}

	if(rhsRoot->label == TAG_ANDNODE || rhsRoot->label == TAG_ORNODE){
		if(t1 == TYPE_BOOL && t2 == TYPE_BOOL ){
			//printf(" correct yayy 3\n");
			return t1;
		}
		else{
			if(t1>=0 && t1<=2 && t2<= 2 && t2>=0 && t1!=t2){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: LHS and RHS type of operator %s do not match\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -9;
			}
			if(t1 == TYPE_INT || t2 == TYPE_INT || t1 == TYPE_REAL || t2 == TYPE_REAL){
				(*sem_type_errors)++;
				printf("ERROR: LINE NO: %d: Incorrect type for operator %s\n", rhsRoot->g->tok->rownum, rhsRoot->g->tok->tokenVal);
				return -11;
			}

		}

	}

}

void check_assignment_type(astNode rootnode, p pnode, NODETYPE nodeType, int *sem_type_errors){
	//printf("enter check_assignment_type %s\n", tag_names[rootnode->label]);
	if(rootnode == NULL){
		//printf("null root in check_assignment_type\n");
		return;
	}
	astNode lhs = rootnode->first_child;
	//printf("lhs: %s and var name %s\n", tag_names[lhs->label], lhs->g->tok->tokenVal);

	// bool isArrayEle = false;
	// if(lhs && lhs->next_sibling && lhs->next_sibling->label == TAG_NUMNODE) isArrayEle = true;
	int x = hash(lhs->g->tok->tokenVal);
	//printf("hiii\n");
	//printf("hash value for lhs = %d\n", x);
	//IDNode lhsID = findID(lhs->g->tok->tokenVal, x, pnode, nodeType); //symboltbale node for id node recieved and out of scope error handled
	IDNode lhsID = findIdutil(lhs->g->tok->tokenVal, pnode, nodeType, lhs->g->tok->rownum); //symboltbale node for id node recieved and out of scope error handled

	if(lhsID == NULL){
		printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", lhs->g->tok->rownum, lhs->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;	
	}
		//printf("lhsidfound %s\n", lhsID->infoAboutNode->tok->tokenVal);


	lhsID->isAssigned = 1;

	//special assignment case in array assignment
	if(lhsID->isArray){
		//printf("//special assignment case in array assignment\n");
		astNode nextToarr =lhs->next_sibling;

		if(nextToarr->label == TAG_VAR){
			//array assignment case. basic data type and bounds have to be same
			int leftbasictype, rightbasictype, leftup, rightup, leftlow, rightlow;

			//IDNode rhsarrname = findID(nextToarr->first_child->g->tok->tokenVal, hash(nextToarr->first_child->g->tok->tokenVal), pnode, nodeType);
			IDNode rhsarrname = findIdutil(nextToarr->first_child->g->tok->tokenVal, pnode, nodeType, nextToarr->first_child->g->tok->rownum);

			if(rhsarrname == NULL){//rhs is there
				//lhsID->isAssigned = 0;
				printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", nextToarr->first_child->g->tok->rownum, nextToarr->first_child->g->tok->tokenVal);
				(*sem_type_errors)++;
				return;	
			}

			if(rhsarrname->isArray != 1){ // rhs id is also of array type
				//lhsID->isAssigned = 0;
				printf("ERROR: LINE NO: %d: Array variable can not be assigned to non array variable\n", nextToarr->first_child->g->tok->rownum);
				(*sem_type_errors)++;
				return;	
			}

			leftbasictype = lhsID->type;
			rightbasictype = rhsarrname->type;

			if(leftbasictype != rightbasictype){ //basic type is same
				//lhsID->isAssigned = 0;
				printf("ERROR: LINE NO: %d: Array Variables %s and %s are not of same basic type\n", nextToarr->first_child->g->tok->rownum, lhs->g->tok->tokenVal, nextToarr->first_child->g->tok->tokenVal );
				(*sem_type_errors)++;
				return;
			}

			leftlow = lhsID->lowBound;
			rightlow = rhsarrname->lowBound;
			leftup = lhsID->upBound;
			rightup = rhsarrname->upBound;

			if((leftlow != rightlow) || (leftup != rightup)){
				//lhsID->isAssigned = 0;
				printf("ERROR: LINE NO: %d: Array Variables %s and %s do not have same bounds\n", nextToarr->first_child->g->tok->rownum, lhs->g->tok->tokenVal, nextToarr->first_child->g->tok->tokenVal );
				(*sem_type_errors)++;
				return;	
			}

		}
		else if(nextToarr->label == TAG_NUMNODE || nextToarr->label == TAG_IDNODE){
			//array element being assigned. need to check bounds
			//int prev = *sem_type_errors;
			checkArrayBounds(rootnode, pnode, nodeType, sem_type_errors);
			//if(prev< *sem_type_errors) lhsID->isAssigned = 0;
		}
		else{//any other operator used directly with array elements
			//lhsID->isAssigned = 0;
			printf("ERROR: LINE NO: %d: Operator %s cannot be used with ARRAY type variables\n", nextToarr->g->tok->rownum, nextToarr->g->tok->tokenVal );
			(*sem_type_errors)++;
			return;
		}
	}




	int lhsType = lhsID->type;

	//printf("lhsType = %s\n", returnTypeName(lhsType));

	astNode rhs = lhs->next_sibling;
	//printf("rhs: %s\n", tag_names[rhs->label]);
	if(lhsID->isArray) {
		//printf("arrayyyyyyyy\n");
		rhs = rhs->next_sibling;
	}

	int prev = *sem_type_errors;

	int rhsType = calculateType(rhs, pnode, nodeType, sem_type_errors);

	//if(prev< *sem_type_errors) lhsID->isAssigned = 0;

	//printf("rhsType = %s\n", returnTypeName(rhsType));
	int t1 = lhsType;
	int t2 = rhsType;

	if(((t1>=0 && t2>=0) && t1<=2 && t2<=2 && t1!=t2)){
		//lhsID->isAssigned = 0;
		(*sem_type_errors)++;
		printf("ERROR: LINE NO: %d: TYPE ERROR. Expected type %s on RHS got %s\n", lhs->g->tok->rownum, returnTypeName(lhsType), returnTypeName(rhsType));
		return;
	}

	return;

}

void checkAllOuputParametersAssigned(LinkedList outputPList, int rown, int* sem_type_errors){
	LinkedList node = outputPList;
	while(node != NULL){
		if (node->parameter->isAssigned == 0){
			(*sem_type_errors)++;
			printf("ERROR: LINE NO - %d: Ouput Paramter %s not assigned before return from function\n", rown, node->parameter->infoAboutNode->tok->tokenVal);
		}
		node = node->next;
	}
	return;
}

void check_for_semantics(astNode root, int *sem_type_errors, p forparent, NODETYPE fp, p forscope, NODETYPE fs){
	astNode var = root->first_child;
	int vartype = whichType(var, forparent, fp, sem_type_errors);
	if(vartype != TYPE_INT){
		printf("ERROR: LINE NO: %d: Incompatible type of Vaiable %s in FOR Statement\n", var->g->tok->rownum, var->g->tok->tokenVal);
		(*sem_type_errors)++;
	}
	astNode stmtnode = var->next_sibling->next_sibling->next_sibling;
	checkSemantics(stmtnode,sem_type_errors,forscope,fs);

	//int ctr = 1;
	// while(stmtnode->label!= TAG_END){
	// 	printf("%d stmt inside for\n", ctr);
	// 	checkSemantics(stmtnode, sem_type_errors, forscope, fs);
	// 	printf("checked %d inside for\n",ctr );
	// 	ctr++;
	// 	stmtnode = stmtnode->next_sibling;
	// }
	return;
}

whileList createWhileListNode(astNode node){
	whileList newNode = (whileList) malloc(sizeof(struct whileVar));
	newNode->ast_node = node;
	newNode->hasChanged = 0;
	newNode->next_var = NULL;
	return newNode;
}

// whileList createWhileList(astNode root, whileList head){
// 	if(root == NULL) return head;
// 	if(root->label == TAG_VAR){
// 		if (head == NULL){
// 			head = createWhileListNode(root->first_child);
// 			return head;
// 		}
// 		else{
// 			whileList ptr = head;
// 			while (ptr->next_var != NULL) ptr = ptr->next_var;
// 			ptr->next_var = createWhileListNode(root->first_child);
// 			return head;
// 		}

// 	}
// 	else{
// 		head = createWhileList(root->first_child, head);
// 		head = createWhileList(root->next_sibling, head);
// 	}
// 	return head;
// }

// int check_while_itr_change(astNode stmtnode, whileList itrListHead){
// 	if (itrListHead == NULL) return 1;
// 	if (stmtnode == NULL) return 0;
// 	return 1;
// }

void check_while_semantics(astNode root, int *sem_type_errors, p wparent, NODETYPE wp, p wscope, NODETYPE ws){

	astNode exp = root->first_child;

	// whileList itrListHead = NULL;
	// itrListHead = createWhileList(exp, itrListHead);



	int exptype = calculateType(exp, wparent, wp, sem_type_errors);
	if(exptype != TYPE_BOOL){
		printf("ERROR: LINE NO: %d: Incompatible type of expression in WHILE Statement\n", exp->g->tok->rownum);
		(*sem_type_errors)++;
	}
	astNode stmtnode = exp->next_sibling;
	//int change = check_while_itr_change(stmtnode, itrListHead);

	//astNode endnode = last_child(stmtnode);
	checkSemantics(stmtnode, sem_type_errors, wscope, ws);

	// if(change == 0{
	// 	printf("ERROR: LINE NO: %d: None of the variables in while condition are changing in loop\n", endnode->g->tok->rownum);
	// 	(*sem_type_errors)++;
	// }

	// while(stmtnode!= NULL){
	// 	checkSemantics(stmtnode, sem_type_errors, wscope, ws);
	// 	stmtnode = stmtnode->next_sibling;
	// }
	return;
}
void check_conditional_semantics(astNode root, int *sem_type_errors, p cparent, NODETYPE cp, p cscope, NODETYPE cs){
	astNode var = root->first_child;
	//IDNode varid = findID(var->g->tok->tokenVal, hash(var->g->tok->tokenVal), cparent, cp);
	IDNode varid = findIdutil(var->g->tok->tokenVal, cparent, cp, var->g->tok->rownum);

	if(varid == NULL){
		printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", var->g->tok->rownum, var->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;
	}

	int typeofvar = varid->type;

	if(varid->type != TYPE_BOOL && varid->type != TYPE_INT){
		printf("ERROR: LINE NO: %d: TYPE of variable %s in Incompatible for conditional statement\n", var->g->tok->rownum, var->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;	
	}

	astNode cases = root->first_child->next_sibling->next_sibling;
	astNode prev = NULL;
	while(cases != NULL && cases->label == TAG_CASESTMT){
		astNode tag = cases->first_child;
		astNode casestmt = tag->next_sibling;
		int tagtype = whichType(tag, cparent, cp, sem_type_errors);
		if(tagtype != typeofvar){
			printf("ERROR: LINE NO: %d: Incompatible type of case tag variable %s\n", tag->g->tok->rownum, tag->g->tok->tokenVal );
			(*sem_type_errors)++;
		}
		checkSemantics(casestmt, sem_type_errors, cscope, cs);
		prev = cases;
		cases = cases->next_sibling;
	}

	if(cases->label == TAG_DEFAULTSTMT){//default stmt is existing
		//printf("%s\n", tag_names[cases->label] );
		if(typeofvar == TYPE_BOOL){//default stmt with bool is not allowed
			printf("ERROR: LINE NO: %d: Default Statement is not allowed with TYPE_BOOL in conditional statement\n", cases->g->tok->rownum );
			(*sem_type_errors)++;
		}
		else {
			checkSemantics(cases->first_child, sem_type_errors, cscope, cs);
		}
	}
	else{//default statement not there
		if(typeofvar == TYPE_INT){
			printf("ERROR: LINE NO: %d: Default Statement Missing\n", cases->g->tok->rownum);
			(*sem_type_errors)++;
		}
	}

	return; 
}

void check_get_value_semantics(astNode node, int *sem_type_errors, p scope, NODETYPE scopetype){
	if(node == NULL) return;
	astNode var = node->first_child;
	astNode index;
	index = var->next_sibling;
	IDNode varnode = findIdutil(var->g->tok->tokenVal, scope, scopetype, var->g->tok->rownum);
	//IDNode varnode = findID(var->g->tok->tokenVal, hash(var->g->tok->tokenVal), scope, scopetype);
	if(varnode == NULL){
		printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", var->g->tok->rownum, var->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;
	}
	varnode->isAssigned = 1;

	if(index != NULL && varnode->isArray == 1){
		//int prev = *sem_type_errors;
		checkArrayBounds(node, scope, scopetype, sem_type_errors);
		//if(prev< *sem_type_errors) varnode->isAssigned = 0;
	}
	if (index != NULL && varnode->isArray == 0){
		//varnode->isAssigned = 0;
		printf("ERROR: LINE NO: %d Variable %s is not of type array and cannot be indexed\n",var->g->tok->rownum, var->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;	
	}
	
	return;
}


void check_print_semantics(astNode ast_root, int *sem_type_errors, p scope, NODETYPE scopetype){
	 astNode print_var = ast_root->first_child;
	 if (print_var!=NULL && print_var->label == TAG_VAR){
	 	if(print_var->first_child->label == TAG_IDNODE){
	 		IDNode varnode = findIdutil(print_var->first_child->g->tok->tokenVal, scope, scopetype, print_var->first_child->g->tok->rownum);
	 		//can only print something that exists
	 		//IDNode varnode = findID(print_var->first_child->g->tok->tokenVal, hash(print_var->first_child->g->tok->tokenVal), scope, scopetype);
			if(varnode == NULL){
				printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", print_var->first_child->g->tok->rownum, print_var->first_child->g->tok->tokenVal);
				(*sem_type_errors)++;
				return;
			}
	 	}
	 	if (print_var->first_child->label == TAG_IDNODE && print_var->first_child->next_sibling != NULL){
	 		checkArrayBounds(print_var, scope, scopetype, sem_type_errors);
	 	}
	 }
}

void check_module_reuse_semantics(astNode root, int *sem_type_errors, p scope, NODETYPE scopetype){
	astNode outputHead, moduleName, inputHead;
	outputHead = NULL;
	moduleName = NULL;
	inputHead = NULL;
	outputHead = root->first_child->first_child;
	moduleName = root->first_child->next_sibling;
	inputHead = moduleName->next_sibling;
	
	FuncNode func = (FuncNode) retrieve(moduleName->g->tok->tokenVal, hash(moduleName->g->tok->tokenVal), globalSymTable, IS_FUNCTION); //retrieve symbol table of the function
	if (func == NULL){
		printf("ERROR: LINE NO: %d: Could not find module %s\n", moduleName->g->tok->rownum, moduleName->g->tok->tokenVal);
		(*sem_type_errors)++;
		return;
	}

	LinkedList it = func->outputPList;

	// if(((it == NULL || it->parameter == NULL) && outputHead!=NULL) || ((it != NULL && it->parameter != NULL) && outputHead ==NULL)){
	// 	printf("ERROR: LINE NO: %d: Incorrect Number of Output Paramters for module %s\n", moduleName->g->tok->rownum, moduleName->g->tok->tokenVal);
	// 	(*sem_type_errors)++;
	// }
	// else{
		//check matching types of out parameters
		while(it != NULL && outputHead != NULL){
		
			IDNode curr = (IDNode) findIdutil(outputHead->g->tok->tokenVal, scope, scopetype, outputHead->g->tok->rownum);
			//IDNode curr = (IDNode) findID(outputHead->g->tok->tokenVal, hash(outputHead->g->tok->tokenVal), scope, scopetype);
			if(curr == NULL){
				printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", outputHead->g->tok->rownum, outputHead->g->tok->tokenVal);
				(*sem_type_errors)++;
				it = it->next;
				outputHead = outputHead->next_sibling;
				continue;
			}
			if(curr->type != it->parameter->type){
				printf("ERROR: LINE NO: %d: Type %s of output paramter %s does not match with expected type of %s\n", outputHead->g->tok->rownum, returnTypeName(curr->type), outputHead->g->tok->tokenVal, returnTypeName(it->parameter->type));
				(*sem_type_errors)++;
				it = it->next;
				outputHead = outputHead->next_sibling;
				continue;
			}
			if(curr->isArray == 1){
				if(it->parameter->isArray == 0){
					printf("ERROR: LINE NO: %d: Type %s of input paramter %s does not match with expected type of %s\n", outputHead->g->tok->rownum, returnTypeName(curr->type), outputHead->g->tok->tokenVal, returnTypeName(it->parameter->type));
					(*sem_type_errors)++;
					it = it->next;
					outputHead = outputHead->next_sibling;
					continue;
				}
				else{
					if(curr->isStatic == 1 && it->parameter->isStatic == 1){
						if((curr->lowBound != it->parameter->lowBound) || (curr->upBound != it->parameter->upBound) ){
							printf("ERROR: LINE NO: %d: Bounds of input paramter %s does not match with expected bounds\n", outputHead->g->tok->rownum,  outputHead->g->tok->tokenVal);
							(*sem_type_errors)++;
							it = it->next;
							outputHead = outputHead->next_sibling;
							continue;
						}
					}
				}
			}
			it = it->next;
			outputHead = outputHead->next_sibling;
		}
		if(it != NULL || outputHead != NULL){
			printf("ERROR: LINE NO: %d: Incorrect Number of Output Paramters for module %s\n", moduleName->g->tok->rownum, moduleName->g->tok->tokenVal);
			(*sem_type_errors)++;
		}

	//}

	
	it = func->inputPList;


	//check matching types of out parameters
	while(it != NULL && inputHead != NULL){
		//IDNode curr = (IDNode) findID(inputHead->g->tok->tokenVal, hash(inputHead->g->tok->tokenVal), scope, scopetype);
		IDNode curr = (IDNode) findIdutil(inputHead->g->tok->tokenVal, scope, scopetype, inputHead->g->tok->rownum);
		if(curr == NULL){
			printf("ERROR: LINE NO: %d: Variable %s not declared in scope\n", inputHead->g->tok->rownum, inputHead->g->tok->tokenVal);
			(*sem_type_errors)++;
			it = it->next;
			inputHead = inputHead->next_sibling;
			continue;
		}
		if(curr->type != it->parameter->type){
			printf("ERROR: LINE NO: %d: Type %s of input paramter %s does not match with expected type of %s\n", inputHead->g->tok->rownum, returnTypeName(curr->type), inputHead->g->tok->tokenVal, returnTypeName(it->parameter->type));
			(*sem_type_errors)++;
			it = it->next;
			inputHead = inputHead->next_sibling;
			continue;
		}
		if(curr->isArray == 1){
			if(it->parameter->isArray == 0){
				printf("ERROR: LINE NO: %d: Type %s of input paramter %s does not match with expected type of %s\n", inputHead->g->tok->rownum, returnTypeName(curr->type), inputHead->g->tok->tokenVal, returnTypeName(it->parameter->type));
				(*sem_type_errors)++;
				it = it->next;
				inputHead = inputHead->next_sibling;
				continue;
			}
			else{
				if(curr->isStatic == 1 && it->parameter->isStatic == 1){
					if((curr->lowBound != it->parameter->lowBound) || (curr->upBound != it->parameter->upBound) ){
						printf("ERROR: LINE NO: %d: Bounds of input paramter %s does not match with expected bounds\n", inputHead->g->tok->rownum, inputHead->g->tok->tokenVal);
						(*sem_type_errors)++;
						it = it->next;
						inputHead = inputHead->next_sibling;
						continue;
					}
				}
			}
		}
		it = it->next;
		inputHead = inputHead->next_sibling;
	}

	if(it != NULL || inputHead != NULL){
		printf("ERROR: LINE NO: %d: Incorrect Number of Input Paramters for module %s\n", moduleName->g->tok->rownum, moduleName->g->tok->tokenVal);
		(*sem_type_errors)++;
	}

	return;

}

void checkSemantics(astNode ast_root, int *sem_type_errors, p scope, NODETYPE scopetype){
	if (ast_root == NULL) {
		//printf("NULL case in checkSemantics\n");
		return;
	}
	astLabel statement = ast_root->label;
	//printf("enter checkSemantics with ast_root %s \n", tag_names[statement]);
	switch(statement){
		case 0: //TAG_PROGRAM: Nothing to do, simply go to checking for children				
				checkSemantics(ast_root->first_child, sem_type_errors, scope, scopetype); // goes to module declarations
				break;
		case 1: //TAG_MODULEDEC: No semantic checks required other than redeclaration (redclaration already done in previous pass)
				checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				break;
		case 2: ;//TAG_MODULE: retrieve local symbol table for the module. nothing to do with input parameters. 
				//nothing to do with output parameters in this pass. type checking for statements in module def to be done now

				//DONE in this pass only//isAssigned wala 

				astNode module_it = ast_root->first_child; //module_it is at idnode(name of function)
				p fn;
				fn.func = (FuncNode) retrieve(module_it->g->tok->tokenVal, hash(module_it->g->tok->tokenVal), globalSymTable, IS_FUNCTION); //retrieve symbol table of the function
				if (fn.func == NULL){
					printf("func not found\n");
				}
				astNode tagend, moduledef;
				module_it = module_it->next_sibling;
				while(module_it!= NULL && module_it->label == TAG_INPUTPARAMETER) module_it = module_it->next_sibling; // skip all input params
				while(module_it!= NULL && module_it->label == TAG_OUTPUTPARAMETER) module_it = module_it->next_sibling; // skip all output params
				//now module it must be at module def node
				if(module_it!= NULL && module_it->label == TAG_MODULEDEF){
					moduledef = module_it->first_child->next_sibling;
					checkSemantics(moduledef, sem_type_errors, fn, IS_FUNCTION);
				}
				tagend = last_child(moduledef);
				
				checkAllOuputParametersAssigned(fn.func->outputPList, tagend->g->tok->rownum, sem_type_errors);  //needs to be corrected
				
				checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype); //check semantics for the next module, cycle will break at NULL
				break;
		case 3: ;//TAG_DRIVER_MODULE **very similar to stuff done above
				//FuncNode driverST = (FuncNode) retrieve(ast_root->first_child->g->str_name, hash(ast_root->first_child->g->str_name), globalSymTable, IS_FUNCTION); // have to check the retrieve driver ST. ** TOBE_DONE: SHEF
				p driverST;
				driverST.func = driverFunc;
				if(driverST.func == NULL){
					printf("gadbad. driver ST not found\n");
					return;
				}
				//first retrieved symbol table for driver than go to its module def to check all semantics
				astNode driver_root = ast_root->first_child;
				if(driver_root !=NULL && driver_root->label == TAG_MODULEDEF){
					astNode driver_moduledef = driver_root->first_child->next_sibling;
					checkSemantics(driver_moduledef, sem_type_errors, (p)driverST, IS_FUNCTION);
				}
				//will finish here when all statements of driver module are done
				checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);				
				break;
		case 4: //TAG_OTHERMODULES
				checkSemantics(ast_root->first_child, sem_type_errors, scope, scopetype); //this will end after all modules are checked
				checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype); // this will lead to driver module in the first instance and null in the second
				break;
		case 5: //TAG_INPUTPARAMETER
				break;
		case 6: //TAG_OUTPUTPARAMTER
				break;
		case 7: //TAG_DATATYPE
				break;
		case 8: //TAG_ARRAY
				break;
		case 9: //TAG_RANGEARRAY
				break;
		case 10: //TAG_GETVALUESTMT
				 check_get_value_semantics(ast_root, sem_type_errors, scope, scopetype);
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		case 11: ;//TAG_PRINTSTMT bounds have to be checked in case of array
				 check_print_semantics(ast_root, sem_type_errors, scope, scopetype);
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		case 12: //TAG_ASSIGNMENTSTMT
				 //printf("heyya case 12 chek semantics\n");
				 check_assignment_type(ast_root, scope, scopetype, sem_type_errors); 
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		case 13: //TAG_MODULEREUSESTMT // TOBE_DONE
				 check_module_reuse_semantics(ast_root, sem_type_errors, scope, scopetype);
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		case 14: //TAG_DECLARESTMT
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		case 15: ;// TAG_CONDITIONALSTMT - switch case
				 p conditional;
				 if(scopetype == IS_STATEMENT){
				 	//printf("ifffff\n");
				  	conditional.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.stmt->localSymTable, IS_STATEMENT);

				 }
				 else {
				 	//printf("elseeee\n");
				 	conditional.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.func->localSymTable, IS_STATEMENT);
				 	//printf("elsee yesss\n");

				 }
				 //conditional.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.stmt->localSymTable, IS_STATEMENT);
				 check_conditional_semantics(ast_root, sem_type_errors, scope, scopetype, conditional, IS_STATEMENT);
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
		case 16: break;
		case 17: ;//TAG_FORSTMT
				 p forstmt;
				 //printf("%d scopetype\n", scopetype);
				 if(scopetype == IS_STATEMENT){
				 	//printf("ifffff\n");
				  	forstmt.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.stmt->localSymTable, IS_STATEMENT);

				 }
				 else {
				 	//printf("elseeee\n");
				 	forstmt.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.func->localSymTable, IS_STATEMENT);
				 	//printf("elsee yesss\n");

				 }
				 check_for_semantics(ast_root, sem_type_errors, scope, scopetype, forstmt, IS_STATEMENT);
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		case 18: ;//TAG_WHILESTMT
				 p wstmt;
				 if(scopetype == IS_STATEMENT){
				 	//printf("ifffff\n");
				  	wstmt.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.stmt->localSymTable, IS_STATEMENT);

				 }
				 else {
				 	//printf("elseeee\n");
				 	wstmt.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.func->localSymTable, IS_STATEMENT);
				 	//printf("elsee yesss\n");

				 }
				 //wstmt.stmt = (StmtNode) retrieve(ast_root->g->str_name, hash(ast_root->g->str_name), scope.stmt->localSymTable, IS_STATEMENT);
				 check_while_semantics(ast_root, sem_type_errors, scope, scopetype, wstmt, IS_STATEMENT);
				 checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
		default: checkSemantics(ast_root->next_sibling, sem_type_errors, scope, scopetype);
				 break;
	}
}