#ifndef BIGINT_H
#define BIGINT_H

#include <stdio.h>

/* ##### types ##### */

typedef struct BigInt BigInt;



/* ##### methods ##### */

/** Allocates memory for a new big int object
 *  returns new BigInt object initialized to zero.
 */
BigInt * newBigInt(void);
void freeBigInt(BigInt * self);
int lengthBigInt(BigInt * self);

/** Sets value of a BigInt object to the value in the third argument.
 *  @param self the object the value will be copied into
 *  @param length the length of the value array
 *  @param value a little endian list of int values representing the value
 */
int setValue(BigInt * self, int length, unsigned int * value);
char * toString(BigInt * self);
BigInt * add(BigInt * self, BigInt * bi);



#endif
