



# Compilation Settings
CINC = -I ./include
CDEF = -DBIGINT_DEBUG
CFLAGS = -Wall
COPT = ${CFLAGS} ${CDEF} ${CINC}
CC = gcc ${COPT}



# Targets
DIR = ./build
HDR = ./include/BigInt.h ./include/Chunk.h
OBJ = ${DIR}/BigInt.o ${DIR}/Chunk.o
TEXE = ${DIR}/testBigInt.x ${DIR}/testChunk.x



############### BUILD RULES ###############

all: ${DIR} ${OBJ}



${DIR}:
	mkdir -p ${DIR}



${DIR}/BigInt.o: src/BigInt.c ${HDR}
	${CC} -c -o $@ $<



${DIR}/Chunk.o: src/Chunk.c include/Chunk.h
	${CC} -c -o $@ $<


clean:
	@echo
	@echo "cleaning build directory"
	rm -rf ${OBJ} ${TEXE}
	@echo



############### TEST SUITE ###############

test: ${DIR} ${TEXE}
	@echo
	@echo "running Big Int tests:"
	@echo
	@./${DIR}/testBigInt.x
	@echo



${DIR}/testBigInt.x: ${DIR}/testBigInt.o ${OBJ}
	${CC} -o $@ $^



${DIR}/testChunk.x:
	@echo
	@echo "testChunk.x not implemented yet"
	touch $@
	@echo



${DIR}/testBigInt.o: test/testBigInt.c test/test.h include/BigInt.h
	${CC} -c -o $@ $<



${DIR}/testChunk.o: test/testChunk.c test/test.h src/Chunk.c include/Chunk.h
	${CC} -c -o $@ $<





