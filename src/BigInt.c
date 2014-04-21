#include <BigInt.h>
#include <Chunk.h>
#include <stdlib.h>

#define ERROR(x) if(x) return 1
#define MIN(a,b) a < b ? a : b
#define MAX(a,b) a > b ? a : b

/* ##### structures ##### */

struct BigInt
{
	Chunk *first;
	Chunk *last;
	int parity;
	int length; //length in chunks
};



/* ##### private member declarations ##### */

static int reset(BigInt * self);
static int append(BigInt * self, Chunk * chunk);

const static int charsPerUint = 2 * sizeof(unsigned int);
const static char hex[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};

static int uintToHex(char * target, unsigned int uint);




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




int lengthBigInt(BigInt * self)
{
	if (!self) return 0;
	return self->length;
}



int setValue(BigInt * self, int length, unsigned int * value)
{
	
	ERROR(!self || !value);
	
	int numChunks = length / CHUNKSIZE;
	if (length % CHUNKSIZE) numChunks++;
	
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
		chunk->length = MIN(CHUNKSIZE, length);
		
		for (chunkIndex = 0; chunkIndex < chunk->length; chunkIndex++)
		{
			chunk->value[chunkIndex] = value[arrayIndex++];
		}
		
		chunk = chunk->next;
	}
	
	return 0;
	
}



char * toString(BigInt * self)
{
	
	if (!self) return NULL;
	
	int numChars = 1 + self->length * CHUNKSIZE;
	
	char * string = (char *) malloc( sizeof(char) * numChars );
	
	Chunk * chunk;
	
	int index, width = sizeof(char) * charsPerUint;
	char *p = string;
	
	for (chunk = self->first; chunk; chunk = chunk->next)
	{
		for (index = 0; index < chunk->length; index++)
		{
			uintToHex(p, chunk->value[index]);
			p += width;
		}
	}
	
	return string;
	
}



BigInt * add(BigInt * self, BigInt * arg)
{
	
	if (!self || !arg)
	{
		#ifdef BIGINT_DEBUG
		if (!self) printf("first arg to add() is null\n");
		if (!arg)  printf("second arg to add() is null\n");
		#endif
		return NULL;
	}
	
	/* 
	 * This is a pain in the ass. We need to iterate through each of the BigInts
	 * and add arg to self, while watching for overflows so we can carry. 
	 * 
	 * Things we need to watch out for:
	 *   * Chunks that are not full
	 *   * Self is shorter than arg and needs to be extended.
	 *   * arg is shorter than self and we have a trailing carry.
	 * 
	 * Carry detection is accomplished by watching for a sum that is less than
	 * either addend
	 */
	
	
	
	unsigned int sum, carry = 0;
	int li = 0, ri = 0; // left index, right index
	Chunk * lc = self->first; //left chunk
	Chunk * rc = arg->first; // right chunk
	
	if (!lc)
	{
		append(self, newChunk());
		lc = self->first;
		lc->value[0] = 0;
		lc->length++;
	}
	
	//get first index
	while (li >= lc->length) lc = lc->next;
	while (ri >= rc->length) rc = rc->next;
	
	while (rc)
	{
		for (ri = 0; ri < rc->length; ri++)
		{
			
			sum = carry + lc->value[li] + rc->value[ri];
			if (sum < lc->value[li] || sum < rc->value[ri]) carry = 1;
			else carry = 0;
			
			//increment left index
			li++;
			if (li == lc->length) // end of this chunk?
			{
				if (lc->next) // is there a next chunk?
				{
					li = 0;
					lc = lc->next;
					#ifdef BIGINT_DEBUG
					if (li == lc->length);
					{
						printf("empty chunk in add()\n");
						return NULL;
					}
					#endif
				}
				
				else if (lc->length < CHUNKSIZE)
				{
					lc->value[li] = 0; lc->length++;
				}
				
				else
				{
					lc = newChunk();
					append(self,lc);
					lc->value[0] = 0; lc->length++;
				}
			}
		}
		
		//increment right chunk
		do {
			rc = rc->next;
		} while (rc && rc->length);
		
	}
	
	while (carry)
	{
		sum = lc->value[li] + carry;
		if (sum != 0)
		{
			carry = 0;
		}
		
		else //increment left index
		{
			
			li++;
			if (li == lc->length) // end of this chunk?
			{
				if (lc->next) // is there a next chunk?
				{
					li = 0;
					lc = lc->next;
					#ifdef BIGINT_DEBUG
					if (!lc->length);
					{
						printf("empty chunk in add()\n");
						return NULL;
					}
					#endif
				}
				
				else if (lc->length < CHUNKSIZE)
				{
					lc->value[li] = 0; lc->length++;
				}
				
				else
				{
					lc = newChunk();
					append(self,lc);
					lc->value[0] = 0; lc->length++;
				}
				
			} // done finding next real chunk
			
		} //done incrementing
		
	} // done doing final carry
	
	// finally done for good
	return self;
}



/* ##### private member definitions ##### */

static int reset(BigInt * self)
{
	if (!self)
	{
		return 1;
	}
	
	Chunk * p = trimChunk(self->first);
	if (p) free(p);
	
	self->length = 0;
	
	return 0;
}


static int append(BigInt * self, Chunk * chunk)
{
	if (!self || !chunk) return 1;
	
	self->length++;
	chunk->prev = self->last;
	self->last->next = chunk;
	self->last = chunk;
	
	return 0;
}


static int uintToHex(char * target, unsigned int uint)
{
	
	int i;
	
	for (i = 0; i < charsPerUint; i++)
	{
		target[i] = hex[uint & 0x0f];
		uint >>= 4;
	}
	
	target[charsPerUint] = '\0';
	
	return 0;
	
}






