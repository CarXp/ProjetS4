# DEBUT DU MAKEFILE

EXEC = projet
CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS =

all: $(EXEC)

$(EXEC): main.o couche1.o
	$(CC) -o $(EXEC) main.o couche1.o $(CFLAGS)

main.o: main.c
	$(CC) -c main.c -o main.o $(FLAGS)

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)

# FIN DU MAKEFILE