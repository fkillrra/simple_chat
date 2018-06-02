all : simple_client

simple_client : main.o
	g++ -g -o simple_client main.o -lpthread -D_REENTRANT

main.o:
	g++ -g -c -o main.o main.cpp

clean:
	rm -f simple_client
	rm -f *.o

