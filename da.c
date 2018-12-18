//project questions:
//-I think i may be losing memory from the temp array in unionDA
//make sure union works for bigger arrays that require shrinking/growing
//remve test prints


#include "da.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct da
	{
		int size;
		int capacity;
		int flag;
		void **arr;			

		void (*DAdisplay)(void *, FILE *);		
		void (*DAfree)(void *);
	};

//Constructors
DA *
newDA()
	{
		DA *p = malloc(sizeof(DA));
		if (p == 0) { fprintf(stderr, "out of memory\n"); exit(1); }
		p->size = 0;
		p->capacity = 1;
		p->flag = 0;		
		p->arr = malloc(sizeof(void *));	
		p->DAdisplay = 0;
		p->DAfree = 0;
		return p;
	}

//Acessors
void *getDA(DA *items, int index) 
{	
	assert(index >= 0  && index < items->size);
	return items->arr[index]; 
}


//Mutators
void setDAdisplay(DA *items,void (*displayDA)(void *,FILE *))
{
	items->DAdisplay = displayDA;
}
void setDAfree(DA *items,void (*freeDA)(void *))
{
	items->DAfree = freeDA;
}


void *setDA(DA *items,int index,void *value) //may need to relook at this
{
	assert(index >= 0  && index <= items->size);
	
	void *temp;

	if (index == sizeDA(items))
	{
		insertDA(items,index,value);
		return NULL;	
	}
	else
	{
		temp = items->arr[index];
		items->arr[index] = value;
		return temp;
	}
}

//Visualizers
void displayDA(DA *items,FILE *fp)
{
	
	if (items->size == 0)
	{
		if (items->flag > 0)
		{
			fprintf(fp,"[[1]]");
		}
		else
		{
			fprintf(fp,"[]");
		}
	}

	else
	{
		if (items->flag > 0)
		{
			fprintf(fp,"[");
			int i;
			for (i = 0; i < items->size; i++)
			{
				if (i == items->size - 1)
				{
					if (items->DAdisplay)
					{
						items->DAdisplay(items->arr[i],fp);
						fprintf(fp,",[%d]",items->capacity - items->size);
					}
					else
					{
						fprintf(fp,"@%p",items->arr[i]);
						fprintf(fp,",[%d]",items->capacity - items->size);
					}
				}
				else
				{
					if (items->DAdisplay)
					{
						items->DAdisplay(items->arr[i],fp);
						fprintf(fp,",");
					}
					else
					{
						fprintf(fp,"@%p",items->arr[i]);
						fprintf(fp,",");
					}
					
				}	
			}
		}
		else
		{
			fprintf(fp,"[");
			int i;
			for (i = 0; i < items->size; i++)
			{
				if (i == items->size - 1)	//last entry doesnt need comma
				{
					if (items->DAdisplay)
					{
						items->DAdisplay(items->arr[i],fp);
					}
					else
					{
						fprintf(fp,"@%p",items->arr[i]);
					}
				}
				else
				{
					if (items->DAdisplay)
					{
						items->DAdisplay(items->arr[i],fp);
						fprintf(fp,",");
					}
					else
					{
						fprintf(fp,"@%p",items->arr[i]);
						fprintf(fp,",");
					}
				}	
			}
		}
	fprintf(fp,"]");
	}
	       // printf(". capacity:%d, size:%d\n",items->capacity, items->size);
	return;
}

//Additional
void insertDA(DA *items,int index,void *value)
{
	if (items->size >= items->capacity)	//double array capacity
	{
		items->arr = realloc(items->arr, items->capacity * 2 * sizeof(void *));
		items->capacity = items->capacity * 2;			//set new capacity value
		assert(items->arr);   				//check to make sure memory allocated is not 0
	}

	if (index >= items->size) 	
	{	

		items->arr[index] = value;												//and there is something in slot
	}
	else
	{
		int i;
		for (i = sizeDA(items); i > index; i--)
		{
			items->arr[i] = items->arr[i-1];
		}
		items->arr[index] = value;
	}

	items->size++;			
	return;
}


void *removeDA(DA *items,int index)
{
	assert(items->size > 0);				//make sure there is an item to remove
	void *temp2 = getDA(items,index);

	if ((items->size-1) / (float)items->capacity < 0.25)
	{
		items->arr = realloc(items->arr, items->capacity / 2 * sizeof(void *));
		items->capacity = items->capacity / 2;
	}

	
	while(index < items->size - 1)
	{
		void *temp = items->arr[index + 1];
		items->arr[index] = temp;
		index++;
	}



	if (items->size - 1 == 0) //array is empty now
	{
		items->arr = realloc(items->arr, sizeof(void *) * items->capacity / 2);
		items->capacity = items->capacity / 2;
	}

	items->size--;
	return temp2;
}

void unionDA(DA *recipient,DA *donor)
{
	int i;
	for (i = 0; i < donor->size; i++)
	{
		insertDA(recipient, recipient->size, removeDA(donor,0));
	}

	return;
}

int sizeDA(DA *items)
{
	return items->size;
}

int debugDA(DA *items,int level)
{
	int temp = items->flag;
	items->flag = level;
	return temp;
}

void freeDA(DA *items)
{
	int i;

	if (items->DAfree)
	{
		for (i=0; i < items->size; i++)
		{
			items->DAfree(items->arr[i]);
			items->arr[i] = NULL;
		}
	}
	
	free(items->arr);
	free(items);
}