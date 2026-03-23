CC = g++

app:
	$(CC) test/test.cpp -Iinclude -o app

clean:
	rm -f app *.o

run:	
	./app


	