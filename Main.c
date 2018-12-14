/*
Main file in Assignment 2: The Assembler
	
	File name:  Main.c
	programmer: Alexander Giammaruti
	Course:     COSC 4316
	Last edit:  March 2nd 2018
	Description: 
		This program is a two pass assembler which translates "Machine code"
		taken from a file named in the argument list into 32-bit binary machine 
		instructions encoded with the following format:
			-bits 0-15 hold the operand of the instruction
			-bits 16-20 hold the opcode
			-bits 21-31 are ignored and padded with zeros
		these instructions are output to a binary file "Output.bin" in Big-Endian format
			 
		to assemble ./arg010_ASM <filename>
*/

#include <stdio.h>
#include <string.h>
#include "ASM.h"
const char *opcodes[30] = {"HALT", "PUSH", "RVALUE", "LVALUE", "POP", "STO", "COPY", "ADD", "SUB", "MPY", "DIV", "MOD",
	     "NEG", "NOT", "OR", "AND", "EQ", "NE", "GT", "GE", "LT", "LE", "LABEL", "GOTO", "GOFALSE", 
	     "GOTRUE", "PRINT", "READ", "GOSUB", "RET"};
FILE *inFile;
FILE *outFile;

NodePtr root = NULL; 	//root of the symbol table

int main(int argc, char*argv[]){
	
	open_file(argc, argv);
	pass1(inFile, root);
	fclose(inFile);
	return 0;
}

unsigned long littleToBig(unsigned long x){
	return (((x >> 24) & 0x000000ff) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000)| ((x << 24) & 0xff000000)); 
}

void open_file(int argc, char *argv[]){
	inFile = NULL;
	if(argc > 1){
		if((inFile = fopen(argv[1], "r"))==NULL){
			fprintf(stderr, "ERROR: failed to open input file: %s", argv[1]);
		}
	}else{
		inFile = stdin;
	}	
}// end open_file

static int lookForOpcode(char *s){
	for(int i = 0; i < 30; i++){
		if(strcmp(s, opcodes[i])==0){
		return i;
		}
	}//end for
	return -1;
}// end lookForOpcode

static void insertCode(unsigned int opcode, unsigned long operand){
	operand += opcode;
	operand = littleToBig(operand);
	fwrite(&operand,4,1,outFile);

}// end insertCode

static void insertcode(int opcode){
	insertCode(opcode,0);
	
}// end insertcode

void pass1(FILE *inFile, NodePtr root/*, more...*/ ){
	int locationCounter = 0; 
	char line[40] = "", *lexeme; 
	do{
		fscanf(inFile, "%s ", line);
	}while(strcmp(line, "Section")!=0);	
	fscanf(inFile, "%s ", line);
	if(strcmp(line,".data")!=0){
		fprintf(stderr, "ERROR: Missing \"Section .data\" directive\n");
	}else{
		printf("Parsing data section, Pass 1\n");
	}// end if
	fgets(line, 40, inFile);
	lexeme = strtok(line, " \t\n");
	while(strcmp(lexeme, "Section")!=0 && lexeme != NULL){
		if(lexeme[strlen(lexeme)-1] != ':'){
			fprintf(stderr, "ERROR: expected \":\" proccessing line: %s\n", line);
		}else{
			lexeme[strlen(lexeme)-1]='\0';
			insert(&root, lexeme, "int", locationCounter);
			locationCounter++;
		}// end if

		//time to read another line
		fgets(line, 40, inFile);
		lexeme = strtok(line, " \t\n");
			
	}// end while (data section)
	if(lexeme == NULL){
		fprintf(stderr, "ERROR: Missing \"Section .code\" directive");

	}// end if

	
	printf("Parsing code section, pass 1\n");
	locationCounter = 0;
	while(fscanf(inFile, "%s", line)!=EOF){
		if(strcmp(line, "PUSH") == 0 || strcmp(line, "RVALUE") == 0 || 
		   strcmp(line, "LVALUE") == 0 || strcmp(line, "GOTO") == 0 ||
		   strcmp(line, "GOFALSE") == 0){
			fscanf(inFile, "%s", line);
			
		}
		if(strcmp(lexeme, "LABEL")==0){
			fscanf(inFile, "%s", line);
			insert(&root, line, "Code", locationCounter);	

		}// end if
		locationCounter++;

	}// end while (code section)
	
	//Move on to the second pass	
	rewind(inFile);
	pass2(inFile, root);
	
}// end pass1


void pass2(FILE *infile, NodePtr root){
	outFile = fopen("output.bin", "wb");
	int locationCounter =0;
	char line[40]; 
	int op = 0, found;
	do{
		fscanf(inFile, "%s", line);
	}while(strcmp(line, "Section")!=0);
	fscanf(inFile, "%s", line);
	if(strcmp(line,".data")!=0){
                fprintf(stderr, "ERROR: Missing 'Section .data' directive\n");
                exit(1);
        }else{
		printf("Parsing data section, pass 2\n");	
	}// end if
	do{
                fscanf(inFile, "%s", line);
        }while(strcmp(line, "Section")!=0); 
	fscanf(inFile, "%s", line);	
	//time to generate some code!
	printf("Parsing code section, pass 2\n");
	while(fscanf(inFile, "%s", line)!=EOF){
		op = lookForOpcode(line);
		switch(op){
			case 0:// HALT		
				insertcode(op); 
				break;
			case 1:// PUSH v	
				fscanf(inFile, "%s", line );
				insertCode(65536,  atoi(line));
				break;
			case 2:// RVALUE i 	
				fscanf(inFile, "%s", line );
                                found = search(root, line);
                                if(found != -1){  
                                        insertCode(131072, found);
                                }else{
                                        fprintf(stderr, "ERROR: Operand %s is undefined\n", line);
				}// end if
				break;
			case 3:// LVALUE i 	
				fscanf(inFile, "%s", line );
                                found = search(root, line);
                                if(found != -1){  
                                        insertCode(196608, found);
                                }else{
                                        fprintf(stderr, "ERROR: Operand %s is undefined\n", line);
				}// end if
				break;
			case 4:// POP 		
				insertcode(262144);
				break;
			case 5:// STO 		
				insertcode(327680);
				break;
			case 6:// COPY		
				insertcode(393216);
				break;
			case 7:// ADD		
				insertcode(458752);
				break;
			case 8:// SUB		
				insertcode(524288);
				break;
			case 9:// MPY		
				insertcode(589824);
				break;
			case 10:// DIV		
				insertcode(655360);
				break;
			case 11:// MOD		
				insertcode(720896);
				break;
			case 12:// NEG		
				insertcode(786432);
				break;
			case 13:// NOT		
				insertcode(851968);
				break;
			case 14:// OR		
				insertcode(917504);
				break;
			case 15:// AND		
				insertcode(983040);
				break;
			case 16:// EQ		
				insertcode(1048576);
				break;
			case 17:// NE		
				insertcode(1114112);
				break;
			case 18:// GT		
				insertcode(1179648);
				break;
			case 19:// GE		
				insertcode(1245184);
				break;
			case 20:// LT		
				insertcode(1310720);
				break;
			case 21:// LE		
				insertcode(1376256);
				break;
			case 22:// LABEL n	
				fscanf(inFile, "%s", line );
				insertcode(1441792);
				break;
			case 23:// GOTO n	
				fscanf(inFile, "%s", line );
                                found = search(root, line);
                                if(found != -1){
                                        insertCode(1507328, found);
                                }else{
                                        fprintf(stderr, "ERROR: Operand %s undefined\n", line);
                                }// end if
				break;
			case 24:// GOFALSE n	
				fscanf(inFile, "%s", line );
                                found = search(root, line);
                                if(found != -1){
                                        insertCode(1572864, found);
                                }else{
                                        fprintf(stderr, "ERROR: Operand %s is undefined\n", line);
                                }// end if
				break;
			case 25:// GOTRUE n	
				fscanf(inFile, "%s", line );
                                found = search(root, line);
                                if(found != -1){
                                        insertCode(1638400, found);
                                }else{
                                        fprintf(stderr, "ERROR: Operand %s is undefined\n", line);
                                }// end if
				break;
			case 26:// PRINT	
				insertcode(1703936);
				break;
			case 27:// READ		
				insertcode(1769472);
				break;
			case 28:// GOSUB i	
				fscanf(inFile, "%s", line );
                                insertCode(1835008,  atoi(line));
				break;
			case 29:// RET		
				insertcode(1900544);
				break;
			default:
				fprintf(stderr, "Invalid opcode processed: %s\n", line);
		}// end case
		locationCounter++;
	}// end while
	fclose(outFile);
	dropTable(&root);	
}// end pass 2
