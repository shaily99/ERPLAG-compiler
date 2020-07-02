//Group 15
//DISHITA MALAV 2017APS0164P
//SHAILY BHATT 2017A7PS0040P
//SHEFALI TRIPATHI 2017A7PS0139P

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"


char keywordlist[33][20] = {
	"integer",
	"real",
	"boolean",
	"of",
	"array",
	"start",
	"end",
	"declare",
	"module",
	"driver",
	"program",
	"record",
	"tagged",
	"union",
	"get_value",
	"print",
	"use",
	"with",
	"parameters",
	"true",
	"false",
	"takes",
	"input",
	"returns",
	"AND",
	"OR",
	"for",
	"in",
	"switch",
	"case",
	"break",
	"default",
	"while",

	};

char ipStr[50000]; // Input String buffer

char currStr[1000]; //buffer for whatever I have read so far

int isError; // whether I detected an error
int state; //current dfa state
int currStrStart; //pointer in currStr Buffer
int row; // current line number of file being read
int lookahead; //pointer in ipStr buffer


void clearString(char *str, int size){

	//to clear a string


	for (int i=0; i<size;i++){
		str[i] = '\0';
	}
}

//checks whether lexeme is keyword or not
int isKeyword(char *tokstr){
	for (int i=0; i<33; i++){
		if (strcmp(tokstr,keywordlist[i]) == 0) return 1;
	} 
	return 0;
}

//accept states of dfa, returns the lexeme with corresponding token type
token acceptState(char *str, int s){
	int flag;
	token tok1 = (token)malloc(sizeof(struct token));

	if(tok1==NULL) {
		printf("no memory\n");
		exit(1);
	}

	//allocating the lexeme and row number
	strcpy(tok1->tokenVal,currStr);
	tok1->rownum = row;

	//switch case for token type

	switch(s){
		case 2: flag = isKeyword(currStr);
				if (flag){
					int i;
					i=0;
					while(currStr[i]!='\0') {
						//if (currStr[i]>='a' && currStr[i]<= 'z'){
							char ch = currStr[i];
							ch = toupper(ch); 
							currStr[i] = ch;
							i++;
						//s}
					}
					char tempstr[1002];
					clearString(tempstr,1002);
					tempstr[0] = 'T';
					tempstr[1] = '_';
					strcat(tempstr,currStr);
					strcpy(tok1->tokenType, tempstr);
				}
				else {
					if (strlen(currStr)>20){
						//In case ID has length>20, it gives lexical error
						printf("ERROR_2: Identifier: \" %s \" has length greater than 20 at Line Number: %d\n", currStr, row);
						state = 0;
						clearString(currStr,1000);
						return NULL;
					
					}
					else {

						strcpy(tok1->tokenType, "T_ID");
					}
				}
				break;
		case 5: strcpy(tok1->tokenType, "T_NUM");break;
		case 6: strcpy(tok1->tokenType, "T_NUM"); break;
		case 11: strcpy(tok1->tokenType, "T_RNUM"); break;
		case 12: strcpy(tok1->tokenType, "T_PLUS"); break;
		case 13: strcpy(tok1->tokenType, "T_MINUS"); break;
		case 14: strcpy(tok1->tokenType, "T_DIV"); break;
		case 16: strcpy(tok1->tokenType, "T_MUL"); break;
		case 21: strcpy(tok1->tokenType, "T_LT"); break;
		case 22: strcpy(tok1->tokenType, "T_LE"); break;
		case 24: strcpy(tok1->tokenType,"T_DEF"); break;
		case 25: strcpy(tok1->tokenType, "T_DRIVERDEF"); break;
		case 27: strcpy(tok1->tokenType, "T_GT"); break;
		case 28: strcpy(tok1->tokenType, "T_GE"); break;
		case 30: strcpy(tok1->tokenType,"T_ENDDEF"); break;
		case 31: strcpy(tok1->tokenType, "T_DRIVERENDDEF"); break;
		case 33: strcpy(tok1->tokenType, "T_EQ"); break;
		case 35: strcpy(tok1->tokenType, "T_NE"); break;
		case 37: strcpy(tok1->tokenType, "T_RANGEOP"); break;
		case 39: strcpy(tok1->tokenType, "T_COLON"); break;
		case 40: strcpy(tok1->tokenType, "T_ASSIGNOP"); break;
		case 41: strcpy(tok1->tokenType, "T_COMMA"); break;
		case 42: strcpy(tok1->tokenType, "T_SEMICOL"); break;
		case 43: strcpy(tok1->tokenType, "T_SQBC"); break;
		case 44: strcpy(tok1->tokenType, "T_SQBO"); break;
		case 45: strcpy(tok1->tokenType, "T_BC"); break;
		case 46: strcpy(tok1->tokenType, "T_BO"); break;
		case 47: strcpy(tok1->tokenType, "T_ERROR"); break;
	}
	clearString(currStr,1000);
	currStrStart = 0;
	state = 0; // reverting state back to 0
	return tok1; //returns the populated token 
}

token getNextToken(){
	//works with twin buffers to process src file token by token

	while(1){
		switch(state){
			case 0: currStrStart = 0;

					if ((ipStr[lookahead] >= 'a' && ipStr[lookahead] <= 'z') || (ipStr[lookahead] >= 'A' && ipStr[lookahead] <= 'Z')){
						currStr[currStrStart] = ipStr[lookahead];
						currStrStart++;
						lookahead++;
						state = 1;
						//printf("here %d\n", lookahead );
						break;
					}
					else if (isspace(ipStr[lookahead]) && ipStr[lookahead] != '\n' ){
						lookahead++;
					}
					else if(ipStr[lookahead] >= '0' && ipStr[lookahead] <= '9'){
						currStr[currStrStart] = ipStr[lookahead];
						currStrStart++;
						lookahead++;
						state = 3;

					}
					else if (ipStr[lookahead] == '+'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						state = 12;
					}
					else if (ipStr[lookahead] == '-'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						state = 13;
					}
					else if (ipStr[lookahead] == '/'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						state = 14;
					}
					else if (ipStr[lookahead] == '*'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 15;
					}
					else if (ipStr[lookahead] =='<'){
						currStr[currStrStart]=ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 20;
					}
					else if (ipStr[lookahead] =='>'){
						currStr[currStrStart]=ipStr[lookahead];
						currStrStart++;
						lookahead++;
						state = 26;
					}
					else if (ipStr[lookahead] == '='){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 32;
					}
					else if (ipStr[lookahead] == '!'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 34;
					}
					else if (ipStr[lookahead] == '.'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 36;
					}
					else if (ipStr[lookahead] == ':'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 38;
					}
					else if (ipStr[lookahead] == ','){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 41;
					}
					else if (ipStr[lookahead] == ';'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 42;
					}
					else if (ipStr[lookahead] == ']'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 43;
					}
					else if (ipStr[lookahead] == '['){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 44;
					}
					else if (ipStr[lookahead] == ')'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 45;
					}
					else if (ipStr[lookahead] == '('){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 46;
					}
					else if (ipStr[lookahead] == '\n'){
						lookahead++;
						row++;
						break;

					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						state = 47;
						//lookahead++;
						//clearString(currStr,1000);
						//printf("should not be here %d\n", lookahead );

					}
					break;
			case 1: if ((ipStr[lookahead] >= 'a' && ipStr[lookahead] <= 'z') || (ipStr[lookahead] >= 'A' && ipStr[lookahead] <= 'Z') || (ipStr[lookahead] >= '0' && ipStr[lookahead] <= '9') || ipStr[lookahead] == '_'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						
					}
					else {
						state = 2;
						
					}
					break;
			case 2: return acceptState(currStr,state);
					break;
			case 3: if (ipStr[lookahead] >= '0' && ipStr[lookahead] <= '9'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						
					}
					else if (ipStr[lookahead] == '.'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 4;
						
					}
					else {
						state = 5;
						
					}
					break;
			case 4: if (ipStr[lookahead] == '.'){
						lookahead--;
						currStrStart--;
						currStr[currStrStart] = '\0';
						state = 6;
						
					}
					else if(ipStr[lookahead] >= '0' && ipStr[lookahead]<= '9') {
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 7;
						

					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						//clearString(currStr,1000);
						//lookahead++;
						state = 47;
						//printf("should not be here %d\n", lookahead );
						
					}
					break;
			case 5: return acceptState(currStr,state); break;
			case 6: return acceptState(currStr, state); break;
			case 7: if(ipStr[lookahead] >= '0' && ipStr[lookahead]<= '9'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						
					}
					else if (ipStr[lookahead] == 'e' || ipStr[lookahead] == 'E'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 8;
						
					}
					else {
						state = 11;
						
					}
					break;
			case 8: if (ipStr[lookahead] == '+' || ipStr[lookahead] == '-' ){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 9;
						
					}
					else if(ipStr[lookahead] >= '0' && ipStr[lookahead]<= '9'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 10;
						
					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						//clearString(currStr,1000);
						//lookahead++;
						state=47;
						//printf("should not be here %d\n", lookahead );
						
					}
					break;
			case 9: if(ipStr[lookahead] >= '0' && ipStr[lookahead]<= '9'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 10;
						
					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						//clearString(currStr,1000);
						//lookahead++;
						state = 47;
						//printf("should not be here %d\n", lookahead );
						
					}
					break;
			case 10:if(ipStr[lookahead] >= '0' && ipStr[lookahead]<= '9'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						
					}
					else {
						state = 11;
						
					}
					break;
			case 11: return acceptState(currStr,state); break;
			case 12: return acceptState(currStr, state); break;
			case 13: return acceptState(currStr, state); break;
			case 14: return acceptState(currStr, state); break;
			case 15: if (ipStr[lookahead] == '*'){
						clearString(currStr, 50);
						lookahead++;
						state = 17;

					}
					else {
						state = 16;
					}
					break;
			case 16: return acceptState(currStr, state); break;
			case 17: if (ipStr[lookahead] == '*'){
						lookahead++;
						state = 18;
					}
					else if(ipStr[lookahead] == '\n'){
						row++;
						lookahead++;
						//break;
					}
					else lookahead++;
					break;
			case 18: if (ipStr[lookahead] == '*'){
						lookahead++;
						state = 19;
					}
					else if(ipStr[lookahead]=='\n'){
						row++;
						lookahead++;
						state = 17;
						break;
					}
					else{
						lookahead++;
						state = 17;
						break;
					}
					break;
			case 19: //lookahead++; 
					state = 0; break;
			case 20: if (ipStr[lookahead] == '='){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 22;
					}
					else if (ipStr[lookahead] == '<'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 23;

					}
					else {
						state = 21;
					}
					break;
			case 21: return acceptState(currStr, state); break;
			case 22: return acceptState(currStr,state); break;
			case 23: if (ipStr[lookahead] == '<'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 25;

					}
					else {
						state = 24;
					}
					break;
			case 24: return acceptState(currStr, state); break;
			case 25: return acceptState(currStr,state); break;
			case 26: if (ipStr[lookahead] == '='){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 28;
					}
					else if (ipStr[lookahead] == '>'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 29;

					}
					else {
						state = 27;
					}
					break;
			case 27: return acceptState(currStr,state); break;
			case 28: return acceptState(currStr,state); break;
			case 29: if (ipStr[lookahead] == '>'){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 31;

					}
					else {
						state = 30;
					}
					break;
			case 30: return acceptState(currStr, state); break;
			case 31: return acceptState(currStr,state); break;
			case 32: if (ipStr[lookahead] == '='){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 33;
					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						//clearString(currStr,1000);
						//lookahead++;
						state = 47;
						//printf("should not be here %d\n", lookahead );
						
					}
					break;
			case 33: return acceptState(currStr,state); break;
			case 34: if (ipStr[lookahead] == '='){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 35;
					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						//clearString(currStr,1000);
						//lookahead++;
						state = 47;
						//printf("should not be here %d\n", lookahead );
						
					}
					break;
			case 35: return acceptState(currStr,state); break;
			case 36: if (ipStr[lookahead] == '.'){
						currStr[currStrStart] = ipStr[lookahead];
						//clearString(currStr,1000);
						lookahead++;
						currStrStart++;
						state = 37;
					}
					else {
						isError = 1;
						printf("ERROR_1: Unknown symbol in lexeme: \" %s \" at Line Number %d \n", currStr, row );
						//clearString(currStr,1000);
						//lookahead++;
						state = 47;
						//printf("should not be here %d\n", lookahead );
						
					}
					break;
			case 37: return acceptState(currStr,state); break;
			case 38: if (ipStr[lookahead] == '='){
						currStr[currStrStart] = ipStr[lookahead];
						lookahead++;
						currStrStart++;
						state = 40;
					}
					else {
						state = 39;
						
					}
					break;
			case 39: return acceptState(currStr,state); break;
			case 40: return acceptState(currStr, state); break;
			case 41: return acceptState(currStr, state); break;
			case 42: return acceptState(currStr, state); break;
			case 43: return acceptState(currStr, state); break;
			case 44: return acceptState(currStr, state); break;
			case 45: return acceptState(currStr, state); break;
			case 46: return acceptState(currStr, state); break;
			case 47: return acceptState(currStr, state); break;
		}
		if (ipStr[lookahead] == '\0') return NULL;

	}
	return NULL;
}

void getStream(FILE *fp){
	
	int t;

	isError = 0; // whether I detected an error
	state = 0; //initialise state to 0
	row = 1; // initialise start line number to 1
	currStrStart = 0; //initialise curr string buffer start to 0

	token ptr;

	clearString(ipStr,50000);

	while ((t=fread(ipStr,1,50000,fp))){
		lookahead = 0;
		while(ipStr[lookahead] != '\0'){
			ptr = getNextToken();
			if (ptr == NULL) continue;
			printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
		}

		clearString(ipStr,50000);

		if (strlen(currStr)>0){
			// printf("enter if\n");
			// printf("%d\n", state);
			// ptr = acceptState(currStr,state);
			// printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
			ptr = getNextToken();
			if (ptr == NULL) continue;
			printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
		}

	}

	// printf("%d ipstring length\n", strlen(ipStr) );
	// printf("%d currstr length\n", strlen(currStr) );

	if (strlen(currStr)>0){
			ptr = acceptState(currStr,state);
			printf("%d \t %s \t\t %s \n", ptr->rownum, ptr->tokenVal, ptr->tokenType);
		}

	printf(" done \n");	 

}


//prints code with removed comments onto console
void removeComments(char* inputFile){
	int line_no=1;
	FILE* sourceFile = fopen (inputFile, "r");

	if (sourceFile == NULL){
		printf ("File %s does not exist\n", inputFile);
	}

	//FILE* cleanFile = fopen (outputFile, "a+");

	char* line = (char*)calloc(512,sizeof(char));

	int j;
	int flag=0;

	while (fgets(line, 512, sourceFile)){

		if(line[0]=='\n' || strlen(line)==0){
			printf("%d\n",line_no);
			line_no++;
			continue;
		}

		if(line[strlen(line)-1]=='\n'){
			line[strlen(line)-1]='\0';
		}

		j=0;

		char* cleanLine = (char*)calloc(512,sizeof(char));
		
		for(int i=0; i<strlen(line); i++){ 

			if(flag == 0){
				if(line[i] == '*'){
					if(i+1 < strlen(line)){
						if(line[i+1] == '*'){
							flag=1;
							i++;
						}
						else{
							cleanLine[j++] = line[i];
						}
					}
					else{
						
						cleanLine[j++] = line[i];
					}
				}
				else{
					
					cleanLine[j++] = line[i];

				}
			}

			else if (flag == 1){
				if(line[i] == '*'){
					if(i+1 < strlen(line)){
						if(line[i+1] == '*'){
							flag=0;
							i++;
						}
						else{
							continue;						
						}
					}
					else{
						continue;
					}
				}
				else{
					continue;
				}
			}
		}
	
		if(strlen(cleanLine) == 0){
			printf("%d\n",line_no);
		}
		else{
			printf( "%d %s\n",line_no, cleanLine ); 
		}
		line_no++;
	}

	fclose(sourceFile);
	//fclose(cleanFile);

	return;
}
