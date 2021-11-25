# to run in terminal $ make
CC = gcc
CFLAGS = -ggdb -g -Wall -Wextra

# List all your .c (or .cpp or .cc) files here (source files, excluding header files)
SRC_CODE = driver.c lex.o parser.c vm.o

#################################

# default rule

all: $(SRC_CODE)
	$(CC) ${CFLAGS} $(SRC_CODE) -lm
	