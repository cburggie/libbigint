#include "./BigInt.h"
#include <stdlib.h>

#define ERROR(x) if(x) return 1
#define MIN(a,b) a < b ? a : b
#define MAX(a,b) a > b ? a : b
#define BIGINT_DEBUG


/* ##### private method declarations ##### */

static int reset(BigInt * self);
static int append(BigInt * self, Chunk * chunk);

static char hex[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};




/* ##### public method definitions ##### */

BigInt * newBigInt(void)
{
	
	//allocate memory
	BigInt * self = (BigInt *) malloc(sizeof(BigInt));
	if (!self)
	{
		return NULL;
	}
	
	
	Chunk * chunk = newChunk();
	if (!chunk)
	{
		free(self); return NULL;
	}
	
	
	//initialize object
	self->first = chunk;
	self->last = chunk;
	self->parity = 1;
	self->length = 1;
	
	chunk->prev = NULL;
	chunk->next = NULL;
	chunk->length = 1;
	chunk->value[0] = 0;
	
	return self;
	
}



void freeBigInt(BigInt * self)
{
	
	//let's avoid freeing a null pointer
	if (self)
	{
		Chunk * chunk = self->first;
		Chunk * next;
		while (chunk)
		{
			next = chunk->next;
			free(chunk);
			chunk = next;
		}
		
		free(self);
	}
	
}



int setValue(BigInt * self, int length, unsigned int * value)
{
	
	ERROR(!self || !value);
	
	int numChunks = length / CHUNKWIDTH;
	if (length % CHUNKWIDTH) numChunks++;
	
	//trim chunks that overflow the value array
	if (self->length > numChunks)
	{
		int i = 0;
		Chunk * p = self->first;
		
		while (i < numChunks && p)
		{
			p = p->next;
		}
		
		self->last = p;
		trimChunk(p);
		self->length = numChunks;
	}
	
	//else extend the big int until we have the right number of chunks
	else while (self->length < numChunks)
	{
		append(self, newChunk());
	}
	
	Chunk * chunk = self->first;
	
	int arrayIndex = 0, chunkIndex;
	
	//loop until out of chunks
	while (chunk)
	{
		chunk->length = MIN(CHUNKWIDTH, length);
		
		for (chunkIndex = 0; chunkIndex < chunk->length; chunkIndex++)
		{
			chunk->value[chunkIndex] = value[arrayIndex++];
		}
		
		chunk = chunk->next;
	}
	
	return 0;
	
}

/*

char * toString(BigInt * self)
{
	int bytecount = 0;
	BigIntChunk * chunk = self->first;
	while (chunk != NULL)
	{
		bytecount += sizeof(int) * chunk->length;
		chunk = chunk->next;
	}
	
	char * string = malloc( sizeof(char) * (2 * bytecount + 1) );
	string[2* bytecount + 1] = '\0';
	
	int sIndex = 0, cIndex, i;
	chunk = self->first;
	
	while (chunk != NULL)
	{
		for (cIndex = 0; cIndex < chunk->length; cIndex++)
		{
			i = chunk->value[cIndex];
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
			string[sIndex] = hex[i % 16];
			sIndex++; i = i >> 4;
		}
		
		chunk = chunk->next;
	}
	
	return string;
}

BigInt * add(BigInt * self, BigInt * bi)
{
	
	if (!self || !bi)
	{
		#ifdef BIGINT_DEBUG
		printf("Error calling add() method. Null argument passed.\n");
		#endif

		return NULL;
	}
	
	if (!self->first)
	{
		#ifdef BIGINT_DEBUG
		printf("Error calling add() method. Object has no first chunk.\n");
		#endif
		
		append(self, newChunk());
	}
	
	//setup location in sum
	BigIntChunk * chunk = self->first;
	int index = 0;
	
	//enumerate what we're adding
	EnumeratedBigInt * iterator = enumerate(bi);
	int * p = next(iterator);
	int value;
	
	while (p != NULL)
	{
		while (index == chunk->length)
		{
			if (chunk->next != NULL)
			{
				index = 0;
				chunk = chunk->next;
			}
			
			else if (index < CHUNKWIDTH)
			{
				chunk->length = index + 1;
				chunk->value[index] = 0;
				printf("extending within chunk, now %i wide\n", index);
			}
			
			else
			{
				append(self, newChunk());
				chunk = self->last;
				chunk->length++;
				index = 0;
				printf("appending a new chunk\n");
			}
		}
		
		
		value = *p;
		addWithCarry(self,chunk,index++,value);
		p = next(iterator);
	}
	
	return self;
	
}

*/

/* ##### private member definitions ##### */

static int reset(BigInt * self)
{
	if (!self)
	{
		return 1;
	}
	
	Chunk * p = trimChunk(self->first);
	if (p) free(p);
	
	return 0;
}


static int append(BigInt * self, Chunk * chunk)
{
	ERROR(!self || !chunk);
	
	self->length++;
	chunk->prev = self->last;
	self->last->next = chunk;
	self->last = chunk;
	
	return 0;
}



