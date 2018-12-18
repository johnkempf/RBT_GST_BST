#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cda.h"

//remove testing prints

struct queue 
{
	CDA *cda;
	int flag;

	void (*QUEUEdisplay)(void *, FILE *);		
	void (*QUEUEfree)(void *);
};


QUEUE *newQUEUE()
{
	QUEUE *p = malloc(sizeof(QUEUE));
	if (p == 0) { fprintf(stderr, "out of memory\n"); exit(1); }
	p->flag = 0;
	p->cda = newCDA();
	p->QUEUEdisplay = 0;
	p->QUEUEfree = 0;
	return p;
}

void setQUEUEdisplay(QUEUE *items,void (*displayQUEUE)(void *,FILE *))
{
	items->QUEUEdisplay = displayQUEUE;
	setCDAdisplay(items->cda, displayQUEUE);
	return;
}
void setQUEUEfree(QUEUE *items,void (*freeQUEUE)(void *))
{
	items->QUEUEfree = freeQUEUE;
	setCDAfree(items->cda, freeQUEUE);
	return;
}

void freeQUEUE(QUEUE *items)
{
	freeCDA(items->cda);
	free(items);
	//if(items->QUEUEfree)
	//{
	//	items->QUEUEfree(items);
	//}
	return;
}

void enqueue(QUEUE *items, void *value)
{
	insertCDAback(items->cda,value);
	return;
}

void *dequeue(QUEUE *items)
{
	return removeCDAfront(items->cda);
}

void *peekQUEUE(QUEUE *items)
{
	return getCDA(items->cda,0);
}

int debugQUEUE(QUEUE *items, int level)
{
	int temp = items->flag;
	items->flag = level;
	return temp;
}

int sizeQUEUE(QUEUE *items)
{
	return sizeCDA(items->cda);
}


void displayQUEUE(QUEUE *items, FILE *fp)
{
	if (items->flag == 0)
	{
		if (sizeCDA(items->cda) == 0)
		{
			fprintf(fp,"<>");
		}
		else
		{
			fprintf(fp,"<");
		
			int i=0;
			for (i = 0; i < sizeCDA(items->cda); i++)
			{
				if (i == sizeCDA(items->cda) - 1)
				{
					items->QUEUEdisplay(getCDA(items->cda,i),fp);
				}
				else
				{
					items->QUEUEdisplay(getCDA(items->cda,i),fp);
					fprintf(fp,",");
				}
			}
			fprintf(fp,">");
		}
	}
	else if (items->flag == 1)
	{
		debugCDA(items->cda,0);
		displayCDA(items->cda,fp);
	}
	else if (items->flag == 2)
	{
		debugCDA(items->cda,1);
		displayCDA(items->cda,fp);
	}
	return;
}