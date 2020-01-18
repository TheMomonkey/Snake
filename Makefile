SRC = snakeAPI.c clientAPI.c arena.c
OBJ = snakeAPI.o clientAPI.o arena.o

all: mainASadouki

%.o: %.c
	gcc -c $<

mainASadouki: $(SRC) mainASadouki.c
	gcc -g $^ -o $@

clean:
	rm -f *.o mainASadouki
	
.PHONY: all clean
