#include "tnode.h"
#include "bst.h"
#include "gst.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct gst 
{
	BST *tree;
	int size;

	int (*GSTcompare)(void *,void *);
	void (*GSTdisplay)(void *, FILE *);
	void (*GSTswapper)(TNODE *, TNODE *);
	void (*GSTfree)(void *);
};


typedef struct gstValue 
{
	void *value;
	int freq;
	void (*display)(void *,FILE *);
	int (*compare)(void *, void *);
	void (*free)(void *);
} GSTVALUE;


GSTVALUE *newGSTvalue(int (*c)(void *, void *), void (*d)(void *,FILE *),void (*f)(void *));
void swapper(TNODE *a,TNODE *b);
int compareGSTValue(void *a, void *b);
void displayGSTValue(void *value, FILE *fp);
void freeGSTValue(void *v);




GST *newGST(int (*c)(void *, void *))	//pass in integer compare.
{										//then set bst compare to the actual gst one
	GST *g = malloc(sizeof(GST));
	if (g == 0) { fprintf(stderr, "out of memory\n"); exit(1); }


	g->GSTcompare = c;
	g->tree = newBST(compareGSTValue);
	g->size = 0;	
	g->GSTdisplay = NULL;			//this is set in setGSTDisplay
	g->GSTswapper = swapper;
	g->GSTfree = NULL;				//this is set in setGSTfree


	return g;
}

GSTVALUE *newGSTvalue(int (*c)(void *, void *), void (*d)(void *,FILE *fp),	void (*f)(void *))
{
	GSTVALUE *gstValue = malloc(sizeof(GSTVALUE));
	if (gstValue == 0) { fprintf(stderr, "out of memory\n"); exit(1); }

	gstValue->value = NULL;
	gstValue->freq = 1;
	gstValue->display = d;			//these are called in inser and take the trees compare and display
	gstValue->compare = c;
	gstValue->free = f;			//maybe change


	return gstValue;
}

void setGSTdisplay(GST *g, void (*d)(void *, FILE *))
{
	g->GSTdisplay = d;
	setBSTdisplay(g->tree,displayGSTValue);
	return;
}

void setGSTswapper(GST *g, void (*s)(TNODE *, TNODE *))
{
	g->GSTswapper = s;
	setBSTswapper(g->tree,s);		//maybe dont do this
	return;
}

void setGSTfree(GST *g, void (*f)(void *))
{
	g->GSTfree = f;
	setBSTfree(g->tree,freeGSTValue);
	return;
}

TNODE *getGSTroot(GST *g)
{
	return getBSTroot(g->tree);
}

void setGSTroot(GST *g,TNODE *replacement)
{
	setBSTroot(g->tree,replacement);
	return;
}

void setGSTsize(GST *g,int s)
{
	//g->size = s;
	//s = sizeBST(g->tree)-1;
	setBSTsize(g->tree,s);
}

TNODE *insertGST(GST *g,void *value)
{
	g->size++;
	
	TNODE *n = locateGST(g,value);

	if (n == NULL)
	{
		GSTVALUE *gstValue = newGSTvalue(g->GSTcompare,g->GSTdisplay,g->GSTfree);
		gstValue->value = value;
		return insertBST(g->tree,gstValue);
	}
	else
	{
		//GSTVALUE *gstValue = findGST(g,value);
		GSTVALUE *gstValue = getTNODEvalue(locateGST(g,value));

		

		gstValue->freq++;

		if (g->GSTfree != NULL)
		{
			g->GSTfree(value);
		}
		return NULL;
	}
}

void *findGST(GST *g,void *key)
{
	GSTVALUE *x = newGSTvalue(g->GSTcompare,g->GSTdisplay,g->GSTfree);
	x->value = key;
	GSTVALUE *tn = findBST(g->tree,x);
	free(x);
	if (tn == NULL)
		return NULL;
	return tn->value; //returns 7
}

TNODE *locateGST(GST *g,void *key)
{
	GSTVALUE *x = newGSTvalue(g->GSTcompare,g->GSTdisplay,g->GSTfree);
	x->value = key;

	TNODE *tn = locateBST(g->tree,x);
	free(x);
	return tn;
}

int deleteGST(GST *t,void *key)
{
	TNODE *n = locateGST(t,key);

	if (n == NULL)
	{
		return -1;
	}
	else
	{
		t->size--;
		//GSTVALUE *gstValue = findGST(t,key);
		GSTVALUE *gstValue = getTNODEvalue(locateGST(t,key));


		if(gstValue->freq > 1)
		{
			gstValue->freq--;
			return gstValue->freq;
		}
		else
		{
			deleteBST(t->tree,gstValue);
			free(gstValue);
			return 0;
		}
	}
}

TNODE *swapToLeafGST(GST *t,TNODE *node)
{
	return swapToLeafBST(t->tree,node);
}

void pruneLeafGST(GST *t,TNODE *leaf)
{	
	t->size--;
	pruneLeafBST(t->tree,leaf);
	GSTVALUE *gstValue = getTNODEvalue(leaf);
	free(gstValue);
}

int sizeGST(GST *g)
{
	return sizeBST(g->tree);
}

void statisticsGST(GST *t, FILE *fp)
{
	fprintf(fp,"Duplicates: %d\n",duplicatesGST(t));
	statisticsBST(t->tree,fp);
	return;
}


void displayGST(GST *t,FILE *fp)
{
	displayBST(t->tree,fp);
	return;
}

int debugGST(GST *t,int level)
{
	return debugBST(t->tree,level);
}

void freeGST(GST *t)
{
	freeBST(t->tree);
	free(t);
	return;
}





void *unwrapGST(TNODE *n)
{
	GSTVALUE *gstValue = getTNODEvalue(n);
	return gstValue->value;
}

int freqGST(GST *g,void *key) ///wrong
{
	TNODE *n = locateGST(g,key);
	if (n == NULL)
	{
		return 0;
	}
	else
	{
		GSTVALUE *gv = getTNODEvalue(locateGST(g,key));
		return gv->freq;
	}
}

int duplicatesGST(GST *g)
{
	return g->size - sizeGST(g);
}

void swapper(TNODE *a,TNODE *b)
{
    void *x = getTNODEvalue(a);
    void *y = getTNODEvalue(b);
    setTNODEvalue(a,y);
    setTNODEvalue(b,x);
    return;
}

int compareGSTValue(void *a, void *b)
{

	GSTVALUE *x = a;
	GSTVALUE *y = b;

	return x->compare(x->value, y->value);
}


void displayGSTValue(void *value, FILE *fp)
{
	GSTVALUE *gt = value;

	if (gt->freq > 1)
	{
		gt->display(gt->value,fp);
		fprintf(fp,"<");
		fprintf(fp,"%d>",gt->freq);
	}
	else
	{
		gt->display(gt->value,fp);
	}	

return;
}


void freeGSTValue(void *v)
{
	GSTVALUE *gv = v;
	gv->free(gv->value);	//errors here
	free(gv);
}