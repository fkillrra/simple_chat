all : simple_server

simple_server : main.o
	g++ -g -o simple_server main.o -lpthread -D_REENTRANT

main.o:
	g++ -g -c -o main.o main.cpp

clean:
	rm -f simple_server
	rm -f *.o

