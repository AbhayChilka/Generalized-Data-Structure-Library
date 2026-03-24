CC = g++
CFLAGS = -Iinclude -Wall

SRC = test/test.cpp
OBJ = test/test.o

app: $(OBJ)
	$(CC) $(OBJ) -o app

test/test.o: test/test.cpp
	$(CC) $(CFLAGS) -c test/test.cpp -o test/test.o

clean:
	rm -f app *.o test/*.o

run:	
	./app
