//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

parse_table pt;
hash_node hash_table[HASH_SIZE];
grammar_block grammar_table[NO_OF_RULES];
mapTableFirstBlock firstSetMT[TOTAL_NON_TERMINALS];
FirstAndFollow F[TOTAL_NON_TERMINALS];

int syntaxErr = 0;
int PARSETREE_SIZE = 0;


//Functions for parsing

ptnode parseInputSourceCode(char *filename, int** ptable, int *synataxerrors){
	FILE *fp = fopen(filename,"r");
	if (fp == NULL){
		printf("\n File cant be opened for parsing\n");
		return NULL;
	}

	ptnode root = initializeParseTree(); // malloc + making create a start node
	printf("tree initialised\n");
	Stack stack = createStack();

	ptnode dollarnode = createParseTreeNode(NULL, T_DOLLAR, "T_DOLLAR", 0, -3);

	push(stack, dollarnode);
	push(stack, root);

	printf("stack created and initialised\n");

	int t;

	isError = 0; // whether I detected an error
	state = 0;
	row = 1;
	currStrStart = 0;

	token ptr=NULL;

	clearString(ipStr,50000);

	bool reUseToken = true;

	while ((t=fread(ipStr,1,50000,fp))){
		lookahead = 0;
		while(ipStr[lookahead] != '\0'){
			ptr = getNextToken();
			if (ptr == NULL) continue;
			//printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
			reUseToken= true;
			while (reUseToken){
				usetokenforparsing(ptr, stack, synataxerrors, &reUseToken);
			}
			reUseToken = true;
		}

		clearString(ipStr,50000);

		if (strlen(currStr)>0){
			//ptr = acceptState(currStr,state);
			//printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
			//printf("herreeee\n");
			ptr = getNextToken();
			if (ptr == NULL) continue;
			while (reUseToken){
				usetokenforparsing(ptr, stack, synataxerrors, &reUseToken);
			}
			reUseToken = true;
		}

	}
	if (strlen(currStr)>0){
			ptr = acceptState(currStr,state);
			//printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
			while (reUseToken){
				usetokenforparsing(ptr, stack, synataxerrors, &reUseToken);
			}
			reUseToken = true;
		}

	token eof = (token) malloc(sizeof(token));
	strcpy(eof->tokenType, "T_DOLLAR");
	strcpy(eof->tokenVal, "eof_T_DOLLAR");
	while (reUseToken){
		usetokenforparsing(eof, stack, synataxerrors, &reUseToken);
	}
	reUseToken = true;
	if(*synataxerrors != -1) isparsingover(stack,synataxerrors);
	printf(" done \n");	
	return root;
}

void usetokenforparsing(token ptr, Stack stack, int *errors, bool* reUseToken){
	int incomingtokentype; 
	//printf("\n\n*************\n");
	//printStack(stack);

	//setting incoming tokentype based on token recieved
	if (strcmp(ptr->tokenType, "T_ERROR") == 0) incomingtokentype = -1;
	else incomingtokentype = find_enum_val(ptr->tokenType);
	

	//if lexical error comes
	if (incomingtokentype == -1){
		*reUseToken=false;
		return;
	}

	//stack empty but tokens left
	stack_node topofstack = top(stack);
	
	if(topofstack == NULL){
	//if (topofstack->p->g->name == T_DOLLAR ){
		*errors = -1;
		printf("ERROR_4: Unexpected Tokens at End - Stack empty but input source code is not over\n");
		printf("PARSING INCORRECT \n");
		*reUseToken = false;
//COMENTED
		//free(topofstack);
		return;
	}

	topofstack = pop(stack);
	//printStack(stack);
	ptnode tos_pnode = topofstack->p;

	//if top of stack is terminal
	if (tos_pnode->g->flag_ntandt == 0){
		if (incomingtokentype == tos_pnode->g->name){
			//matched
			//printf("matched\n");
			tos_pnode->g->tok = ptr;
			*reUseToken = false;
			return;
		}
		else {
			//error
			*errors = 1;
			if (tos_pnode->g->name == T_SEMICOL){
				printf("ERROR_5: Line: %d - Semicolon missing.\n", ptr->rownum);
			}
			else {
				printf("ERROR_6: Line: %d Expected Token %s but got lexeme %s of type %s\n", ptr->rownum, names_ntandt[tos_pnode->g->name], ptr->tokenVal, ptr->tokenType );
			}
			
//COMENTED
//	free(topofstack);
			return;
			
		}
	}
	// top of stack is non terminal
	else {


		int whichRule = pt[tos_pnode->g->name][incomingtokentype-TOTAL_NON_TERMINALS];
		//printf("whichRule: %d\n",whichRule );

		if (whichRule == -1){
			//error
			*errors = 1;
			push(stack, tos_pnode);
			*reUseToken = false;

			//COMMENTED		free(topofstack);
			return;
		}
		else if (whichRule == -2){
			//syn set
			*errors = 1;
			printf("ERROR_6: Line: %d Expected Token %s but got lexeme %s of type %s\n", ptr->rownum, names_ntandt[tos_pnode->g->name], ptr->tokenVal, ptr->tokenType );

			//COMMENTED		free(topofstack);
			return;
		}
		else {

			tos_pnode->first_child = addChild(whichRule, tos_pnode);
			tos_pnode->rule_no = whichRule;
			if(tos_pnode->first_child != NULL){
				pushAllChildren(stack, tos_pnode->first_child);
			}
			//free(topofstack->p->g);
			//free(topofstack->p);
			//COMMENTED		free(topofstack);
			return;
		}

	}

}

void isparsingover(Stack stack, int* errors){
	//printf("&&&&&&&\n");
	//printStack(stack);
	stack_node topnode = top(stack);
	if (topnode==NULL){
		printf("PARSING COMPLETE AND CORRECT\n");
		*errors = 0;
	}else{
		*errors = 1;
		//printf("%d\n", topnode->p->g->name);
		printf("ERROR_3: Error in parsing. Stack is not empty but No nore tokens in input source code\n");
		printf("PARSING INCORRECT\n");

	}
	return;
}



//Parse Table

parse_table initialiseParseTable(){
	pt = (int**)malloc(sizeof(int*)*TOTAL_NON_TERMINALS);
	for (int i=0; i<TOTAL_NON_TERMINALS; i++) {
	        pt[i] = (int*)malloc(sizeof(int)*TOTAL_TERMINALS);
	        for (int j=0; j<TOTAL_TERMINALS; j++)
	            pt[i][j] = -1;
	    }
	return pt;
}

parse_table createParseTable(FirstAndFollow *ff){
	pt = initialiseParseTable();
	for (int i=0; i<NO_OF_RULES;i++){
		grammar_block thisRule = grammar_table[i]; // to include grammar.h for declaration of grammarblock
		grammar_block thisRuleRHS = grammar_table[i]->next;
		int leftid = thisRule->name;
		int rightid = thisRuleRHS->name;

		bool *RHSfirst = computeFirstUtil(i);

		if (RHSfirst[epsilon] == true){
			RHSfirst = createUnion(RHSfirst, ff[leftid].follow);
			RHSfirst[epsilon]=false;
		}
		for (int j=0; j<TOTAL_TERMINALS;j++){
			if(RHSfirst[j+TOTAL_NON_TERMINALS]) 
				pt[leftid][j] = i;
		}

		for (int j=0; j<TOTAL_TERMINALS;j++){
			if((pt[leftid][j] == -1) && (F[leftid].follow[j+TOTAL_NON_TERMINALS] == true))
				pt[leftid][j] = -2;
		}

		if(pt[leftid][T_SEMICOL-TOTAL_NON_TERMINALS] == -1)
			pt[leftid][T_SEMICOL-TOTAL_NON_TERMINALS] = -2;

	}

	return pt;
}

void printParseTable(parse_table pt){
	printf("\n");
	for(int i =0;i<TOTAL_NON_TERMINALS;i++){
		for (int j=0;j<TOTAL_TERMINALS;j++){
			printf("%d ", pt[i][j]);
		}
		printf("\n");
	}
}



// Hash Functions

hash_node createHashNode(hash_node h){
	h = (hash_node) malloc(sizeof(struct HASH_NODE));
	return h;
}

int hashFunc(char* pattern){
	//printf("%s\n",pattern);
	int p = 29;
	int m =  HASH_SIZE;
	int hashVal = 0;

	int mult = 1;

	for(int i=0; i<strlen(pattern); i++){
	    hashVal = (hashVal + pattern[i]*mult)%m;
	    mult = (mult*p)%m;
	}
	return hashVal;    
}

void insertHashKey ( char* pattern, int enum_value){
    int hashVal = hashFunc(pattern);
    //printf("%d\n",enum_value);
	if(hash_table[hashVal]==NULL)
	{
		hash_table[hashVal] = createHashNode(hash_table[hashVal]);
		strcpy(hash_table[hashVal]->name,pattern);
		hash_table[hashVal]->enum_val = enum_value;
		hash_table[hashVal]->next = NULL;
	}
	else
	{
		hash_node it = hash_table[hashVal];
		while(it->next){
			it = it->next;
	    }
	    it->next = createHashNode(it->next);
	    strcpy(it->next->name,pattern);
		it->next->enum_val = enum_value;
		it->next->next = NULL;
	}
}

void hash_ntandt(char** to_hash){
	//printf("enetered func\n");
	for(int i=0;i<MAX_NTANDT;i++)
	{
		//printf("%d\n",i);
		insertHashKey(to_hash[i],i);
	}
	//printf("over\n");
}

int find_in_hash(char* word){
	return hashFunc(word);
}


int find_enum_val(char* word){
	int pos = find_in_hash(word);
	//printf("hash val is %d\n",pos);
	hash_node temp = hash_table[pos];
	while(temp){
		if(strcmp(temp->name,word)==0)
			return temp->enum_val;
		temp=temp->next;
	}
	printf("%s not found in hash table\n",word);
	return -1;
}

void display_hash_table(hash_node* hash_table){
	for(int i=0;i<HASH_SIZE;i++)
		{
			hash_node head = hash_table[i];
			printf("hashed to : %d\n",i);
			if(head)
			{	printf("%s %d\n",head->name,head->enum_val);
				hash_node temp=head->next;
				while(temp)
				{
					printf("%s %d\n",temp->name,temp->enum_val);
					temp=temp->next;
				}
			}
		}
}

// Grammar Table

grammar_block createGrammarNode(grammar_block gb){
	gb = (grammar_block) malloc(sizeof(struct g));
	return gb;
}

void populateGrammarTable(char* filename){
	int i = 0;
	FILE* fp = fopen(filename,"r");
	if (fp == NULL){
		printf("grammar file cannot be opened\n");
		return;
	}
	char* line = (char*) malloc(200*sizeof(char)); 
	//read one rule at a time
	while(fgets(line,200*sizeof(char),fp))
	{
		if(line[strlen(line)-1]=='\n')
			line[strlen(line)-1]='\0';
		char* word = (char*) malloc(50*sizeof(char));

		word = strtok(line," \n");
		int enum_val = find_enum_val(word);

		grammar_table[i] = createGrammarNode(grammar_table[i]);
		grammar_table[i]->name = enum_val;
		grammar_table[i]->next = NULL;
		grammar_table[i]->flag_ntandt = 1;
		strcpy(grammar_table[i]->str_name,word);
		grammar_block t = grammar_table[i];
			
		int j=0;
		while((word = strtok(NULL," ")))
		{
			enum_val = find_enum_val(word);

			t->next = (grammar_block) malloc(sizeof(struct g));
			t->next->name = enum_val;
			strcpy(t->next->str_name,word);
			if(word[0]=='T')
			 	t->next->flag_ntandt=0;
			else if(word[0]=='N')
			 	t->next->flag_ntandt = 1;
			else if(word[0]=='e')
			 	t->next->flag_ntandt = 0;
			else printf("*****************************************************************************************************************");
			t->next->next = NULL;
			t = t->next;
		}
		i++;
	}

}

void displayGrammarTable(){



	for(int i=0;i<NO_OF_RULES;i++)
	{
		grammar_block temp = grammar_table[i];
		while(temp)
		{
			printf("%d: %s -> ",i, temp->str_name);
			temp=temp->next;
		}
		printf("\n");
	}
}


// First and Follow sets

void computeFirstAndFollow (){
	int k;

	for (int i=0; i<TOTAL_NON_TERMINALS; i++){
		F[i].flag_first = false;
		F[i].flag_follow = false;
		F[i].first = (bool*)calloc(MAX_NTANDT,sizeof(bool));
		F[i].follow = (bool*)calloc(MAX_NTANDT,sizeof(bool));
	}

	for (int i=0; i<TOTAL_NON_TERMINALS; i++){
		if(!F[i].flag_first){
			computeFirst(i);
		}
	}

    F[NT_program].follow[T_DOLLAR] = true;
	for (int i=0; i<TOTAL_NON_TERMINALS; i++){
		if(!F[i].flag_follow){
			computeFollow(i);
		}
	}
}

void computeFirst( int x ){

	//printf("%d in func\n ",x);

	int s = firstSetMT[x].start;
	int e = firstSetMT[x].end;//start point in the grammar table

	while(s<=e){
		grammar_block temp = grammar_table[s]->next;
		//printf("%d shef\n",temp->name);
		if(temp->name >= T_INTEGER && temp->name <= T_RNUM)
			F[x].first[temp->name] = true;
		else if(temp->name == epsilon)
			F[x].first[epsilon] = true;
		else{
			if(!F[temp->name].flag_first)
				computeFirst(temp->name);
			grammar_block temp2=temp;
			while(F[temp2->name].first[epsilon] ==  true && temp2->next !=NULL){
				F[temp2->name].first[epsilon] = false;
				F[x].first = createUnion(F[temp2->name].first, F[x].first);
				F[temp2->name].first[epsilon] = true;
				temp2=temp2->next;

				if(temp2->name >= T_INTEGER && temp2->name <= T_RNUM)
					break;
				if(!F[temp2->name].flag_first)
					computeFirst(temp2->name);
			}
			if((temp2->name >= T_INTEGER && temp2->name <= T_RNUM) || temp2->name == epsilon)
				F[x].first[temp2->name] = true;
			else
				F[x].first = createUnion(F[temp2->name].first, F[x].first);
		}
		//printf("%d\n",s);
		s++;
	}

	F[x].flag_first = true;

}

bool* computeFirstUtil( int i ){
	//printf("%d in func\n ",x);
    bool* firstSet = (bool*)calloc(MAX_NTANDT, sizeof(bool));
	grammar_block temp = grammar_table[i]->next;
	if(temp->name >= T_INTEGER && temp->name <= T_RNUM)
		firstSet[temp->name] = true;
	else if(temp->name == epsilon)
		firstSet[epsilon] = true;
	else{
		grammar_block temp2 = temp;
		while(F[temp2->name].first[epsilon] ==  true && temp2->next !=NULL){
			F[temp2->name].first[epsilon] = false;
			firstSet = createUnion(F[temp2->name].first, firstSet);
			F[temp2->name].first[epsilon] = true;
			temp2=temp2->next;

			if(temp2->name >= T_INTEGER && temp2->name <= T_RNUM)
				break;
		}
	
		if((temp2->name >= T_INTEGER && temp2->name <= T_RNUM) || temp2->name == epsilon)
			firstSet[temp2->name] = true;
		else
			firstSet = createUnion(F[temp2->name].first, firstSet);
	}

	return firstSet;
}

void computeFollow( int x){
	//printf("%d in func\n",x);
	grammar_block temp = (grammar_block)calloc(1, sizeof(struct g));

	for(int i=0; i<NO_OF_RULES; i++){
		temp = grammar_table[i]->next;
		while(temp){
			if(temp->name == x){
				grammar_block temp2 = temp->next;
				if(temp2 == NULL){
					if(grammar_table[i]->name != x){
						if(!F[grammar_table[i]->name].flag_follow){
							computeFollow(grammar_table[i]->name);
						}
						F[x].follow = createUnion(F[grammar_table[i]->name].follow, F[x].follow);
					}
				}
				else if(temp2->name >= T_INTEGER && temp2->name <= T_RNUM){
					F[x].follow[temp2->name]=true;
				}
				else if(F[temp2->name].first[epsilon]!=true){
					F[x].follow = createUnion(F[temp2->name].first, F[x].follow);
				}
				else{
					while(temp2!=NULL && temp2->flag_ntandt == 1 && F[temp2->name].first[epsilon]==true ){
						F[x].follow = createUnion(F[temp2->name].first, F[x].follow);
						
						if(temp2->next == NULL){
							if(grammar_table[i]->name != x){
								if(!F[grammar_table[i]->name].flag_follow){
									computeFollow(grammar_table[i]->name);
								}
								F[x].follow = createUnion(F[grammar_table[i]->name].follow, F[x].follow);
							}
						}

						temp2 = temp2->next;
					}
					if (temp2 != NULL && temp2->flag_ntandt == 0) F[x].follow[temp2->name] = true;
					if (temp2 != NULL && temp2->flag_ntandt == 1) F[x].follow = createUnion(F[temp2->name].first, F[x].follow);
				}
			}
			temp = temp->next;
		}

	}
	F[x].follow[epsilon] = false;
	F[x].flag_follow = true;
}

void printFirst(){
	for(int i = 0; i<TOTAL_NON_TERMINALS; i++){
		printf("%s: %d:		",names_ntandt[i],i);
		for(int j=0; j<MAX_NTANDT; j++){
			if(F[i].first[j]==true)
				printf("%s: %d ", names_ntandt[j], F[i].first[j]);
		}
		printf("\n");
	}
}

void printFollow(){
	for(int i = 0; i<TOTAL_NON_TERMINALS; i++){
		printf("%s: %d:		",names_ntandt[i],i);
		for(int j=0; j<MAX_NTANDT; j++){
			if(F[i].follow[j]==true)
				printf("%s: %d ", names_ntandt[j], F[i].follow[j]);
		}
		printf("\n");
	}
}

void populateMapTableFirst(){

	//printf("HELO\n");

	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		firstSetMT[i].start=-1;
		firstSetMT[i].end=-1;
	}
	int temp;
	for(int i=0; i<NO_OF_RULES; i++){
		temp = grammar_table[i]->name;
		//printf("%d\n",  temp);
		if(firstSetMT[temp].start == -1){
			//printf("YO\n");
			firstSetMT[temp].start = i;
		}
		firstSetMT[temp].end = i;
	}

}

bool* createUnion(bool* A, bool* B){

	bool* result = (bool*)calloc(MAX_NTANDT,sizeof(bool));

	for(int i=0; i<MAX_NTANDT; i++){
		if(A[i] == true || B[i] == true)
			result[i] = true;
		else
			result[i] = false;
	}

	return result;
}


//Stack Functions
Stack createStack(){
	Stack stack = (Stack) malloc(sizeof(Stack));
	stack->head = NULL;
	stack->tail = NULL;
	stack->size = 0;
    return stack;
}


stack_node top(Stack s){
	return s->head;
}

stack_node createStackNode(	ptnode pnode){
	//printf("A\n");
	stack_node snode = (stack_node) calloc(1,sizeof(struct stackNode));
	//printf("B\n");
	snode->p = pnode;
	snode->next = NULL;
	return snode;
}

stack_node pop(Stack stack) {
    if (stack->size == 0) {
        return NULL;
    }
    stack_node temp = stack -> head;
    stack -> head = stack -> head -> next;
    stack -> size --;
    return temp;
}

void push(Stack stack, ptnode stack_data) {
    stack_node new_node = createStackNode(stack_data);

    if (stack -> size == 0) {
        stack -> head = new_node;
        stack -> tail = new_node;
        stack -> size++;
    } else {
        new_node -> next = stack -> head;
        stack -> head = new_node;
        stack -> size++;
    }
}

void pushAllChildren(Stack stack, ptnode stack_data) {
    if (stack_data->next_sibling!=NULL)
    {
        pushAllChildren(stack,stack_data->next_sibling);
        //printf("ShefCheck %s\n",stack->head->p->g->str_name);
    }

    push(stack,stack_data);
}


void printStack(Stack stack) {
    stack_node temp = stack->head;
    while(temp!=NULL)
    {
        printf("%s\n",temp->p->g->str_name);
        temp = temp->next;
    }

    //printf("stck done\n\n");
}


//Parse tree functions

ptnode createParseTreeNode(ptnode parent, NTANDT t, char *str, int flag, int rule_no){
	ptnode new_node = (ptnode)malloc(sizeof(struct parseTreeNode));
	if (new_node==NULL){
		printf("Cant alloc mem in ptnode\n");
		exit(1);
	}
	
	new_node->g = (grammar_block)malloc(sizeof(struct g));
	if(new_node->g == NULL){
		printf("Cant alloc mem in ptnode\n");
		exit(1);
	}
	new_node->g->name = t;
	new_node->g->flag_ntandt = flag;
	strcpy(new_node->g->str_name, str);
	new_node->g->next = NULL;
	/////*****((((()))))*****
	new_node->g->tok = NULL;
	new_node->parent = parent;
	new_node->first_child = NULL;
	new_node->next_sibling = NULL;
	new_node->rule_no = rule_no;
	new_node->no_of_children = 0;
	PARSETREE_SIZE++;
	return new_node;
}


ptnode createlistofchildren(grammar_block ruleRHShead, ptnode parent, int rule){
	ptnode headoflist = NULL;
	ptnode itr = NULL;
	grammar_block blockitr = ruleRHShead;
	//printf("rhs: %s\n", names_ntandt[ruleRHShead->name]);
	while(blockitr){
		ptnode temp = createParseTreeNode(parent, blockitr->name, blockitr->str_name, blockitr->flag_ntandt, rule);
		//printf("aftercreate parsetreenode ");
		if (headoflist == NULL){
			headoflist = temp;
			itr = temp;
		}
		else {
			itr->next_sibling = temp;
			itr = temp;
		}
		blockitr = blockitr->next;
	}
	return headoflist;

}

ptnode addChild(int rule, ptnode parent){
	//printf("enter\n");
	grammar_block thisrule = grammar_table[rule];
	//printf("thisrule: name %s\n", names_ntandt[thisrule->name]);
	grammar_block thisRuleRHS = thisrule->next;
	//printf("thisrulenext: name %s\n", names_ntandt[thisRuleRHS->name]);
	if(thisRuleRHS->name == epsilon) return NULL;
	return createlistofchildren(thisRuleRHS, parent, rule);	
}


ptnode initializeParseTree(){
	ptnode node = createParseTreeNode(NULL,0,"NT_program",1,0);
	return node;
}



void printParseTreeNode(ptnode node){
	//printf("entered node\n");
	if(node->g->name== epsilon)
	{
		printf("epsilon\n");

	}



	else if(node->first_child== NULL)   // IS LEAF
	{
		//weird cases
		if(node->g->flag_ntandt == 1) //non terminal at leaf
		{
			if(node->parent == NULL)
			{
				printf("ROOT: %s",names_ntandt[node->g->name]);
				//fprintf(f1,"ROOT: %s",names_ntandt[node->g->name]);

			}
			else
			{
				printf("%s",names_ntandt[node->g->name]);
				//fprintf(f1,"%s",names_ntandt[node->g->name]);
			}
			//printf("INCOMPLETE TREE\n");
		}
		else
		{
			char lexeme[200];
			if(node->g->flag_ntandt == 0){
				strcpy(lexeme, node->g->tok->tokenVal);

			}
			printf("%s - %s",names_ntandt[node->g->name], lexeme);
			//fprintf(f1,"%s - %s ",names_ntandt[node->g->name], lexeme);
		}

	}

	else //NOT LEAF
	{
		if(node->parent == NULL)
		{
			printf("ROOT: %s",names_ntandt[node->g->name]);
			//fprintf(f1,"ROOT: %s",names_ntandt[node->g->name]);
		}
		else
		{
			printf("%s",names_ntandt[node->g->name]);
			//fprintf(f1,"%s",names_ntandt[node->g->name]);
		}	
	}

}



void printParseTree(ptnode r){
	//FILE* f1 = fopen(filename,"a+");
	//printf("opened\n");
	//ptnode r = root;

	char* c = "   ------>    ";
	static int chk=0;
	if(r->first_child != NULL)
	{
		//printf("K\n");
		printParseTree(r->first_child);
	}

	// CHECK IF LEAF in printNode

	if(chk==1)
	{	
		//printf("   ------>    ");
		printf("%s",c);
	}


	printParseTreeNode(r);
	//if(chk==1)
	//{

	//}
	ptnode temp = r->first_child;
	chk=1;

	if(temp)
		temp=temp->next_sibling;

	while(temp)
	{
		//printf("   ------>    ");
		printParseTree(temp);
		temp=temp->next_sibling;

	}
	//fclose(f1);
}

void print_parseTree (ptnode parse_root){
	//FILE* fp = fopen(outputfilename,"w");
	printParseTree(parse_root);
	//fclose(fp);
}

ptnode parser_main(char *filename){
	
	printf("start\n");
	hash_ntandt(names_ntandt);
	populateGrammarTable("input_grammar.txt");
	//displayGrammarTable();
	//printf("check\n");
	populateMapTableFirst();
	computeFirstAndFollow();
	printf("first and follow done\n");
	pt = createParseTable(F);
	//printParseTable(pt);
	printf("parsetable completed\n");
	ptnode root = parseInputSourceCode(filename, pt, &syntaxErr);
	return root;

 }



