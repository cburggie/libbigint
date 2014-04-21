#include <stdio.h>
#include <BigInt.h>
#include <Chunk.h>
#include "test.h"


/* ##### Prototypes ##### */

int testSetValue(BigInt * obj);
//int testAdd(void);


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


