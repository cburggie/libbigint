#ifndef CHUNK_H
#define CHUNK_H

#include <stdlib.h>

#define CHUNKSIZE 10

/* ##### types ##### */

typedef struct Chunk Chunk;



/* ##### methods ##### */

Chunk * newChunk(void);
Chunk * trimChunk(Chunk * self);



/* ##### structures ##### */

struct Chunk
{
	Chunk * prev;
	Chunk * next;
	int length;
	unsigned int value[CHUNKSIZE];
};

#endif

