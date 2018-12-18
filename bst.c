#include "tnode.h"
#include "bst.h"
#include "queue.h"
//#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct bst 
{
	int size;
	TNODE *root;
	int debug;

	int (*compare)(void *,void *);
	void (*BSTdisplay)(void *, FILE *);
	void (*BSTswapper)(TNODE *, TNODE *);
	void (*BSTfree)(void *);
};

int minTreeHeight(TNODE *);
int maxTreeHeight(TNODE *);
void swapperFunction(TNODE *, TNODE *);
void displayPreOrder(BST *t,TNODE *n,FILE *fp);
void displayPostOrder(BST *t,TNODE *n,FILE *fp);
void displayInOrder(BST *t, TNODE *n,FILE *fp);
void freeBSTloop(BST *t, TNODE *n);





BST *newBST(int (*c)(void *,void *))
{
	BST *tree = malloc(sizeof(BST));
	if (tree == 0) { fprintf(stderr, "out of memory\n"); exit(1); }

	tree->size = 0;
	tree->debug = 0;
	tree->root = NULL;
	tree->BSTfree = NULL;
	tree->compare = c;
	tree->BSTswapper = swapperFunction;

	return tree;
}

void setBSTdisplay(BST *t, void (*d)(void *, FILE *))
{
	t->BSTdisplay = d;
	return;
}

void setBSTswapper(BST *t, void (*swapperBST)(TNODE *, TNODE *))
{
	t->BSTswapper = swapperBST;
	return;
}

void setBSTfree(BST *t, void (*freeBST)(void *))
{
	t->BSTfree = freeBST;
	return;
}

TNODE *getBSTroot(BST *t)
{
	return t->root;
}

void setBSTroot(BST *t,TNODE *replacement)
{
	t->root = replacement;
	if (t->root != NULL)				//check this
	{
		setTNODEparent(replacement,replacement);
	}
	return;
}

void setBSTsize(BST *t,int s)
{
	t->size = s;
	return;
}

TNODE *insertBST(BST *r,void *value)
{
	TNODE *y = NULL;
    TNODE *x = r->root;
    TNODE *n = newTNODE(value,NULL,NULL,NULL);
    setTNODEdisplay(n,r->BSTdisplay);

    while (x != NULL) {
        y = x;
        if (r->compare(getTNODEvalue(n), getTNODEvalue(x)) < 0)
            x = getTNODEleft(x);
        else
            x = getTNODEright(x);
    }
    setTNODEparent(n,y);
    r->size++;

    //empty tree
    if (y == NULL) {
        setBSTroot(r,n);
    }
    else if (r->compare(getTNODEvalue(n), getTNODEvalue(y)) < 0)
        setTNODEleft(y,n);
    else
        setTNODEright(y,n);
    return n;
}

void *findBST(BST *t, void *key)
{
	if (t->root == NULL)
	{
        return NULL;
	}

	void *foundValue = NULL;
    TNODE *n = t->root;

    while (n != NULL) {
        if (t->compare(key, getTNODEvalue(n)) == 0) {
            foundValue = getTNODEvalue(n);
            break;
        }
        else if (t->compare(key, getTNODEvalue(n)) < 0)
            n = getTNODEleft(n);
        else
            n = getTNODEright(n); 
    }

	return foundValue;
}

TNODE *locateBST(BST *t, void *key)
{
	if (t->root == NULL)
	{
        return NULL;
	}

    TNODE *n = t->root;

    while (n != NULL) {
        if (t->compare(key, getTNODEvalue(n)) == 0) {
            return n;
        }
        else if (t->compare(key, getTNODEvalue(n)) < 0)
            n = getTNODEleft(n);
        else
            n = getTNODEright(n);
    }

	return NULL;

}

int deleteBST(BST *t, void *key)
{
	if(locateBST(t,key))
	{
		TNODE *foundNode = locateBST(t,key);
		foundNode = swapToLeafBST(t,foundNode);
		pruneLeafBST(t,foundNode);
		freeTNODE(foundNode);

		//setBSTsize(t,sizeBST(t)-1);
		t->size--;
		return 0;
	}
	else
	{
		return -1;
	}
}

TNODE *swapToLeafBST(BST *t, TNODE *node)
{
	if (getTNODEleft(node) == NULL && getTNODEright(node) == NULL)
	{
		return node;
	}

	else 
	{
		TNODE *newNode = NULL;

		if (getTNODEleft(node) != NULL)
		{
			newNode = getTNODEleft(node);
			while (getTNODEright(newNode) != NULL)
			{
				newNode = getTNODEright(newNode);
			}
		}

		else
		{
			newNode = getTNODEright(node);
			while(getTNODEleft(newNode) != NULL)
			{
				newNode = getTNODEleft(newNode);
			}
		}

		t->BSTswapper(node,newNode);
		return swapToLeafBST(t,newNode);
	}
}

void pruneLeafBST(BST *t, TNODE *leaf)
{
	if (getBSTroot(t) == leaf)
	{
		leaf = NULL;			//DO SOMETHING ELSE HERE PROBABLY
		setBSTroot(t,leaf);
	}

	else
	{
		if (getTNODEright(getTNODEparent(leaf)) == leaf)
		{
			setTNODEright(getTNODEparent(leaf),NULL);
		}

		else
		{
			setTNODEleft(getTNODEparent(leaf),NULL);
		}
	}
	return;
}


int sizeBST(BST *t)
{
	return t->size;
}


void statisticsBST(BST *t, FILE *fp)
{
	fprintf(fp,"Nodes: %d\n",sizeBST(t));
	fprintf(fp,"Minimum depth: %d\n",minTreeHeight(getBSTroot(t))-1);
	fprintf(fp,"Maximum depth: %d\n",maxTreeHeight(getBSTroot(t)));
	return;
}



void displayBST(BST *t,FILE *fp)
{	
	if(t->debug == 0)
	{
		if (getBSTroot(t) == NULL)
		{
			fprintf(fp,"0:\n");
			return;
		}

		int level = 0;
		TNODE *n;
		QUEUE *q = newQUEUE();

		enqueue(q,getBSTroot(t));
		enqueue(q,NULL);

		fprintf(fp, "%d: ",level);

		while(sizeQUEUE(q) > 0)
		{
			n = dequeue(q);

			//if NULL about to start new level
			if (sizeQUEUE(q) == 0)
			{
				fprintf(fp,"\n");
				break;
			}

			else if (n == NULL)
			{
				fprintf(fp, "\n");
				enqueue(q,NULL);
				level++;
				if (sizeQUEUE(q) != 1)
				{
					fprintf(fp, "%d: ",level);
				}
			}
			else
			{										
				//if it is a leaf put =
				if (getTNODEleft(n) == NULL && getTNODEright(n) == NULL)
				{
					fprintf(fp,"=");
				}

				//t->BSTdisplay(getTNODEvalue(n),fp);
				displayTNODE(n,fp);
				fprintf(fp,"(");
				//t->BSTdisplay(getTNODEvalue(getTNODEparent(n)),fp);
				displayTNODE(getTNODEparent(n),fp);
				fprintf(fp,")");

				if (t->compare(getTNODEvalue(n), getTNODEvalue(getTNODEparent(n))) < 0)
				{
					fprintf(fp,"L");
				}
				else if (t->compare(getTNODEvalue(n), getTNODEvalue(getTNODEparent(n))) > 0)
				{
					fprintf(fp,"R");
				}
				else
				{
					fprintf(fp,"X");
				}

				if (peekQUEUE(q) != NULL)
				{
					fprintf(fp," ");
				}
				if (getTNODEleft(n) != NULL)
				{
					enqueue(q, getTNODEleft(n));
				}
				if (getTNODEright(n) != NULL)
				{
					enqueue(q, getTNODEright(n));
				}
			}
		}
		freeQUEUE(q);
	}
	else if (t->debug == 1)
	{
		if (getBSTroot(t) == NULL)
		{
			fprintf(fp,"[]");
			return;
		}
		else
		{
			displayInOrder(t,getBSTroot(t),fp);
			fprintf(fp,"]");
		}
	}
	else if (t->debug == 2)
	{
		if (getBSTroot(t) == NULL)
		{
			fprintf(fp,"[]");
			return;
		}
		else
		{
			displayPreOrder(t,getBSTroot(t),fp);
			fprintf(fp,"]");
		}
	}
	else if (t->debug == 3)
	{
		if (getBSTroot(t) == NULL)
		{
			fprintf(fp,"[]");
			return;
		}
		else
		{
			displayPostOrder(t,getBSTroot(t),fp);
			fprintf(fp,"]");
		}

	}
	return;
}






int debugBST(BST *t, int level)
{
	int temp = t->debug;
	t->debug = level;
	return temp;
}

void freeBST(BST *t)	//fix
{
	freeBSTloop(t, getBSTroot(t));
	free(t);
	return;
}



int minTreeHeight(TNODE *n)
{
	if (n == NULL)
	{
		return 0;
	}

	if (getTNODEleft(n) == NULL && getTNODEright(n) == NULL)
	{
		return 1;
	}

	int heightLeft = minTreeHeight(getTNODEleft(n)) + 1;
	int heightRight = minTreeHeight(getTNODEright(n)) + 1;
	
	if (heightLeft < heightRight)
	{
		return heightLeft;
	}
	else
	{
		return heightRight;
	}
}

int maxTreeHeight(TNODE *n)
{
	if (n == NULL)
	{
		return -1;
	}
	else
	{
		int heightLeft = maxTreeHeight(getTNODEleft(n));
		int heightRight = maxTreeHeight(getTNODEright(n));

		if (heightRight > heightLeft)
		{
			return heightRight + 1;
		}
		else
		{
			return heightLeft + 1;
		}
	}
}

void swapperFunction(TNODE *oldNode, TNODE *newNode)
{
	void *temp = getTNODEvalue(oldNode);
	setTNODEvalue(oldNode,getTNODEvalue(newNode));
	setTNODEvalue(newNode,temp);
	return;
}


void displayPreOrder(BST *t,TNODE *n,FILE *fp)
{
	fprintf(fp, "[");

	if (n == NULL)
	{
		return;
	}

	displayTNODE(n,fp);
							//THIS SHOUKD BE ||
	if (getTNODEleft(n) != NULL || getTNODEright(n) != NULL)
	{
		fprintf(fp," ");
	}

	if (getTNODEleft(n) != NULL)
	{
		displayPreOrder(t,getTNODEleft(n),fp);
		fprintf(fp, "]");

		if (getTNODEright(n) != NULL)
		{
			fprintf(fp," ");
		}
	}


	if (getTNODEright(n) != NULL)
	{
		displayPreOrder(t,getTNODEright(n),fp);
		fprintf(fp, "]");
	}
}

void displayPostOrder(BST *t, TNODE *n,FILE *fp)
{
	fprintf(fp, "[");

	if (n == NULL)
	{
		return;
	}
	//fprintf(fp, "[");

	if (getTNODEleft(n) != NULL)
	{
		displayPostOrder(t,getTNODEleft(n),fp);
			fprintf(fp, "] ");
	}


	if (getTNODEright(n) != NULL)
	{
		displayPostOrder(t,getTNODEright(n),fp);
		//fprintf(fp, "]");
	}

	if (getTNODEright(n) != NULL)
	{
		//displayPostOrder(t,getTNODEright(n),fp);
		fprintf(fp, "]");
	}

	if (getTNODEright(n) != NULL)
	{
		fprintf(fp," ");
	}

	displayTNODE(n,fp);
}



void displayInOrder(BST *t, TNODE *n,FILE *fp)
{
	fprintf(fp, "[");

	if (t == NULL)
	{
		return;
	};

	if (getTNODEleft(n) != NULL)
	{
		displayInOrder(t,getTNODEleft(n),fp);
		fprintf(fp,"] ");
	}

	displayTNODE(n,fp);

	if (getTNODEright(n) != NULL)
	{
		fprintf(fp," ");
	}


	if (getTNODEright(n) != NULL)
	{
		displayInOrder(t,getTNODEright(n),fp);
		fprintf(fp, "]");
	}
}


void freeBSTloop(BST *t, TNODE *n)
{
	if (n != NULL)
	{
		freeBSTloop(t,getTNODEleft(n));
		freeBSTloop(t,getTNODEright(n));
		setTNODEfree(n, t->BSTfree);	//t->BSTfree takes in a GVal
		freeTNODE(n);					//but we are passing a tnode
	}
}




