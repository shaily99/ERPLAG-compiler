//Group-15
//Dishita Malav 2017A7PS0164P
//Shaily Bhatt 2017A7PS0040P
//Shefali Tripathi 2017A7PS0139P


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "ast.h"

int AST_SIZE = 0;

ptnode find_child(ptnode proot, int key, int index){
	int ctr = 0;
	proot = proot->first_child;
	while(proot != NULL){
		if (proot->g->name == key) ctr ++;
		if (proot->g->name == key && ctr == index) break;
		proot = proot->next_sibling;
	}
	return proot;
	
}

astNode last_child(astNode parent){
	astNode temp = parent;
	if (temp == NULL) return NULL;
	while(temp->next_sibling!=NULL){
		temp = temp->next_sibling;
	} 
	return temp;
}

astNode createNewASTnode(astNode temp, astLabel l, astNode p, astNode fc, astNode ns, grammar_block gb){
	// printf("enter createNewASTnode %s\n", tag_names[l]);
	// if (p!=NULL)
	// {
	// 	printf("parent %s \n", tag_names[p->label]);	
	// }
	if (temp == NULL){
		//printf("enter if\n");
		astNode node = (struct astStruct*)malloc(sizeof(struct astStruct));

		node->label = l;
		node->parent = p;
		node->first_child = fc;
		node->next_sibling = ns;
		node->g = gb;
		node->table = NULL;
		AST_SIZE++;
		//printf("%d\n", AST_SIZE);
		return node;

	}
	else return NULL;
}


// void handle_inh(astNode* node, astNode* ptr1, astNode* ptr2){
// 	printf("handle_inh\n");
// 	printf("node = ");
// 	if (*node) printf("%s\n", tag_names[(*node)->label] );
// 	else printf("NULL\n");

// 	printf("ptr1 = ");
// 	if (*ptr1) printf("%s\n", tag_names[(*ptr1)->label] );
// 	else printf("NULL\n");
// 	if(*ptr1!=NULL)
// 	{
// 		*ptr2 = (*ptr1)->first_child;
// 		(*ptr1)->first_child = *node;
// 		(*ptr1)->first_child->next_sibling=*ptr2;
// 		(*ptr1)->first_child->next_sibling->parent = *ptr1;
// 		while((*ptr1)->parent!=NULL){
// 			(*ptr1) = (*ptr1)->parent;
// 		}
// 		*node = *ptr1;
// 	}
// }

// void handle_inh_2(astNode* node, astNode* ptr1, astNode* ptr2){
// 	*ptr2 = (*ptr1)->first_child;
// 	(*node)->parent = *ptr1;
// 	(*ptr1)->first_child = *node;
// 	(*ptr2)->parent = *ptr1;
// 	(*ptr1)->first_child->next_sibling = *ptr2;
// 	*node = (*ptr1)->first_child;
// }


astNode make_ast(ptnode parseTreeRoot, astNode parent){
 	if (parseTreeRoot == NULL) {
 		//printf("NULL case\n");
 		return NULL;
 	}
	int rule_num = parseTreeRoot->rule_no;
	//printf("enter make_ast %s and rule num %d \n", names_ntandt[parseTreeRoot->g->name], rule_num);

	astNode node, ptr1, ptr2, nextNode, templeaf;
	ptnode child;
	switch(rule_num){
		case 0: node = createNewASTnode(NULL, TAG_PROGRAM, parent, NULL, NULL, parseTreeRoot->g);
				templeaf = createNewASTnode(NULL, TAG_MODULEDEC, node, NULL, NULL, (find_child(parseTreeRoot, NT_moduleDeclarations,1))->g);
				node->first_child = templeaf;
				child = find_child(parseTreeRoot, NT_moduleDeclarations, 1);
				templeaf->first_child = make_ast(child,templeaf); //mds
				nextNode = node->first_child;
				templeaf = createNewASTnode(NULL, TAG_OTHERMODULES, node, NULL, NULL, (find_child(parseTreeRoot, NT_otherModules,1))->g);
				nextNode->next_sibling = templeaf;
				templeaf->first_child = make_ast(find_child(parseTreeRoot, NT_otherModules,1), templeaf);
				nextNode = last_child(node->first_child);
				nextNode->next_sibling = make_ast(find_child(parseTreeRoot,NT_driverModule,1), node);
				nextNode = last_child(node->first_child);
				templeaf = createNewASTnode(NULL, TAG_OTHERMODULES, node, NULL, NULL, (find_child(parseTreeRoot, NT_otherModules,1))->g);
				nextNode->next_sibling = templeaf;
				templeaf->first_child = make_ast(find_child(parseTreeRoot, NT_otherModules,2), templeaf);
				break;

		case 1: node = make_ast(find_child(parseTreeRoot, NT_moduleDeclaration,1), parent);
				node->next_sibling = make_ast(find_child(parseTreeRoot, NT_moduleDeclarations,1), parent);
				break;
		case 2: node = NULL;
				break;
		case 3: child = find_child(parseTreeRoot, T_ID, 1);
				node = createNewASTnode(NULL, TAG_IDNODE, parent, NULL, NULL, child->g);
				break;
		case 4: node = make_ast(find_child(parseTreeRoot,NT_module,1), parent);
				node->next_sibling = make_ast(find_child(parseTreeRoot, NT_otherModules,1), parent);
				break;
		case 5: node = NULL; 
				break;
		case 6: node = createNewASTnode(NULL, TAG_DRIVERMODULE, parent, NULL, NULL, parseTreeRoot->g);
				node->first_child = make_ast(find_child(parseTreeRoot, NT_moduleDef,1),node);
				break;
		case 7: node = createNewASTnode(NULL, TAG_MODULE, parent, NULL, NULL, parseTreeRoot->g);
				child = find_child(parseTreeRoot, T_ID,1);
				node->first_child = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, child->g);
				nextNode = last_child(node->first_child);
				child = find_child(parseTreeRoot, NT_input_plist,1);
				nextNode->next_sibling = make_ast(child, node);
				child = find_child(parseTreeRoot, NT_ret,1);
				nextNode = last_child(node->first_child);
				nextNode->next_sibling = make_ast(child, node);
				child =  find_child(parseTreeRoot, NT_moduleDef,1);
				nextNode = last_child(node->first_child);
				nextNode->next_sibling = make_ast(child, node);
				break;
		case 8: node = make_ast(find_child(parseTreeRoot, NT_output_plist,1), parent);
				break;
		case 9: node = NULL;
				break;
		case 10: node = createNewASTnode(NULL, TAG_INPUTPARAMETER, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, T_ID, 1);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, child->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_dataType,1),node);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_input_plist_1, 1), parent);
				 break;
		case 11: node = createNewASTnode(NULL, TAG_INPUTPARAMETER, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, T_ID, 1);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, child->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_dataType,1),node);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_input_plist_1, 1), parent);
				 break;
		case 12: node = NULL;
				 break;
		case 13: node = createNewASTnode(NULL, TAG_OUTPUTPARAMETER, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, T_ID, 1);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, child->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_type,1),node);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_output_plist_1, 1), parent);
				 break;
		case 14: node = createNewASTnode(NULL, TAG_OUTPUTPARAMETER, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, T_ID, 1);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, child->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_type,1),node);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_output_plist_1, 1), parent);
				 break;
		case 15: node = NULL;
				 break;
		case 16: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, (find_child(parseTreeRoot, T_INTEGER, 1))->g);
				 break;
		case 17: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, (find_child(parseTreeRoot, T_REAL, 1))->g);
				 break;
		case 18: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, (find_child(parseTreeRoot, T_BOOLEAN, 1))->g);
				 break;
		case 19: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_ARRAY, node, NULL, NULL, (find_child(parseTreeRoot, T_ARRAY, 1))->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_range_arrays, 1), node);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_type,1), node);
				 break;
		case 20: node = createNewASTnode(NULL, TAG_RANGEARRAY, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, NT_index, 1);
				 node->first_child = make_ast(child, node);
				 child = find_child(parseTreeRoot, NT_index, 2);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(child, node);
				 break;
		case 21: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, (find_child(parseTreeRoot, T_INTEGER, 1))->g);
				 break;
		case 22: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, (find_child(parseTreeRoot, T_REAL, 1))->g);
				 break;
		case 23: node = createNewASTnode(NULL, TAG_DATATYPE, parent, NULL, NULL, (find_child(parseTreeRoot, T_BOOLEAN, 1))->g);
				 break;
		case 24: node = createNewASTnode(NULL,TAG_MODULEDEF, parent, NULL, NULL, parseTreeRoot->g);
				 node->first_child = createNewASTnode(NULL, TAG_START, node, NULL, NULL, (find_child(parseTreeRoot, T_START, 1))->g);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_statements, 1), node);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = createNewASTnode(NULL, TAG_END, node, NULL, NULL, (find_child(parseTreeRoot, T_END, 1))->g);
				 break;
		case 25: node = make_ast(find_child(parseTreeRoot, NT_statement, 1), parent);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_statements,1), parent);
				 break;
		case 26: node = NULL;
				 break;
		case 27: node = make_ast(find_child(parseTreeRoot, NT_ioStmt,1),parent);
				 break;
		case 28: node = make_ast(find_child(parseTreeRoot, NT_simpleStmt,1),parent);
				 break;
		case 29: node = make_ast(find_child(parseTreeRoot, NT_declareStmt,1),parent);
				 break;
		case 30: node = make_ast(find_child(parseTreeRoot, NT_conditionalStmt,1),parent);
				 break;
		case 31: node = make_ast(find_child(parseTreeRoot, NT_iterativeStmt,1),parent);
				 break;
		case 32: node = createNewASTnode(NULL, TAG_GETVALUESTMT, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_ID, 1))->g);
				 node->first_child = templeaf;
				 break;
		case 33: node = createNewASTnode(NULL, TAG_PRINTSTMT, parent, NULL, NULL, parseTreeRoot->g);
				 node->first_child = make_ast(find_child(parseTreeRoot,NT_var,1),node);
				 break;
		case 34: node = createNewASTnode(NULL, TAG_VAR, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_ID, 1))->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_whichID, 1), node);
				 break;
		case 35: node = createNewASTnode(NULL, TAG_VAR, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_NUMNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_NUM, 1))->g);
				 node->first_child = templeaf;
				 // nextNode = last_child(node->first_child);
				 // nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_whichID, 1), node);				 
				 break;
		case 36: node = createNewASTnode(NULL, TAG_VAR, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_RNUMNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_RNUM, 1))->g);
				 node->first_child = templeaf;
				 // nextNode = last_child(node->first_child);
				 // nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_whichID, 1), node);				 
				 break;
		case 37 :node = createNewASTnode(NULL, TAG_VAR, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_TRUENODE, node, NULL, NULL, (find_child(parseTreeRoot, T_TRUE, 1))->g);
				 node->first_child = templeaf;
				 //nextNode = last_child(node->first_child);
				 //nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_whichID, 1), node);				 
				 break;
		case 38: node = createNewASTnode(NULL, TAG_VAR, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_FALSENODE, node, NULL, NULL, (find_child(parseTreeRoot, T_FALSE, 1))->g);
				 node->first_child = templeaf;
				 //nextNode = last_child(node->first_child);
				 //nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_whichID, 1), node);				 
				 break;
		case 39: node = make_ast(find_child(parseTreeRoot, NT_index, 1), parent);
				 break;
		case 40: node = NULL;
				 break;
		case 41: node = make_ast(find_child(parseTreeRoot, NT_moduleReuseStmt, 1), parent);
				 break;
		case 42: node = make_ast(find_child(parseTreeRoot, NT_assignmentStmt, 1), parent);
				 break;
		case 43: node = createNewASTnode(NULL, TAG_ASSIGNMENTSTMT, parent, NULL, NULL, parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_ID,1))->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_whichStmt, 1), node);
				 break;
		case 44: node = make_ast(find_child(parseTreeRoot, NT_lvalueIDStmt,1), parent);
				 break;
		case 45: node = make_ast(find_child(parseTreeRoot, NT_lvalueARRStmt,1), parent);
				 break;
		case 46: node = make_ast(find_child(parseTreeRoot, NT_expression,1), parent);
				 break;
		case 47: node = make_ast(find_child(parseTreeRoot, NT_index, 1), parent);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_expression,1),parent);
				 break;
		case 48: node = createNewASTnode(NULL, TAG_NUMNODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_NUM, 1))->g);
				 break;
		case 49: node = createNewASTnode(NULL, TAG_IDNODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_ID, 1))->g);
				 break;
		case 50: node = createNewASTnode(NULL, TAG_MODULEREUSESTMT, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, NT_optional, 1);
				 templeaf = createNewASTnode(NULL, TAG_OPTIONAL, node, NULL, NULL, (find_child(parseTreeRoot, NT_optional, 1))->g);
				 node->first_child = templeaf;
				 templeaf->first_child = make_ast(child, templeaf);
				 if (node->first_child != NULL){
				 	child = find_child(parseTreeRoot,T_ID,1);
				 	templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, child->g);
				 	nextNode = last_child(node->first_child);
				 	nextNode->next_sibling = templeaf;
				 }
				 else{
				 	templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_ID, 1))->g);
				 	node->first_child = templeaf;
				 }
				 nextNode = last_child(node->first_child);
				 child = find_child(parseTreeRoot, NT_idList,1);
				 nextNode->next_sibling = make_ast(child,node);
				 break;
		case 51: node = make_ast(find_child(parseTreeRoot, NT_idList, 1), parent);
				 break;
		case 52: node = NULL;
				 break;
		///dish
		case 53: child = find_child(parseTreeRoot, T_ID, 1);
				 node = createNewASTnode(NULL, TAG_IDNODE, parent, NULL, NULL, child->g);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_idList_1,1), parent);
				 break;
		case 54: child = find_child(parseTreeRoot,T_ID,1);
				 node = createNewASTnode(NULL,TAG_IDNODE,parent,NULL,NULL,child->g);
				 node->next_sibling = make_ast(find_child(parseTreeRoot,NT_idList_1,1),parent);
				 break;
		case 55: node = NULL;
				 break;
		case 56: //NT_expression -> NT_arithmeticOrBooleanExpr
				 node = make_ast(find_child(parseTreeRoot, NT_arithmeticOrBooleanExpression, 1), parent);
				 break;
		case 57: //NT_expression -> NT_U
				 node = make_ast(find_child(parseTreeRoot, NT_U, 1), parent);
				 break;
		case 58: //NT_U -> NT_sign NT_new_NT
				 node = make_ast(find_child(parseTreeRoot, NT_sign, 1), parent);
				 node->first_child = make_ast(find_child(parseTreeRoot, NT_new_NT,1),node);
				 break;
				 // node = createNewASTnode(NULL,TAG_SIGNEDSTMT,parent,NULL,NULL,parseTreeRoot->g);
				 // child = find_child(parseTreeRoot,NT_sign,1);
				 // node->first_child = make_ast(child,node);
				 // child= find_child(parseTreeRoot,NT_new_NT,1);
				 // nextNode = last_child(node->first_child);
				 // nextNode->next_sibling=make_ast(child, node);
				 // break;
		case 59: //NT_sign  -> T_PLUS
				 node = createNewASTnode(NULL, TAG_PLUSNODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_PLUS, 1))->g);
				 break;
		case 60: //NT_sign -> T_MINUS
				 node = createNewASTnode(NULL, TAG_MINUSNODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_MINUS, 1))->g);
				 break;
		case 61: //NT_new_NT -> T_BO NT_arithmeticExpr T_BC
				 node = make_ast(find_child(parseTreeRoot, NT_arithmeticExpr, 1), parent);
				 break;
		case 62: //NT_new_NT -> NT_var_id_num
				 node = make_ast(find_child(parseTreeRoot, NT_var_id_num, 1), parent);
				 break;
		case 63: //NT_var_id_num -> T_ID NT_whichID
				 node = createNewASTnode(NULL, TAG_VAR,parent,NULL,NULL,parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_ID,1))->g);
				 node->first_child = templeaf;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot,NT_whichID,1),node);
				 break;
		case 64: //NT_var_id_num -> T_NUM
				 node = createNewASTnode(NULL, TAG_VAR,parent,NULL,NULL,parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_NUMNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_NUM,1))->g);
				 node->first_child = templeaf;
				 break;
		case 65: //NT_var_id_num -> T_RNUM
				 node = createNewASTnode(NULL, TAG_VAR,parent,NULL,NULL,parseTreeRoot->g);
				 templeaf = createNewASTnode(NULL, TAG_RNUMNODE, node, NULL, NULL, (find_child(parseTreeRoot, T_RNUM,1))->g);
				 node->first_child = templeaf;
				 break;
		case 66: ;//NT_arithmeticOrBooleanExpr -> NT_booleanTerm NT_booleanExpr_1
				 
				 astNode boolexpr1node = make_ast(find_child(parseTreeRoot, NT_booleanExpr_1,1), NULL);
				 astNode btnode = make_ast(find_child(parseTreeRoot, NT_booleanTerm,1), NULL);
				 if(boolexpr1node != NULL){
				 	astNode temp1 = boolexpr1node->first_child;
				 	boolexpr1node->first_child = btnode;
				 	btnode->parent = boolexpr1node;
				 	btnode->next_sibling = temp1;
				 	node = boolexpr1node;
				 }
				 else{
				 	node = btnode;
				 }
				 break;

				 // node = make_ast(find_child(parseTreeRoot,NT_booleanTerm,1),parent);
			  //    ptr1 = make_ast(find_child(parseTreeRoot,NT_booleanExpr_1,1),NULL);
			  //    handle_inh(&node, &ptr1, &ptr2);
			  //    break;
		case 67: //NT_booleanExpr_1 -> NT_logicalOp NT_booleanTerm NT_booleanExpr_1

				 node = make_ast(find_child(parseTreeRoot,NT_logicalOp,1), parent);
				 astNode booltermnode = make_ast(find_child(parseTreeRoot,NT_booleanTerm,1), NULL);
				 astNode be1node = make_ast(find_child(parseTreeRoot, NT_booleanExpr_1, 1), NULL);

				 if(be1node != NULL){
				 	astNode temp1 = be1node->first_child;
				 	be1node->first_child = booltermnode;
				 	booltermnode->parent = be1node;
				 	booltermnode->next_sibling = temp1;
				 	node->first_child = be1node;
				 	be1node->parent = node;
				 }
				 else{
				 	node->first_child = booltermnode;
				 	booltermnode->parent = node;
				 }

				 break;

				 // node = createNewASTnode(NULL, TAG_BOOLEXPR,parent,NULL,NULL,(find_child(parseTreeRoot,NT_logicalOp,1))->g);
				 // node->first_child = make_ast(find_child(parseTreeRoot,NT_booleanTerm,1),node);
				 // ptr1 = make_ast(find_child(parseTreeRoot,NT_booleanExpr_1,1),parent);
				 // if(ptr1 != NULL)
				 // {
				 // 	handle_inh_2(&node, &ptr1, &ptr2);
				 // 	return node;
				 // }
				 // break;
		case 68: //NT_booleanExpr_1 -> epsilon
				 node = NULL;
				 break;
		case 69: ;//NT_booleanTerm -> NT_arithmeticExpr NT_booleanTerm_1
				 astNode boolterm1node = make_ast(find_child(parseTreeRoot, NT_booleanTerm_1,1), NULL);
				 astNode aenode1 = make_ast(find_child(parseTreeRoot, NT_arithmeticExpr,1), NULL);
				 if(boolterm1node != NULL){
				 	astNode temp1 = boolterm1node->first_child;
				 	boolterm1node->first_child = aenode1;
				 	aenode1->parent = boolterm1node;
				 	aenode1->next_sibling = temp1;
				 	node = boolterm1node;
				 }
				 else{
				 	node = aenode1;
				 }
				 break;

				 // node = make_ast(find_child(parseTreeRoot,NT_arithmeticExpr,1),parent);
				 // printf("node done - ");
				 // if(node) printf("%s\n", tag_names[node->label]);
				 // else printf("NULL\n");
			  //    ptr1 = make_ast(find_child(parseTreeRoot,NT_booleanTerm_1,1),NULL);
			  //    printf("ptr1 done - ");
				 // if(ptr1) printf("%s\n", tag_names[ptr1->label]);
				 // else printf("NULL\n");
			  //    handle_inh(&node, &ptr1, &ptr2);
			  //    break;
		case 70: //NT_booleanTerm -> T_TRUE
				 node = createNewASTnode(NULL, TAG_TRUENODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_TRUE,1))->g);
				 break;
		case 71: //NT_booleanTerm -> T_FALSE
				 node = createNewASTnode(NULL, TAG_FALSENODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_FALSE,1))->g);
				 break;
		case 72: //NT_booleanTerm_1 -> NT_relationalOp NT_arithmeticExpr
				 
				 node = make_ast(find_child(parseTreeRoot, NT_relationalOp,1), parent);
				 node->first_child = make_ast(find_child(parseTreeRoot, NT_arithmeticExpr,1), node);
				 break;


				 // node = createNewASTnode(NULL, TAG_BOOLEXPR, parent, NULL, NULL, parseTreeRoot->g);
				 // node->first_child = make_ast(find_child(parseTreeRoot, NT_relationalOp, 1), node);
				 // node->first_child->next_sibling = make_ast(find_child(parseTreeRoot, NT_arithmeticExpr,1), node);
				 // break;

		case 73: //NT_booleanTerm_1 -> epsilon
				 node = NULL;
				 break;

		case 74: ;//NT_arithmeticExpr -> NT_term NT_arithmeticExpr_1
				 astNode ae1node = make_ast(find_child(parseTreeRoot, NT_arithmeticExpr_1,1), NULL);
				 astNode termnode = make_ast(find_child(parseTreeRoot, NT_term,1), NULL);
				 if(ae1node != NULL){
				 	astNode temp1 = ae1node->first_child;
				 	ae1node->first_child = termnode;
				 	termnode->parent = ae1node;
				 	termnode->next_sibling = temp1;
				 	node = ae1node;
				 }
				 else{
				 	node = termnode;
				 }
				 break;


				 // node = make_ast(find_child(parseTreeRoot,NT_term,1),parent);
			  //    ptr1 = make_ast(find_child(parseTreeRoot,NT_arithmeticExpr_1,1),NULL);
			  //    handle_inh(&node, &ptr1, &ptr2);
			  //    break;

	    case 75: //NT_arithmeticExpr_11 -> NT_op_1 NT_term NT_arithmeticExpr_12
	    		 node = make_ast(find_child(parseTreeRoot,NT_op_1,1), parent);
				 astNode termnode1 = make_ast(find_child(parseTreeRoot,NT_term,1), NULL);
				 astNode ae1node1 = make_ast(find_child(parseTreeRoot, NT_arithmeticExpr_1, 1), NULL);

				 if(ae1node1 != NULL){
				 	astNode temp1 = ae1node1->first_child;
				 	ae1node1->first_child = termnode1;
				 	termnode1->parent = ae1node1;
				 	termnode1->next_sibling = temp1;
				 	node->first_child = ae1node1;
				 	ae1node1->parent = node;
				 }
				 else{
				 	node->first_child = termnode1;
				 	termnode1->parent = node;
				 }

				 break;

		case 76: node = NULL;
				 break;

		case 77: ;//NT_term -> NT_factor NT_term_1
				 astNode term1node1 = make_ast(find_child(parseTreeRoot, NT_term_1,1), NULL);
				 astNode facnode = make_ast(find_child(parseTreeRoot, NT_factor,1), NULL);
				 if(term1node1 != NULL){
				 	astNode temp1 = term1node1->first_child;
				 	term1node1->first_child = facnode;
				 	facnode->parent = term1node1;
				 	facnode->next_sibling = temp1;
				 	node = term1node1;
				 }
				 else{
				 	node = facnode;
				 }
				 break;

				 // node = make_ast(find_child(parseTreeRoot,NT_factor,1),parent);
				 // ptr1 = make_ast(find_child(parseTreeRoot,NT_term_1,1),NULL);
				 // handle_inh(&node, &ptr1, &ptr2);
				 // break;

		case 78: //NT_term_1 -> NT_op_2 NT_factor NT_term_1
				 node = make_ast(find_child(parseTreeRoot,NT_op_2,1), parent);
				 astNode facnode1 = make_ast(find_child(parseTreeRoot,NT_factor,1), NULL);
				 astNode term1node = make_ast(find_child(parseTreeRoot, NT_term_1, 1), NULL);

				 if(term1node != NULL){
				 	astNode temp1 = term1node->first_child;
				 	term1node->first_child = facnode1;
				 	facnode1->parent = term1node;
				 	facnode1->next_sibling = temp1;
				 	node->first_child = term1node;
				 	term1node->parent = node;
				 }
				 else{
				 	node->first_child = facnode1;
				 	facnode1->parent = node;
				 }

				 break;

				 // node = createNewASTnode(NULL, TAG_ARITHMETICEXPR,parent,NULL,NULL,(find_child(parseTreeRoot,NT_op_2,1))->g);
				 // node->first_child = make_ast(find_child(parseTreeRoot,NT_factor,1),node);
				 // ptr1 = make_ast(find_child(parseTreeRoot,NT_term_1,1),parent);
				 // if(ptr1 != NULL)
				 // {
				 // 	handle_inh_2(&node, &ptr1, &ptr2);
				 // 	return node;
				 // }
				 // break;

		case 79: //NT_term_1 -> epsilon
				 node = NULL;
				 break;
		case 80: //NT_factor T_BO -> NT_arithmeticOrBooleanExpr T_BC
				 node = make_ast(find_child(parseTreeRoot,NT_arithmeticOrBooleanExpression,1),parent);
				 break;
		case 81: //NT_factor -> NT_var
				 node = make_ast(find_child(parseTreeRoot,NT_var,1),parent);
				 break;
		case 82: node = createNewASTnode(NULL, TAG_PLUSNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_PLUS,1))->g);
				 break;
		case 83: node = createNewASTnode(NULL, TAG_MINUSNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_MINUS,1))->g);
				 break;
		case 84: node = createNewASTnode(NULL, TAG_MULNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_MUL,1))->g);
				 break;
		case 85: node = createNewASTnode(NULL, TAG_DIVNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_DIV,1))->g);
				 break;
		case 86: node = createNewASTnode(NULL, TAG_ANDNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_AND,1))->g);
				 break;
		case 87: node = createNewASTnode(NULL, TAG_ORNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_OR,1))->g);
				 break;
		case 88: node = createNewASTnode(NULL, TAG_LTNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_LT,1))->g);
				 break;
		case 89: node = createNewASTnode(NULL, TAG_LENODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_LE,1))->g);
				 break;
		case 90: node = createNewASTnode(NULL, TAG_GTNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_GT,1))->g);
				 break;
		case 91: node = createNewASTnode(NULL, TAG_GENODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_GE,1))->g);
				 break;
		case 92: node = createNewASTnode(NULL, TAG_EQNODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_EQ,1))->g);
				 break;
		case 93: node = createNewASTnode(NULL, TAG_NENODE,parent,NULL,NULL,(find_child(parseTreeRoot,T_NE,1))->g);
				 break;

		case 94: node = createNewASTnode(NULL, TAG_DECLARESTMT, parent, NULL, NULL, parseTreeRoot->g);
				 child = find_child(parseTreeRoot, NT_idList, 1);
				 node->first_child = make_ast(child, node);
				 child = find_child(parseTreeRoot,NT_dataType,1);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(child,node);
				 break;
		case 95: node = createNewASTnode(NULL, TAG_CONDITIONALSTMT, parent, NULL, NULL, parseTreeRoot->g);
				 astNode n = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot,T_ID,1))->g);
				 node->first_child = n;
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = createNewASTnode(NULL, TAG_START, node, NULL, NULL, (find_child(parseTreeRoot, T_START,1))->g);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_caseStmt, 1), node);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_defaultStmt, 1),node);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = createNewASTnode(NULL, TAG_END, node, NULL, NULL, (find_child(parseTreeRoot, T_END,1))->g);
				 break;
		case 96: node = createNewASTnode(NULL, TAG_CASESTMT, parent, NULL, NULL, parseTreeRoot->g);
				 node->first_child = make_ast(find_child(parseTreeRoot,NT_value,1),node);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_statements,1),node);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_caseStmts, 1), parent);
				 break;
		case 97: node = createNewASTnode(NULL, TAG_CASESTMT, parent, NULL, NULL, parseTreeRoot->g);
				 node->first_child = make_ast(find_child(parseTreeRoot,NT_value,1),node);
				 nextNode = last_child(node->first_child);
				 nextNode->next_sibling = make_ast(find_child(parseTreeRoot, NT_statements,1),node);
				 node->next_sibling = make_ast(find_child(parseTreeRoot, NT_caseStmts, 1), parent);
				 break;
		case 98: node = NULL;
				 break;
		case 99: node = createNewASTnode(NULL, TAG_NUMNODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_NUM, 1))->g);
				 break;
		case 100: node = createNewASTnode(NULL, TAG_TRUENODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_TRUE, 1))->g);
		 		  break;
		case 101: node = createNewASTnode(NULL, TAG_FALSENODE, parent, NULL, NULL, (find_child(parseTreeRoot, T_FALSE, 1))->g);
		 		  break;
		case 102: node = createNewASTnode(NULL,TAG_DEFAULTSTMT, parent, NULL, NULL, (find_child(parseTreeRoot, T_DEFAULT,1))->g);
				  node->first_child = make_ast(find_child(parseTreeRoot,NT_statements,1),node);
		 		  break;
		case 103: node = NULL;
					break;
		case 104: node = createNewASTnode(NULL, TAG_FORSTMT, parent, NULL, NULL, parseTreeRoot->g);
				   node->first_child = createNewASTnode(NULL, TAG_IDNODE, node, NULL, NULL, (find_child(parseTreeRoot,T_ID,1))->g);
				   //createNewASTnode(NULL, TAG_START, node, NULL, NULL, (find_child(parseTreeRoot, T_START,1))->g);
				   nextNode = last_child(node->first_child);
				   nextNode->next_sibling = make_ast(find_child(parseTreeRoot,NT_range,1),node);
				   nextNode = last_child(node->first_child);
				   nextNode->next_sibling = createNewASTnode(NULL, TAG_START, node, NULL, NULL, (find_child(parseTreeRoot, T_START,1))->g);
				   nextNode = last_child(node->first_child);
				   nextNode->next_sibling = make_ast(find_child(parseTreeRoot,NT_statements,1),node);
				   nextNode = last_child(node->first_child);
				   nextNode->next_sibling = createNewASTnode(NULL, TAG_END, node, NULL, NULL, (find_child(parseTreeRoot, T_END,1))->g);
				   break;
		case 105: node = createNewASTnode(NULL, TAG_WHILESTMT, parent, NULL, NULL, parseTreeRoot->g);
				  node->first_child = make_ast(find_child(parseTreeRoot,NT_arithmeticOrBooleanExpression,1),node);
				  nextNode = last_child(node->first_child);
				  nextNode->next_sibling = createNewASTnode(NULL, TAG_START, node, NULL, NULL, (find_child(parseTreeRoot, T_START,1))->g);
				  nextNode = last_child(node->first_child);
				  nextNode->next_sibling = make_ast(find_child(parseTreeRoot,NT_statements,1),node);
				  nextNode = last_child(node->first_child);
				  nextNode->next_sibling = createNewASTnode(NULL, TAG_END, node, NULL, NULL, (find_child(parseTreeRoot, T_END,1))->g);
				  break;
		case 106: node = createNewASTnode(NULL, TAG_RANGEARRAY, parent, NULL, NULL, parseTreeRoot->g);
				  node->first_child = createNewASTnode(NULL, TAG_NUMNODE,node,NULL,NULL,(find_child(parseTreeRoot,T_NUM,1))->g);
				  nextNode = last_child(node->first_child);
				  nextNode->next_sibling = createNewASTnode(NULL, TAG_NUMNODE,node,NULL,NULL,(find_child(parseTreeRoot,T_NUM,2))->g);
				  break;
	} 
	if (node != NULL){
		if(parent)node->parent = parent;
		//printf("k\n");
	}
	//printf("extiting make_ast with rule %d and node %s\n", rule_num, names_ntandt[parseTreeRoot->g->name]);
	return node;

}


void print_ast(astNode root){
	//printf("entering print_ast \n");
	if (root == NULL) return;
	printf("%s : ", tag_names[root->label]);
	if (root->parent)printf("%s\n", tag_names[root->parent->label]);
	else printf("NULL\n");
	if(root->g != NULL) printf("-%s ", names_ntandt[root->g->name]);
	printf("---->");
	if (root->first_child) print_ast(root->first_child);
	if (root->next_sibling) print_ast(root->next_sibling);
	//printf("extiting print_ast\n");
	return;

}

astNode ast_main(ptnode parseTreeRoot){
	printf("enter ast_main\n");
	astNode root = make_ast(parseTreeRoot, NULL);
	printf("AST CREATION DONE!!\n");
	return root;
}

