#include <BigInt.h>
#include <Chunk.h>
#include <stdlib.h>

#define ERROR(x) if(x) return 1
#define MIN(a,b) a < b ? a : b
#define MAX(a,b) a > b ? a : b

/* ##### types ##### */
typedef struct Iterator Iterator;

/* ##### structures ##### */

struct BigInt
{
	Chunk *first;
	Chunk *last;
	int parity;
	int length; //length in chunks
};

struct Iterator
{
	BigInt * obj;
	Chunk * chunk;
	int index;
};



/* ##### private member declarations ##### */

static Iterator * iterate(BigInt * obj);
static Iterator * IT_next(Iterator * self);
static Iterator * IT_next_with_extend(Iterator * self);
static Iterator * IT_set(Iterator * self, unsigned int value);
static unsigned int IT_get(Iterator * self);

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
	
	
	
	unsigned int sum, carry = 0, lv, rv;
	
	Iterator * li = iterate(self);
	Iterator * ri = iterate(arg);
	
	if (!li->chunk)
	{
		li->chunk = newChunk();
		li->chunk->value[0] = 0;
		li->chunk->length++;
		append(self, li->chunk);
	}
	
	while (ri->chunk)
	{
		lv = IT_get(li);
		rv = IT_get(ri);
		
		sum = carry + lv + rv;
		
		if (sum < lv || sum < rv) carry = 1;
		else carry = 0;
		
		IT_set(li, sum);
		IT_next_with_extend(li);
		IT_next(ri);
	}
	
	while (carry)
	{
		lv = IT_get(li);
		
		sum = carry + lv;
		
		IT_set(li, sum);
		
		if (sum > 0) carry = 0;
		else IT_next_with_extend(li);
		
	} // done doing final carry
	
	return self;
}



/* ##### private member definitions ##### */

static Iterator * iterate(BigInt * obj)
{
	if (!obj) return NULL;
	
	Iterator * self = (Iterator *) malloc(sizeof(Iterator));
	if (!self)
	{
		printf("Memory allocation error.\n");
		return NULL;
	}
	
	self->obj = obj;
	self->chunk = obj->first;
	self->index = 0;
	while (self->chunk && !self->chunk->length)
	{
		IT_next(self);
	}
	
	return self;
	
}



static Iterator * IT_next(Iterator * self)
{
	if (!self)
	{
		#ifdef BIGINT_DEBUG
		printf("IT_next() passed null argument\n");
		#endif
		return NULL;
	}
	
	if (!self->chunk) return NULL;
	
	self->index++;
	
	if (self->index >= self->chunk->length) // end of this chunk?
	{
		
		self->index = 0;
		do {
			
			self->chunk = self->chunk->next;
			
		} while (self->chunk && !self->chunk->length);
		
	}
	
	if (!self->chunk) return NULL;
	
	return self;
}



static Iterator * IT_next_with_extend(Iterator * self)
{
	if (!self) return NULL;
	
	if (!IT_next(self))
	{
		self->chunk = self->obj->last;
		
		if (!self->chunk && self->chunk->length < CHUNKSIZE)
		{
			self->index = self->chunk->length;
			self->chunk->length++;
			return self;
		}
		
		else
		{
			self->chunk = newChunk();
			
			self->chunk->value[0] = 0;
			self->chunk->length++;
			self->index = 0;
			
			append(self->obj, self->chunk);
		}
	}
	
	return self;
}



static Iterator * IT_set(Iterator * self, unsigned int value)
{
	if (!self || !self->chunk) return NULL;
	
	self->chunk->value[self->index] = value;
	
	return self;
}



static unsigned int IT_get(Iterator * self)
{
	if (!self) return 0;
	
	if (self->chunk)
	{
		return self->chunk->value[self->index];
	}
	
	else return 0;
}



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






