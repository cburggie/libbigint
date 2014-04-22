#include <stdio.h>
#include <BigInt.h>
#include <Chunk.h>
#include "test.h"

static const int charsPerUint = 2 * sizeof(unsigned int);

/* ##### Prototypes ##### */

int testSetValue(BigInt * obj);
int testToString(int length);
int testAdd(int length);


/* ##### Definitions ##### */

int testSetValue(BigInt * obj)
{
	
	//get an array of ints
	int size = 3007, i;
	unsigned int array[size];
	for (i = 0; i < size; i++)
	{
		array[i] =(unsigned int) (size - i);
	}
	
	//perform setValue operation
	if (setValue(obj, size, array)) return 1;
	
	//no error during method call
	int tailLength = size % CHUNKSIZE;
	int chunks = size / CHUNKSIZE;
	if (tailLength) chunks++;
	
	int actualLength = lengthBigInt(obj);
	
	if (actualLength != chunks)
	{
		printf("  setValue(): chunk count mismatch.");
		printf(" Is %i, should be %i\n", actualLength, chunks);
		return 1;
	}
	
	return 0;
	
}

int testToString(int length)
{
	int i, error = 0;
	
	unsigned int array[length];
	for (i = 0; i < length; i++) array[i] = (unsigned int) (-1);
	
	BigInt * obj = newBigInt();
	
	setValue(obj, length, array);
	
	int strlen = length * charsPerUint;
	char * string = toString(obj);
	
	for (i = 0; i < strlen; i++)
	{
		if (string[i] != 'f')
		{
			printf("error in toString():\nis: %s\nshould be all 'f's\n",string);
			error += 1;
			break;
		}
	}
	
	if (string[strlen])
	{
		printf("error in toString(): ");
		printf("not null terminated correctly at position %i", strlen);
		printf(". Has value %i, but should ", (int) string[strlen] );
		printf("have value %i.\n", (int) '\0');
		error += 1;
	}
	
	freeBigInt(obj);
	free(string);
	
	return error;
	
	
	
}

int testAdd(int length)
{
	if (!length) return 1;
	
	BigInt * left = newBigInt();
	BigInt * right = newBigInt();
	
	unsigned int array[length];
	int i, strlen, error = 0;
	for (i = 0; i < length; i++)
	{
		array[i] = (unsigned int) (-1);
	}
	
	setValue(left, length, array);
	array[0] = 1;
	setValue(right, 1, array);
	
	add(left, right);
	
	char * string = toString(left);
	strlen = charsPerUint * (length  + 1);
	
	for (i = 0; i < strlen - charsPerUint; i++)
	{
		if (string[i] != '0')
		{
			printf("add(): value at string index ");
			printf("%i is '%c' should be '0'\n", i, string[i]);
			error++;
		}
	}
	
	if (string[strlen - charsPerUint] != '1')
	{
		printf("add(): not carrying into next uint correctly.\n");
		error++;
	}
	
	if (string[strlen] != '\0')
	{
		printf("add(): toString() of add() result ");
		printf("not null terminated correctly\n");
	}
	
	/*
	printf("%s\n", string);
	for (i = 0; i < strlen; i++)
	{
		if (i % 10 == 0) printf("%i", (i/10) % 10);
		else printf(" ");
	} printf("\n");
	*/
	
	return error;
}


//int testAdd(void)



/* ##### Main ##### */

int main(void)
{
	printf("Begin tests of BigInt.c\n");
	
	int err = 0;
	
	BigInt * obj = newBigInt();
	
	if (obj == NULL)
	{
		doTest(1, "newBigInt()");
		FATAL("BigInt", "newBigInt()");
	}
	
	else
	{
		err += doTest(0, "newBigInt()");
	}
	
	err += doTest(testSetValue(obj), "setValue()");
	
	freeBigInt(obj);
	
	err += doTest(testToString(15), "toString()");
	err += doTest(testAdd(15), "add()");
	
	
	
	
	if (err)
	{
		printf("BigInt module failed %i test", err);
		if (err > 1) printf("s");
		printf(".\n");
	}
	
	else
	{
		printf("BigInt module passed all tests.\n");
	}
	
	return 0;
	
}


