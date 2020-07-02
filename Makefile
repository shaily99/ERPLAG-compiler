all: 
	gcc -g -o compiler driver.c symbolTable.c ast.c parser.c lexer.c semanticAnalyser.c codeGen.c
