#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "da.h"

//CHECK ALL ASSERTIONS


struct stack
	{
		DA *da;
		int flag;

		void (*STACKdisplay)(void *, FILE *);		
		void (*STACKfree)(void *);
	};


STACK * 
newSTACK()
	{
		STACK *p = malloc(sizeof(STACK));
		if (p == 0) { fprintf(stderr, "out of memory\n"); exit(1); }
		p->flag = 0;
		p->da = newDA();
		p->STACKdisplay = 0;
		p->STACKfree = 0;
		return p;
	}

void setSTACKdisplay(STACK *items,void (*displaySTACK)(void *,FILE *))
{
	items->STACKdisplay = displaySTACK;
	setDAdisplay(items->da, displaySTACK);
	return;
}
void setSTACKfree(STACK *items,void (*freeSTACK)(void *))
{
	items->STACKfree = freeSTACK;
	setDAfree(items->da, freeSTACK);
	return;
}

void freeSTACK(STACK *items)
{
	freeDA(items->da);
	//new
	free(items);
	//if(items->STACKfree)
	//	{
	//		items->STACKfree(items);
	//	}
	//return;
}

void push(STACK *items, void *value)
{
	//insertDA(items->da,0,value);
	insertDA(items->da,sizeDA(items->da),value);
	return;
}

void *pop(STACK *items)
{
	//return removeDA(items->da,0);
	return removeDA(items->da,sizeDA(items->da)-1);
}

void *peekSTACK(STACK *items)
{
	return getDA(items->da,sizeDA(items->da)-1);
}

int debugSTACK(STACK *items, int level)
{
	int temp = items->flag;
	items->flag = level;
	return temp;
}

int sizeSTACK(STACK *items)
{
	return sizeDA(items->da);
}

void displaySTACK(STACK *items, FILE *fp)
{
	if (items->flag == 0)
	{
		if (sizeDA(items->da) == 0)
		{
			fprintf(fp,"||");
		}
		else
		{
			fprintf(fp,"|");
		
			int i;
			//for (i = 0; i < sizeDA(items->da); i++)
			for (i=sizeDA(items->da)-1;i >= 0; i--)
			{
				//if (i == sizeDA(items->da) - 1)
				if (i==0)
				{
					items->STACKdisplay(getDA(items->da,i),fp);
				}
				else
				{
					items->STACKdisplay(getDA(items->da,i),fp);
					fprintf(fp,",");
				}
			}
			fprintf(fp,"|");
		}
	}
	else if (items->flag == 1)
	{
		debugDA(items->da,0);
		displayDA(items->da,fp);
	}
	else if (items->flag == 2)
	{
		debugDA(items->da,1);
		displayDA(items->da,fp);
	}
	return;
}