#include "tnode.h"
#include "bst.h"
#include "gst.h"
#include "rbt.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "integer.h"
#include "string.h"
#include "real.h"
#include <string.h>
#include "scanner.h"
#include <ctype.h>

void srandom(unsigned int);
long int random(void);

int intComparator(void *a, void *b);
int stringComparator(void *a, void *b);
int GSTcomparator(void *a, void *b);

void readFileGST (FILE *fp, GST *a);
void readFileRBT (FILE *fp, RBT *a);
void readCommandsGST(FILE *fp, FILE *fpOut, GST *a);
void readCommandsRBT(FILE *fp, FILE *fpOut, RBT *a);
char *obtainString(FILE *fp);
void cleanString (char *s);
void trimLeading(char * str);
void trimTrailing(char * str);
int realComparator(void *a, void *b);


int main(int argc, char **argv)
{
	if (argc > 0)
	{
		if (0 == strcmp(argv[1],"-v"))
		{
			fprintf(stdout,"Created By John Kempf\n");
		}

		else if (0 == strcmp(argv[1],"-g"))
		{
			GST *gt = newGST(stringComparator);
			setGSTdisplay(gt,displaySTRING);
			setGSTfree(gt,freeSTRING);
			FILE *fp = fopen(argv[2],"r");
			readFileGST(fp,gt);
			fclose(fp);
			FILE *fp2 = fopen(argv[3],"r");
			readCommandsGST(fp2,stdout,gt);
			fclose(fp2);
			freeGST(gt);
		}
		/*else if (0 == strcmp(argv[1],"-t"))
		{
    	
		}*/
		else
		{
			RBT *gt = newRBT(stringComparator);
			setRBTdisplay(gt,displaySTRING);
			setRBTfree(gt,freeSTRING);
			FILE *fp = fopen(argv[2],"r");
			readFileRBT(fp,gt);
			fclose(fp);
			FILE *fp2 = fopen(argv[3],"r");
			readCommandsRBT(fp2,stdout,gt);
			fclose(fp2);
			freeRBT(gt);			
		}
	}
	return 0;
}






int intComparator(void *a, void *b) {
    int x = getINTEGER(a);
    int y = getINTEGER(b);
    if (x == y)
        return 0;
    if (x > y)
        return 1;
    else
        return -1;
}


int stringComparator(void *a, void *b) {
    char *x = getSTRING(a);
    char *y = getSTRING(b);
    return strcmp(x,y);
}

int realComparator(void *a, void *b)
{
	double x = getREAL(a);
		double y = getREAL(b);
if (x == y)
        return 0;
    if (x > y)
        return 1;
    else
        return -1;
}






void readFileGST(FILE *fp, GST *a) {
    char *str = obtainString(fp);
    while (!feof(fp)) {
        cleanString(str);
        trimTrailing(str);
		
		if (str[0] ==' ')
        {
        	trimLeading(str);
        }
   
        if (strcmp(str,"") != 0)
        {
            insertGST(a, newSTRING(str));
			//freeSTRING(str);		//I THINK WE NEED TO FREE THESE STRINGS!

        }

        str = obtainString(fp);
    }
}


void readFileRBT (FILE *fp, RBT *a) {
    char *str = obtainString(fp);
    while (!feof(fp)) {
        cleanString(str);
        trimTrailing(str);

        if (str[0] ==' ')
        {
        	trimLeading(str);
        }

        if (strcmp(str,"") != 0)
        {
            insertRBT(a,newSTRING(str));
        }

        str = obtainString(fp);
    }
}

void readCommandsGST(FILE *fp, FILE *fpOut, GST *a) {
    char *str = obtainString(fp);

    while (!feof(fp)) {
        cleanString(str);
        //if it is a i you want to get the value as well
        if (strcmp(str, "i") == 0) {
            str = obtainString(fp);
            cleanString(str);
            trimTrailing(str);
            trimLeading(str);

            if (strcmp(str,"") != 0)
            	insertGST(a, newSTRING(str));
        }
        else if (strcmp(str, "d") == 0) {
            str = obtainString(fp);

            cleanString(str);
            trimTrailing(str);
            trimLeading(str);

            if (strcmp(str,"") != 0)
            {
            	int x = deleteGST(a, newSTRING(str));

            	if (x == -1)
            		fprintf(stdout,"Value %s not found.\n",str);
            }
        }
        else if (strcmp(str, "f") == 0) {
            str = obtainString(fp);
            cleanString(str);
            trimTrailing(str);
            trimLeading(str);
            fprintf(fpOut, "Frequency of \"%s\": %d\n", str, freqGST(a, newSTRING(str)));
        }
        else if (strcmp(str, "s") == 0)
        {
        	if (getGSTroot(a) == NULL)
        	{
        		fprintf(stdout,"EMPTY\n");
        	}
        	else
        	{
        	    displayGST(a,fpOut);	
        	}
        }
        else if (strcmp(str, "r") == 0)
        {
            statisticsGST(a,fpOut);
        }
        str = obtainString(fp);
    }
}

void readCommandsRBT(FILE *fp, FILE *fpOut, RBT *a) {
    char *str = obtainString(fp);
    while (!feof(fp)) {
        cleanString(str);
        //if it is a i you want to get the value as well
        if (strcmp(str, "i") == 0) {
            str = obtainString(fp);
            cleanString(str);
            trimTrailing(str);
            trimLeading(str);

            if (strcmp(str,"") != 0)
                insertRBT(a, newSTRING(str));
        }
        else if (strcmp(str, "d") == 0) {
            str = obtainString(fp);
            cleanString(str);
            trimTrailing(str);
            trimLeading(str);

            if (strcmp(str,"") != 0)
            {
            	int x = deleteRBT(a, newSTRING(str));

            	if (x == -1)
            		fprintf(stdout,"Value %s not found.\n",str);
            }
        }
        else if (strcmp(str, "f") == 0) {
            str = obtainString(fp);
            cleanString(str);
            trimTrailing(str);
            trimLeading(str);
            fprintf(fpOut, "Frequency of \"%s\": %d\n", str, freqRBT(a, newSTRING(str)));
        }
        else if (strcmp(str, "s") == 0)
        {
        	if (getRBTroot(a) == NULL)
        	{
        		fprintf(stdout,"EMPTY\n");
        	}
        	else
        	{
        	    displayRBT(a,fpOut);	
        	}
        }
        else if (strcmp(str, "r") == 0)
            statisticsRBT(a,fpOut);
        str = obtainString(fp);
    }
}


char *obtainString(FILE *fp) {
    if (stringPending(fp))
    {
    	//fprintf(stdout,"RS: ");
        return readString(fp);
    }
    else
    {
    	//fprintf(stdout,"RT: ");
        return readToken(fp);
    }
}


 void cleanString (char *s) {
    char *i = s, *j = s;
    while (*i) {
        if(isspace((unsigned char)*i)) {
            //while it is something to remove get rid of it
            while(isspace((unsigned char)*i) || isdigit((unsigned char)*i) || ispunct((unsigned char)*i))
                i++;
            //add a space because there was at least 1 space
            *j++ = ' ';
        }
        //if not alpha
        else if (!isalpha((unsigned char)*i))
            i++;
        //if uppercase make lower
        else if (isupper((unsigned char)*i)) {
            *j++ = tolower((unsigned char)*i);
            i++;
        }
        //if the same then incriment both
        else if (i == j) {
            i++;
            j++;
        }
        //copy character
        else
            *j++ = *i++;
    }
    //set last value null
    *j = 0;
}


void trimLeading(char * str)
{
    int index, i;

    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }


    if(index != 0)
    {
        /* Shit all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}



void trimTrailing(char * str)
{
    int index, i;

    /* Set default index */
    index = -1;

    /* Find last index of non-white space character */
    i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index= i;
        }

        i++;
    }

    /* Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
}