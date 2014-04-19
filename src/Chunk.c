#include "./Chunk.h"


/* ##### private method declarations ##### */



/* ##### public method definitions ##### */

Chunk * newChunk(void)
{
	Chunk * self = (Chunk *) malloc(sizeof(Chunk));
	self->prev = NULL;
	self->next = NULL;
	self->length = 0;
	
	return self;
}


Chunk * trimChunk(Chunk * self)
{
	
	if (!self) return NULL;
	
	Chunk * current = self->next;
	Chunk * next;
	
	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}
	
	self->next = NULL;
	return self;
}



/* ##### private method definitions ##### */
