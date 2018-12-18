//do i have all my assertions right?

#include "cda.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct cda
	{
		int size;
		int capacity;
		int startIndex;
		int endIndex;
		int flag;
		void **arr;			

		void (*CDAdisplay)(void *, FILE *);		
		void (*CDAfree)(void *);
	};

int correctIndex(CDA *items, int index);
//Constructors
CDA *
newCDA()
	{
		CDA *p = malloc(sizeof(CDA));
		if (p == 0) { fprintf(stderr, "out of memory\n"); exit(1); }
		p->size = 0;
		p->capacity = 1;
		p->startIndex = 0;
		p->endIndex = 0;
		p->flag = 0;
		p->arr = malloc(sizeof(void *));	
		p->CDAdisplay = 0;
		p->CDAfree = 0;
		return p;
	}

int correctIndex(CDA *items, int index)
{
	return (index + items->startIndex + items->capacity) % items->capacity;
}

//Acessors
void *getCDA(CDA *items, int index) 
{	
	if (index >= 0 && index <= items->size)
	{
		index = ((index + items->startIndex) + items->capacity) % items->capacity;
		return items->arr[index];
	}
	
	else 
	{
		fprintf(stderr, "index invalid"); exit(1);
	}
}

void *setCDA(CDA *items, int index, void *value)
{
	assert(index >= -1 && index <= items->size);
	
	int new = (index + items->startIndex + items->capacity) % items->capacity;
		
	void *temp;


	if (index == items->size)		 
	{
		insertCDAback(items,value);
		return NULL;
	}

	else if (index == -1)
	{
		insertCDAfront(items,value);
		return NULL;
	}

	else
	{
		temp = items->arr[new];
		items->arr[new] = value;
		return temp;
	}

}


//Mutators
void setCDAdisplay(CDA *items,void (*displayCDA)(void *,FILE *))
{
	items->CDAdisplay = displayCDA;
}
void setCDAfree(CDA *items,void (*freeCDA)(void *))
{
	items->CDAfree = freeCDA;
}

int sizeCDA(CDA *items)
{
	return items->size;
}

int debugCDA(CDA *items,int level)
{
	int temp = items->flag;
	items->flag = level;
	return temp;
}


void freeCDA(CDA *items)
{
	int i;
	for (i=0; i < items->size; i++)
	{
		int index = ((i + items->startIndex) + items->capacity) % items->capacity;
		items->CDAfree(items->arr[index]);
	}
	
	free(items->arr);
	free(items);
}


void insertCDA(CDA *items,int index,void *value)
{
	if (items->size >= items->capacity)	
	{
		if (items->startIndex != 0)
		{
			int i = 0;
			void **arrayTemp = malloc(sizeof(void*) * (items->capacity * 2));
	

			for (i = 0; i < items->size; i++)
			{
				arrayTemp[i] = items->arr[(i + items->startIndex + items->capacity) % items->capacity];
			}

			free(items->arr);
			items->arr = arrayTemp;

			items->arr = realloc(items->arr, sizeof(void *) * items->capacity * 2);
		}  

		else
		{
			items->arr = realloc(items->arr, sizeof(CDA) * items->capacity * 2);
		}	

		items->startIndex =  0;
		items->endIndex = items->size;
		items->capacity = items->capacity * 2;			
	}


	 items->size++;       	 //increase size up here now

	if (items->size == 1 || index == items->size-1) //insert back
	{	
		items->arr[items->endIndex] = value;
		items->endIndex = (items->endIndex + 1 + items->capacity) % items->capacity;		
	}
	else if (index == 0)
	{
		items->startIndex = (items->startIndex - 1 + items->capacity) % items->capacity;
		items->arr[items->startIndex] = value;
	}


	else //insert middle
	{
		if (index < sizeCDA(items)/2)
		{
			items->startIndex = (items->startIndex - 1 + items->capacity) % items->capacity;
			int i;
			for (i=0; i<=index; i++)
			{

				void *temp = getCDA(items,i+1);
				setCDA(items,i,temp);
			}
			setCDA(items,index,value);
		}
		else
		{
			int i;
			for(i = sizeCDA(items)-1; correctIndex(items,i) > correctIndex(items,index); i--)
			{
				items->arr[correctIndex(items,i)] = items->arr[correctIndex(items,i-1)];
			}
			setCDA(items,index,value);
			items->endIndex = (items->endIndex + 1 + items->capacity) % items->capacity;
		}
		
	}

	//items->size++;
	return;
}

void *removeCDA(CDA *items,int index)
{
	assert(items->size > 0);
	void *temp2 = getCDA(items,index);

	if ((items->size-1) / (float)items->capacity < 0.25)
	{		
		if (items->startIndex != 0)
		{
			int i = 0;
			void **arrayTemp = malloc(sizeof(void*) * (items->capacity));
			

			for (i = 0; i < items->size; i++)
			{
				arrayTemp[i] = items->arr[(i + items->startIndex + items->capacity) % items->capacity];
			}									

			free(items->arr);
			items->arr = arrayTemp;

			items->arr = realloc(items->arr, sizeof(void *) * items->capacity / 2);


			items->startIndex =  0;
			items->endIndex = items->size;
		}
		
		else
		{
			items->arr = realloc(items->arr, sizeof(void *) * items->capacity / 2);
		}

		items->startIndex =  0;
		items->endIndex = items->size;
		items->capacity = items->capacity / 2;
	}

	//if (items->capacity == 0)
	//{
	//	items->capacity = 1;
	//}
	if (index==0) //remove front
	{
		//fprintf(stdout,"cap: %d\n",items->capacity);
		items->startIndex = (items->startIndex + 1 + items->capacity) % items->capacity;
	}
	else if (index == items->size-1) //remove back
	{
		items->endIndex = (items->endIndex - 1 + items->capacity) % items->capacity;
	}
	else //remove middle
	{

		int i;
		
		if (index < sizeCDA(items)/2)
		{
			for(i = index; i>0; i--)
			{
				items->arr[correctIndex(items,i)] = items->arr[correctIndex(items,i-1)];
			}
			items->startIndex = (items->startIndex + 1 + items->capacity) % items->capacity;
		}
		else
		{
			for(i = index; i < sizeCDA(items) - 1; i++)
			{
				items->arr[correctIndex(items,i)] = items->arr[correctIndex(items,i+1)];
			}
			items->endIndex = (items->endIndex - 1 + items->capacity) % items->capacity;		
		}












		/*
		void *temp = NULL;
		int i;

		if (index > sizeCDA(items)/2)
		{
			for (i = index; i < items->size - 1; i++)
			{
				int new = (i + items->startIndex + items->capacity) % items->capacity;
			
				temp = items->arr[(new+1+items->capacity) % items->capacity];
			
				items->arr[new] = temp;
			}
			items->endIndex = (items->endIndex - 1 + items->capacity) % items->capacity;		
		}
		else
		{
			for (i = index; i > 0; i--)
			{
				int new = (i + items->startIndex + items->capacity) % items->capacity;
				temp = items->arr[(new-1+items->capacity) % items->capacity];
				items->arr[new] = temp;
			}
			items->startIndex = (items->startIndex + 1 + items->capacity) % items->capacity;		

		}*/
		
	}

	if (items->size - 1 == 0) //array is empty now
	{
		items->arr = realloc(items->arr, sizeof(void *) * items->capacity / 2);
		//items->arr = realloc(items->arr, sizeof(void *));
		items->capacity = items->capacity / 2;
		//items->capacity = 1;
		items->startIndex = 0;
		items->endIndex = 0;
	}
	items->size--;
	return temp2;
}

void unionCDA(CDA *recipient,CDA *donor)
{
	while(sizeCDA(donor))
	{
		insertCDAback(recipient, removeCDAfront(donor));
	}

	return;
}


//Visualizers
void displayCDA(CDA *items,FILE *fp)
{
	if (items->size == 0)
	{
		if (items->flag > 0)
		{
			fprintf(fp,"((1))");
		}
		else
		{
			fprintf(fp,"()");
		}
	}
	else
	{
		if (items->flag > 0)
		{
			fprintf(fp,"(");
			int i;
			for (i = 0; i < items->size; i++)
			{
				int index = ((i + items->startIndex) + items->capacity) % items->capacity;
				if (i == items->size - 1)
				{
					if (items->CDAdisplay)
					{
						items->CDAdisplay(items->arr[index],fp);
						fprintf(fp,",(%d)",items->capacity - items->size);
					}
					else
					{
						fprintf(fp,"@%p",items->arr[index]);
						fprintf(fp,",(%d)",items->capacity - items->size);
					}
				}
				else
				{
					if (items->CDAdisplay)
					{
						items->CDAdisplay(items->arr[index],fp);
						fprintf(fp,",");
					}
					else
					{
						fprintf(fp,"@%p",items->arr[index]);
						fprintf(fp,",");
					}
					
				}	
			}
		}
		else
		{
			fprintf(fp,"(");
			int i;
			for (i = 0; i < items->size; i++)
			{
				int index = ((i + items->startIndex) + items->capacity) % items->capacity;
				if (i == items->size - 1)	//last entry doesnt need comma
				{
					if (items->CDAdisplay)
					{
						items->CDAdisplay(items->arr[index],fp);
					}
					else
					{
						fprintf(fp,"@%p",items->arr[index]);
					}
				}
				else
				{
					if (items->CDAdisplay)
					{
						items->CDAdisplay(items->arr[index],fp);
						fprintf(fp,",");
					}
					else
					{
						fprintf(fp,"@%p",items->arr[index]);
						fprintf(fp,",");
					}
				}	
			}
		}
		fprintf(fp,")");
	}
	//fprintf(fp," size: %d, capacity: %d, startIndex: %d, endIndex: %d",items->size,items->capacity,items->startIndex,items->endIndex);	
	return;
}