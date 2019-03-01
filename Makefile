# DEBUT DU MAKEFILE

EXEC = main
CC = gcc
CFLAGS = -Wall -Werror

all: $(EXEC)

$(EXEC): main.o layer_1.o layer_2.o
	$(CC) -o $(EXEC) main.o layer_1.o layer_2.o $(CFLAGS)

main.o: main.c
	$(CC) -c main.c -o main.o $(FLAGS)

layer_1.o: layer_1.c 
	$(CC) -c layer_1.c -o layer_1.o $(FLAGS)

layer_2.o: layer_2.c 
	$(CC) -c layer_2.c -o layer_2.o $(FLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

# FIN DU MAKEFILE