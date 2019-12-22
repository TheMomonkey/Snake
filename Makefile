SRC = snakeAPI.c clientAPI.c arena.c
OBJ = snakeAPI.o clientAPI.o arena.o

all: main test

%.o: %.c
	gcc -c $<

main: $(SRC) main.c
	gcc -g $^ -o $@
	
test: $(SRC) test.c
	gcc -g $^ -o $@

clean:
	rm -f *.o main test
	
.PHONY: all clean
