#include "tnode.h"
#include "gst.h"
#include "rbt.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


struct rbt
{
    GST *tree;
    int size;

    int (*RBTcompare)(void *,void *);
    void (*RBTdisplay)(void *, FILE *);
    void (*RBTfree)(void *);
    void (*RBTswapper)(TNODE *, TNODE *);

};

typedef struct rbtValue
{
    void *value;
    int color;
    void (*display)(void *,FILE *);
    int (*compare)(void *, void *);
    void (*free)(void *);
} RBTVALUE;


RBTVALUE *newRBTvalue(int (*c)(void *, void *), void (*d)(void *,FILE *fp), void (*f)(void *));
void swapperRBT(TNODE *a,TNODE *b);
void freeRBTvalue(void *v);
void displayRBTValue(void *value, FILE *fp);
int compareRBTValue(void *a, void *b);
void insertionFix(RBT *t, TNODE *n);
int color(TNODE *n);
void setColor(TNODE *n, int color);
TNODE *sibling(TNODE *n);
TNODE *uncle(TNODE *n);
TNODE *grandparent(TNODE *n);
void leftRotation(RBT *t, TNODE *n);
void rightRotation(RBT *t, TNODE *n);
void deletionFix(RBT *t, TNODE *n);
TNODE *neice(TNODE *n);
TNODE *nephew(TNODE *n);




RBT *newRBT(int (*c)(void *,void *))
{
    RBT *r = malloc(sizeof(RBT));
    if (r == 0) { fprintf(stderr, "out of memory\n"); exit(1); }

    r->RBTcompare = c;
    r->tree = newGST(compareRBTValue);
    setRBTswapper(r,swapperRBT);      //maybe not
    r->size = 0;
    r->RBTdisplay = NULL;
    r->RBTfree = NULL;
    r->RBTswapper = swapperRBT;

    return r;
}

RBTVALUE *newRBTvalue(int (*c)(void *, void *), void (*d)(void *,FILE *fp), void (*f)(void *))
{
    RBTVALUE *rbtValue = malloc(sizeof(RBTVALUE));
    if (rbtValue == 0) { fprintf(stderr, "out of memory\n"); exit(1); }

    rbtValue->value = NULL;
    rbtValue->color = 1;        //default to red color for insert
    rbtValue->display = d;
    rbtValue->compare = c;
    rbtValue->free = f;

    return rbtValue;
}


void setRBTdisplay(RBT *t,void (*d)(void *,FILE *))
{
    t->RBTdisplay = d;
    setGSTdisplay(t->tree,displayRBTValue);

    return;
}

void setRBTswapper(RBT *t,void (*s)(TNODE *,TNODE *))
{
    t->RBTswapper = s;
    setGSTswapper(t->tree,s);   //sets gst and bst swapper
    return;
}

void setRBTfree(RBT *t,void (*f)(void *))
{
    t->RBTfree = f;
    setGSTfree(t->tree, freeRBTvalue);
    return;
}

TNODE *getRBTroot(RBT *t)
{
    return getGSTroot(t->tree);
}

void setRBTroot(RBT *t,TNODE *replacement)
{
    setGSTroot(t->tree,replacement);
    setTNODEparent(replacement,replacement);
    return;
}

void setRBTsize(RBT *t,int s)
{
    //t->size = s;
    setGSTsize(t->tree,s);
    return;
}


TNODE *insertRBT(RBT *t,void *value)
{
    t->size++;

    /*TNODE *n = locateRBT(t,value);

    if (n == NULL)      //not in there SO INSERT
    {
        RBTVALUE *rbtValue = newRBTvalue(t->RBTcompare, t->RBTdisplay, t->RBTfree);
        rbtValue->value = value;
        rbtValue->color = 1;                    //set color red
        n = insertGST(t->tree, rbtValue);
        insertionFix(t, n);
        return n;
    }
    else
    {
        RBTVALUE *rbtValue = findRBT(t,value);
        insertGST(t->tree,rbtValue);
    }*/


        RBTVALUE *rbtValue = newRBTvalue(t->RBTcompare, t->RBTdisplay, t->RBTfree);
        rbtValue->value = value;
        rbtValue->color = 1; 
        TNODE *n = insertGST(t->tree, rbtValue);

        if (n != NULL)
        {
            insertionFix(t, n);
        }
        //free(rbtValue);

    return n;       //CHANGE
}

void *findRBT(RBT *t,void *key)
{
    RBTVALUE *x = newRBTvalue(t->RBTcompare, t->RBTdisplay, t->RBTfree);
    x->value = key;
    RBTVALUE *tn = findGST(t->tree,x);
    free(x);
    if (tn == NULL)
        return NULL;

    return tn->value;
}

TNODE *locateRBT(RBT *t,void *key)
{
    RBTVALUE *x = newRBTvalue(t->RBTcompare, t->RBTdisplay, t->RBTfree);
    x->value = key;

    TNODE *tn = locateGST(t->tree,x);
    free(x);
    return tn;
}


int deleteRBT(RBT *t,void *key)
{
    TNODE *n = locateRBT(t,key);

    if (n == NULL)
    {
        return -1;
    }

    if (freqRBT(t,key) > 1)
    {
        t->size--;
        RBTVALUE *rbv = newRBTvalue(t->RBTcompare, t->RBTdisplay, t->RBTfree);
        rbv->value = key;
        int temp = deleteGST(t->tree, rbv);
        free(rbv);
        return temp;
    }
    else        //value actually needs to be deleted. and fixed up.
    {
        t->size--;
        RBTVALUE *rbv = unwrapGST(n);
        TNODE *p = swapToLeafRBT(t,n);
        deletionFix(t,p);
        pruneLeafGST(t->tree,p);
        setGSTsize(t->tree,sizeGST(t->tree)-1);
        freeTNODE(p);
        free(rbv);
        return 0;
    }
}

TNODE *swapToLeafRBT(RBT *t,TNODE *node)
{
    return swapToLeafGST(t->tree,node);
}

void pruneLeafRBT(RBT *t,TNODE *leaf)
{
    pruneLeafGST(t->tree,leaf);
    return;
}

int sizeRBT(RBT *t)
{
    return sizeGST(t->tree);
}

void statisticsRBT(RBT *t,FILE *fp)
{
    statisticsGST(t->tree,fp);
}

void displayRBT(RBT *t,FILE *fp)
{
    displayGST(t->tree,fp);
}

int debugRBT(RBT *t,int level)
{
    return debugGST(t->tree,level);
}


void freeRBT(RBT *t)
{
    freeGST(t->tree);
    free(t);
    return;              //change!!!
}


void *unwrapRBT(TNODE *n)
{
    RBTVALUE *rbtValue = getTNODEvalue(n);
    return rbtValue->value;
}


int freqRBT(RBT *t,void *key)
{
    RBTVALUE *x = newRBTvalue(t->RBTcompare, t->RBTdisplay, t->RBTfree);
    x->value = key;
    int freq = freqGST(t->tree,x);
    free(x);
    return freq;
}

int duplicatesRBT(RBT *g)
{
    return duplicatesGST(g->tree);
}


int compareRBTValue(void *a, void *b)
{
    //X DOESNT HAVE VALUE OR COMPARE
    RBTVALUE *x = a;
    RBTVALUE *y = b;

   /* if (x)
    {
        fprintf(stdout,"we have x\n");
    }
    if (y)
    {
        fprintf(stdout,"we have y\n");
    }
   if (y->compare == NULL)
    {
        fprintf(stdout,"we have x compare");
    }
    if (y->compare == NULL)
    {
        fprintf(stdout,"we have y compare ");
    }
    if (x->value)
    {
        fprintf(stdout,"we have x value");
    }
    if (y->value)
    {
        fprintf(stdout,"we have y value");
    }*/

    return x->compare(x->value, y->value);
    return 0;
}

void freeRBTvalue(void *v)
{
    RBTVALUE *rbv = v;
    rbv->free(rbv->value);
    free(rbv);
}

void displayRBTValue(void *value, FILE *fp)
{
    RBTVALUE *rv = value;

    if (rv->color == 1)
    {
        rv->display(rv->value,fp);
        fprintf(fp,"*");
    }
    else
    {
        rv->display(rv->value,fp);
    } 
}


void swapperRBT(TNODE *a,TNODE *b)
{
    void *va = getTNODEvalue(a);   //get the GST value
    void *vb = getTNODEvalue(b);   //get the GST value
    // swap the GST values
    setTNODEvalue(a,vb);
    setTNODEvalue(b,va);
    // the above swap swapped the colors,
    // but the colors should stay with the nodes,
    // so swap the colors back to the original nodes
    RBTVALUE *x = unwrapGST(a);
    RBTVALUE *y = unwrapGST(b);
    int color = x->color;
    x->color = y->color;
    y->color = color;
    return;
}





void insertionFix(RBT *t, TNODE *n)
{
    while (1)
    {
        if (getRBTroot(t) == n)
        {
            break;
        }

        if (color(getTNODEparent(n)) == 0)
        {
            break;
        }
        if (color(uncle(n)) == 1)
        {
            setColor(getTNODEparent(n),0);
            setColor(uncle(n),0);
            setColor(grandparent(n),1);
            n = grandparent(n);
        }
        else    //uncle must be black
        {
            if (getTNODEright(getTNODEparent(n)) == n)
            {
                if (getTNODEparent(n) == getTNODEleft(getTNODEparent(getTNODEparent(n))))
                {
                    TNODE *temp = getTNODEparent(n);
                    leftRotation(t,n);
                    n = temp;
                }
            }
            else if (getTNODEleft(getTNODEparent(n)) == n)
            {
                if (getTNODEparent(n) == getTNODEright(getTNODEparent(getTNODEparent(n))))
                {
                    TNODE *temp = getTNODEparent(n);
                    rightRotation(t,n);
                    n = temp;
                }
            }

            setColor(getTNODEparent(n),0);
            setColor(grandparent(n),1);

            if (getTNODEparent(n) == getTNODEleft(getTNODEparent(getTNODEparent(n))))
            {
                rightRotation(t,getTNODEparent(n));
            }
            else
            {
                leftRotation(t,getTNODEparent(n));
            }
            break;
        }
    }
    setColor(getRBTroot(t),0);
    return;
}


void deletionFix(RBT *t, TNODE *n)
{
    while(1)
    {
        if (getRBTroot(t) == n) break;
        if (color(n) == 1) break;
        if (color(sibling(n)) == 1)
        {
            setColor(getTNODEparent(n),1);
            setColor(sibling(n),0);
            if (n == getTNODEleft(getTNODEparent(n)))
            {
                leftRotation(t,sibling(n));
            }
            else
            {
                rightRotation(t,sibling(n));
            }
            assert(color(sibling(n)) == 0);             //sibling should be black now
        }
        else if(color(nephew(n)) == 1)
        {
            setColor(sibling(n),color(getTNODEparent(n)));
            setColor(getTNODEparent(n),0);
            setColor(nephew(n),0);

            if (n == getTNODEleft(getTNODEparent(n)))
            {
                leftRotation(t,sibling(n));
            }
            else
            {
                rightRotation(t,sibling(n));
            }

            break;
        }
        else if (color(neice(n)) == 1)
        {
            assert(color(nephew(n)) == 0);
            setColor(neice(n),0);
            setColor(sibling(n),1);

            if (n == getTNODEleft(getTNODEparent(n))) //n is left child so rotate niece right
            {
                rightRotation(t,neice(n));
            }
            else        //rotate neice left
            {
                leftRotation(t,neice(n));
            }
            assert(color(nephew(n)) == 1);
        }
        else
        {
            assert(color(sibling(n)) == 0 && color(neice(n)) == 0 && color(nephew(n)) == 0);
            setColor(sibling(n),1);
            n = getTNODEparent(n);
        }
    }
    setColor(n,0);
    return;
}


int color(TNODE *n)
{
    if (n == NULL)
    {
        return 0;
    }
    else
    {
        RBTVALUE *rbv = unwrapGST(n);    //sends back gst->value which is an rbt value
        return rbv->color;
    }
}

void setColor(TNODE *n, int color)
{
    if (n != NULL)
    {
        RBTVALUE *rbv = unwrapGST(n);
        rbv->color = color;
    }
    return;
}


TNODE *nephew(TNODE *n)
{
    if (getTNODEleft(getTNODEparent(n)) == n)   //node is a left child
    {
        if (sibling(n) == NULL)
        {
            //fprintf(stdout,"NO CHILD\n\n");
            return NULL;
        }
        return getTNODEright(sibling(n));
    }
    else
    {
        if (sibling(n) == NULL)
        {
            //fprintf(stdout,"NO CHILD\n\n");
            return NULL;
        }
        return getTNODEleft(sibling(n));
    }
}


TNODE *neice(TNODE *n)
{
    if (getTNODEleft(getTNODEparent(n)) == n)   //node is a left child
    {
        if (sibling(n) == NULL)
        {
            //fprintf(stdout,"NO CHILD\n\n");
            return NULL;
        }
        return getTNODEleft(sibling(n));
    }
    else
    {
        if (sibling(n) == NULL)
        {
            //fprintf(stdout,"NO CHILD\n\n");
            return NULL;
        }
        return getTNODEright(sibling(n));
    }
}

TNODE *uncle(TNODE *n)
{
    if (getTNODEparent(n) == getTNODEleft(getTNODEparent(getTNODEparent(n))))
    {
        return getTNODEright(getTNODEparent(getTNODEparent(n)));
    }
    else
    {
        return getTNODEleft(getTNODEparent(getTNODEparent(n)));
    }
}

TNODE *grandparent(TNODE *n)
{
    return getTNODEparent(getTNODEparent(n));
}

TNODE *sibling(TNODE *n)
{
    if (getTNODEleft(getTNODEparent(n)) == n)
    {
        return getTNODEright(getTNODEparent(n));
    }
    else
    {
        return getTNODEleft(getTNODEparent(n));
    }
}


void leftRotation(RBT *t, TNODE *n)
{
    TNODE *a = getTNODEparent(n);

    setTNODEparent(n,getTNODEparent(a));

    if (a != getRBTroot(t))
    {
        if (getTNODEleft(getTNODEparent(a)) == a)
        {
            setTNODEleft(getTNODEparent(a),n);
        }
        else
        {
            setTNODEright(getTNODEparent(a),n);
        }
    }

    TNODE *temp = getTNODEleft(n);
    setTNODEleft(n,a);
    setTNODEparent(a,n);
    setTNODEright(a,temp);

    if (temp != NULL)
    {
        setTNODEparent(temp,a);
    }

    if (a == getRBTroot(t))
    {
        setRBTroot(t,n);
    }

    return;
}


void rightRotation(RBT *t, TNODE *n)
{
    TNODE *a = getTNODEparent(n);

    setTNODEparent(n,getTNODEparent(a));

    if (a != getRBTroot(t))
    {
        if (getTNODEleft(getTNODEparent(a)) == a)
        {
            setTNODEleft(getTNODEparent(a),n);
        }
        else
        {
            setTNODEright(getTNODEparent(a),n);
        }
    }

    TNODE *temp = getTNODEright(n);
    setTNODEright(n,a);
    setTNODEparent(a,n);
    setTNODEleft(a,temp);

    if (temp != NULL)
    {
        setTNODEparent(temp,a);
    }

    if (a == getRBTroot(t))
    {
        setRBTroot(t,n);
    }

    return;
}