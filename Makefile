CC=g++
CPPFLAGS=--std=c++17 -Wall -Wsign-compare -Werror -Wextra
ILIBS=-I./include
LDLIBS=-L/usr/local/opt/fltk -lfltk
SRC_DIR := src
IDIR := include

all : main

main: main.o huffman.o window.o
	$(CC) -o main main.o huffman.o window.o $(LDLIBS)

main.o: $(SRC_DIR)/main.cpp $(IDIR)/*.hpp
	$(CC) $(CPPFLAGS) $(ILIBS) -c $(SRC_DIR)/main.cpp

huffman.o: $(SRC_DIR)/huffman.cpp $(IDIR)/huffman.hpp
	$(CC) $(CPPFLAGS) $(ILIBS) -c $(SRC_DIR)/huffman.cpp

window.o: $(SRC_DIR)/window.cpp $(IDIR)/*.hpp
	$(CC) $(CPPFLAGS) $(ILIBS) -c $(SRC_DIR)/window.cpp

clean:
	rm -rf *.o main
