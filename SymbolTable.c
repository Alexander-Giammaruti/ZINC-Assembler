#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct node{
	char lexeme[20];
	char type[10];
	int address;
	struct node *left, *right;
};

typedef struct node *NodePtr;

struct node *newNode(char *lex, char *typ, int addr){
	struct node *temp = malloc(sizeof(struct node));
	strcpy(temp->lexeme, lex);
	strcpy(temp->type, typ);
	temp->address = addr;
	temp->left = temp->right = NULL;
	return temp;
}

void insert(NodePtr *Pt, char *key, char *typ, int addr){
	if(*Pt == NULL){
		*Pt = newNode(key, typ, addr);
		return;
	}
	if(strcmp(key, (*Pt)->lexeme) < 0){
		if((*Pt)->left == NULL){
			(*Pt)->left = newNode(key, typ, addr);
		}else{
			insert(&(*Pt)->left, key, typ, addr);
		}
	}else if(strcmp(key, (*Pt)->lexeme) > 0){
		if((*Pt)->right == NULL){
			(*Pt)->right = newNode(key, typ, addr);
		}else{
			insert(&(*Pt)->right, key, typ, addr);
		}
	}else{
		fprintf(stderr, "ERROR: The label, %s, is multiply-defined\n", key);
		exit(1);
	}
}

void dropTable(NodePtr *node){
	if((*node) == NULL){
		fprintf(stderr, "ERROR: Symbol table is unpopulated!\n");
		return;
	}
	if((*node)->left != NULL){
		dropTable(&(*node)->left);
	}
	if((*node)->right != NULL){
		dropTable(&(*node)->right);
	}
	
	free((*node));
}	

void inOrder(NodePtr Pt){
	if(Pt!=NULL){
		if(Pt->left != NULL) inOrder(Pt->left);
		printf("%s %d\n", Pt->lexeme, Pt->address);
		if(Pt->right != NULL) inOrder(Pt->right);
	}
}

int search(NodePtr root, char *key){
	
	if(root == NULL){
		fprintf(stderr, "ERROR: Symbol table is unpopulated!\n");
		return -1;
	}
	NodePtr Pt = root;
	if(strcmp(key, Pt->lexeme) < 0){
		if(Pt->left != NULL){
			search(Pt->left, key);

		}else{
			fprintf(stderr, "ERROR: Lexeme %s not found in table", key);
			return -1;
		}
	}else if(strcmp(key, Pt->lexeme) > 0){
		if(Pt->right != NULL){
			search(Pt->right, key);
		}else{
			fprintf(stderr, "ERROR: Lexeme %s not found in table", key);
			return -1;
		}
	}else{	//we found it!
		return(Pt->address);		
	}
}
