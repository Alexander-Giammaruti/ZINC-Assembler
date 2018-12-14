#ifndef _ASM_H
#define _ASM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define NULLSTRING ""

struct node{
	char lexeme[20];
	char type[20];
	int address;
	struct node *left, *right;
};

typedef struct node *NodePtr;
void insert(NodePtr *Pt, char *key, char *typ, int addr);
void dropTable(NodePtr *node);
int search(NodePtr Pt, char *key);
void inOrder(NodePtr Pt);

void open_file(int argc, char *argv[]);
void open_file(int argc, char *argv[]);
void pass1(FILE *inFile, NodePtr root);
void pass2(FILE *inFile, NodePtr root);
#endif
