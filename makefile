CC = gcc

CFLAGS = -std=c99
#CFLAGS += -Wall
#CFLAGS += -g

LDFLAGS = -lm

SRC1 = chatclient.c
SRCS = ${SRC1}

HEADER1 = chatclient.h
HEADERS = ${HEADER1}

PROG1 = chatclient
PROGS = ${PROG1}

DOC1 = plaintext1
DOC2 = plaintext2
DOCS = ${DOC1} ${DOC2}

ZIP = hillyers.project1.zip
	

default:
	${CC} ${CFLAGS} ${SRC1} ${HEADERS} -o ${PROGS} -D_XOPEN_SOURCE

zip:
	zip ${ZIP} ${SRCS} ${HEADERS} ${DOCS} makefile

clean:
	rm -f ${PROGS} *.o *.~