# Makefile
cc = gcc

BIN = snprintf
MAIN = main.c
SRC = *.c

all: $(BIN)
snprintf: snprintf.c $(MAIN)
	$(cc) -g -o $@ $^
clean:
	rm $(BIN)
