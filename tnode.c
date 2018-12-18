#include "tnode.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//check out tndode display!

struct tnode
{
	void *value;
	TNODE *parent;
	TNODE *left;
	TNODE *right;
	int debugLevel;

	void (*TNODEdisplay)(void *, FILE *);		
	void (*TNODEfree)(void *);
};


TNODE *newTNODE(void *value, TNODE *left, TNODE *right, TNODE *parent)
{
	TNODE *p = malloc(sizeof(TNODE));
	if (p == 0) { fprintf(stderr, "out of memory\n"); exit(1); }

	p->value = value;
	p->parent = parent;
	p->left = left;
	p->right = right;
	p->debugLevel = 0;
	p->TNODEdisplay = 0;
	p->TNODEfree = 0;
	return p;
}


void setTNODEdisplay(TNODE *n, void (*displayTNODE)(void *, FILE *))
{
	n->TNODEdisplay = displayTNODE;
	return;
}

void setTNODEfree(TNODE *n, void (*freeTNODE)(void *))
{
	n->TNODEfree = freeTNODE;
	return;
}

void *getTNODEvalue(TNODE *n)
{
	return n->value;
}

void setTNODEvalue(TNODE *n,void *replacement)
{
	n->value = replacement;
	return;
}

TNODE *getTNODEleft(TNODE *n)
{
	return n->left;
}

void setTNODEleft(TNODE *n,TNODE *replacement)
{
	n->left = replacement;
	return;
}

TNODE *getTNODEright(TNODE *n)
{
	return n->right;
}

void setTNODEright(TNODE *n,TNODE *replacement)
{
	n->right = replacement;
	return;
}

TNODE *getTNODEparent(TNODE *n)
{
	return n->parent;
}

void setTNODEparent(TNODE *n,TNODE *replacement)
{
	n->parent = replacement;
	return;
}

void displayTNODE(TNODE *n,FILE *fp)
{
	if (n->TNODEdisplay)
	{
		//normal print
		//fprintf(stdout,"debuglvlTNODE: %d\n\n",n->debugLevel);
		if (n->debugLevel == 0)
		{
			n->TNODEdisplay(getTNODEvalue(n),fp);
		}
		else
		{
			n->TNODEdisplay(getTNODEvalue(n),fp);
			fprintf(fp,"&%p",getTNODEvalue(n));
		}
	}
	else
	{
		fprintf(fp,"&%p",getTNODEvalue(n));
	}

	//fprintf(fp,"\n");   //maybe dony do this
	return;
}

int debugTNODE(TNODE *n,int level)
{
	int temp = n->debugLevel;
	n->debugLevel = level;
	return temp;
}

void freeTNODE(TNODE *n)
{
	if (n->TNODEfree)
	{
		n->TNODEfree(getTNODEvalue(n));
	}

	free(n);
	return;
}