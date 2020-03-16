CC = gcc
CFLAGS = -W -Wall -pedantic -ansi
LDFLAGS = -lreadline -lm -lSDL -lSDL_ttf
EXEC = conway
SRC = $(wildcard src/*.c)
OBJ = $(SRC: *.c = *.o)

all : $(EXEC)

help :
	@echo ""
	@echo "Makefile utilisation :"
	@echo "make all -> compile everything"
	@echo "make help -> give help with makefile"
	@echo "make clean -> remove all object files"
	@echo "make mrproper -> remove all executable files"
	@echo ""

conway : $(OBJ)
	@$(CC) $^ -o $@ $(LDFLAGS)

*.o : *.c
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY : clean

.PHONY : mrproper

clean :
	@rm -rf *.o

mrproper : clean
	@rm -rf $(EXEC)
