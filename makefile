



# Compilation Settings
FLAGS = -Wall
CC = gcc ${FLAGS}



# Targets
DIR = ./build
OBJ = ${DIR}/BigInt.o ${DIR}/Chunk.o
TEXE = ${DIR}/testBigInt.x ${DIR}/testChunk.x



############### BUILD RULES ###############

all: ${DIR} ${OBJ}



${DIR}:
	mkdir -p ${DIR}



${DIR}/BigInt.o: src/BigInt.c src/BigInt.h src/Chunk.h
	${CC} -c -o $@ $<



${DIR}/Chunk.o: src/Chunk.c src/Chunk.h
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



${DIR}/testBigInt.x: ${DIR}/testBigInt.o ${DIR}/BigInt.o ${DIR}/Chunk.o
	${CC} -o $@ $^



${DIR}/testChunk.x:
	@echo
	@echo "testChunk.x not implemented yet"
	@echo



${DIR}/testBigInt.o: test/testBigInt.c test/test.h src/BigInt.c src/BigInt.h
	${CC} -c -o $@ $<



${DIR}/testChunk.o: test/testChunk.c test/test.h src/Chunk.c src/Chunk.h
	${CC} -c -o $@ $<





