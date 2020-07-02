//Group-15
//Dishita Malav 2017A7PS0164P
//Shaily Bhatt 2017A7PS0040P
//Shefali Tripathi 2017A7PS0139P

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "parserDef.h"
#include "ast.h"
#include "symbolTable.h"
#include "semanticAnalyser.h"
#include "codeGen.h"

int main(int argc, char const *argv[]){
	int response;

	// ptnode parse_root = NULL;
	// astNode ast_root = NULL;

	// p temp;
	// temp.func = NULL;

	char srcfilename[1000];
	char outputfilename[1000];

	// clock_t start_time, end_time;
	// double total_CPU_time, total_CPU_time_in_seconds;

	strcpy(srcfilename,argv[1]);
	strcpy(outputfilename,argv[2]);

	FILE *fp;
	fp = fopen(srcfilename,"r");

	printf("LEVEL 4: LERXER/ PARSER/ AST GENERATION/ SYMBOL TABLE / TYPE CHECKING/ SEMANTIC RULES CHECKING\n" );

	do{

		// parse_root = NULL;
		// ast_root = NULL;
		// temp.func = NULL;
		printf("******************************************************\n\nENTER DRIVER\n Input the option number corresponding to functionality that you want to test: \n\n");

		printf("0: Exit\n");
		printf("1: For generating token list be lexer and printing it.\n");
		printf("2: For verifying syntactic correctness and printing parsetree.\n");
		printf("3: For generating and printing Abstract Syntax Tree\n");
		printf("4: For printing number of nodes, memory allocated and compression ration\n");
		printf("5: For generating symbol table and printing it\n");
		printf("6: For printing the size of activation records for each function\n");
		printf("7: For printing type expressions and width of array variables\n");
		printf("8: For verifying syntactic and semantic correctness\n");
		printf("9: For producing assembly code\n");


		scanf("%d", &response);

		if(response == 0) return 0;
		else if(response == 1){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();
			getStream(fp);
			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("LEXER DONE\n");
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);
		}
		else if (response == 2){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();

			if(isError == 1){
				printf("Cannot parse a code with lexical errors.\n");
				return 0;
			}

			ptnode parse_root = parser_main(srcfilename);

			if(syntaxErr>0){
				printf("Parsing incorrect cannot print parse tree\n");
				return 0;
			}

			print_parseTree(parse_root);

			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("PARSER DONE\n");
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);

		}

		else if(response == 3){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();

			if(isError == 1){
				printf("Cannot parse a code with lexical errors.\n");
				return 0;
			}

			ptnode parse_root = parser_main(srcfilename);

			if(syntaxErr>0){
				printf("Parsing incorrect cannot build AST\n");
				return 0;
			}

			astNode ast_root = ast_main(parse_root);

			printf("AST printing format:\nTraversal: Print the root followed by Depth First Traveral of sub-tree\nTag_of_ASTnode: Tag_of_AST_parent - corresponding_terminal/non-terminal (subject to existence)");
			printf("NOW printing\n");
			print_ast(ast_root);
			printf("AST done\n");
			

			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);
		}

		else if(response == 4){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();

			if(isError == 1){
				printf("Cannot parse a code with lexical errors.\n");
				return 0;
			}

			ptnode parse_root = parser_main(srcfilename);

			if(syntaxErr>0){
				printf("Parsing incorrect cannot build AST\n");
				return 0;
			}

			astNode ast_root = ast_main(parse_root);

			printf("Number of Parse Tree Nodes = %d\n", PARSETREE_SIZE);
			int ptmem = PARSETREE_SIZE*(sizeof(struct parseTreeNode));
			printf("Allocated memory to parse tree = %d\n", ptmem);

			int astmem = AST_SIZE*(sizeof(struct astStruct));
			printf("Number of Abstrat Syntax Tree Nodes = %d\n", AST_SIZE);
			printf("Allocated memory to ast = %d\n", astmem);

			double cr = (ptmem - astmem)*100/ptmem;
			printf("Compression Ratio = %lf\n", cr);

			

			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);

		}
		else if(response == 5){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();

			ptnode parse_root = parser_main(srcfilename);
			if(syntaxErr > 0){
				printf("Cannot construct AST for syntactically incorrect code\n Hence cant generate symbol table also\n");
				return 0;
			}
			astNode ast_root = ast_main(parse_root);
			printf("AST DONE\n");
			createSymbolTable(ast_root, false);
			printf("GENERATION OF SYMBOL TABLE DONE\n");
			printSymbolTable(globalSymTable);
			printDriver();

			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);
		}
		else if(response == 6){
			ptnode parse_root = parser_main(srcfilename);
			if(syntaxErr > 0){
				printf("code is syntactically incorrect so cannot generate symbol table and determine size of activation records\n");
				return 0;
			}
			astNode ast_root = ast_main(parse_root);
			printf("AST DONE\n");
			createSymbolTable(ast_root, false);
			printf("GENERATION OF SYMBOL TABLE DONE\n");
			printFunctions();

		}
		else if(response == 7){

			ptnode parse_root = parser_main(srcfilename);
			if(syntaxErr > 0){
				printf("Code is syntactically incorrect so cannot construct AST and symbol table hence cannot print info about arrays\n");
				return 0;
			}
			astNode ast_root = ast_main(parse_root);
			printf("AST DONE\n");
			createSymbolTable(ast_root, false);
			printf("GENERATION OF SYMBOL TABLE DONE\n");
			printArrays(globalSymTable);

		}
		else if(response == 8){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();

			ptnode parse_root = parser_main(srcfilename);
			if(syntaxErr > 0){
				printf("Code is syntactically incorrect so cannot check for semantic errors\n");
				break;
			}
			astNode ast_root = ast_main(parse_root);
			printf("AST DONE\n");
			createSymbolTable(ast_root, true);
			p temp;
			temp.func = NULL;
			checkSemantics(ast_root, &symbolTableErrors, temp, -1);
			printf("SEMANTIC CHECKING DONE\n");

			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);
		}

		else if(response ==9){
			clock_t start_time, end_time;
			double total_CPU_time, total_CPU_time_in_seconds;
			start_time = clock();

			ptnode parse_root = parser_main(srcfilename);
			if(syntaxErr > 0){
				printf("Code is syntactically incorrect so cannot check for semantic errors\n");
				break;
			}
			astNode ast_root = ast_main(parse_root);
			printf("AST DONE\n");
			createSymbolTable(ast_root, true);
			p temp;
			temp.func = NULL;
			checkSemantics(ast_root, &symbolTableErrors, temp, -1);
			printf("SEMANTIC CHECKING DONE\n");

			if(symbolTableErrors>0){
				printf("Code is semantically incorrect so cannot generate code\n");
				break;
			}

			FILE *fout;
			fout = fopen(outputfilename, "w");

			fprintf(fout, "SECTION .bss\n\tmyBuffer: resb 18\n\tmyBufSize: equ $-myBuffer\n" );
			fprintf(fout,"leftOp: RESW 8\nfor_temp: RESW 8\nOUT: RESW 10\ntemp: RESW 1\nrightOp: RESW 1\n");

			doCodeGen(fout, ast_root); //symbol table is globally declared

			printf("DONE CODE GEN\n");

			end_time = clock();					
			total_CPU_time = (double) (end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time/CLOCKS_PER_SEC;
			printf("Total CPU Time: %lf\n", total_CPU_time);
			printf("Total CPU Time in seconds: %lf\n", total_CPU_time_in_seconds);

			printf("\n\nTo run use commands\n\t>nasm -f elf -o code.o code.asm\n\t>ld -m elf_i386 -s -o code code.o\n\t>./code\n\n PLEASE NOTE: We were unable to incorporate string output on console through ASM. Hence, true is printed as 1, False is printed as 0. Also, \"input\" is not printed, cursor simply waits for input\n\n\n");

		}
		else {
			printf("Wrong Choice \n");
			return 0;
		}

		for (int x =0; x<TABLESIZE; x++){
			globalSymTable[x] = NULL;
		}

	}while(response != 0);

}