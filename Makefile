CC=g++
CPPFLAGS=--std=c++17 -Wall -DEBUG
LDLIBS=-L/usr/local/opt/fltk -lfltk

all : main

main: main.o huffman.o window.o
	$(CC) -o main main.o huffman.o window.o $(LDLIBS)

main.o: main.cpp huffman.hpp window.hpp
	$(CC) $(CPPFLAGS) -c main.cpp

huffman.o: huffman.hpp huffman.cpp
	$(CC) $(CPPFLAGS) -c huffman.cpp

window.o: window.cpp window.hpp huffman.hpp
	$(CC) $(CPPFLAGS) -c window.cpp

clean:
	rm -rf *.o main
