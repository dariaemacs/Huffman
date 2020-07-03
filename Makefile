CC=g++
CPPFLAGS=--std=c++17 -Wall -Wsign-compare -Werror -Wextra
ILIBS=-I./include
SRC_DIR := src
IDIR := include

all : main

main: main.o huffman.o
	$(CC) -o main main.o huffman.o

main.o: $(SRC_DIR)/main.cpp $(IDIR)/*.hpp
	$(CC) $(CPPFLAGS) $(ILIBS) -c $(SRC_DIR)/main.cpp

huffman.o: $(SRC_DIR)/huffman.cpp $(IDIR)/huffman.hpp
	$(CC) $(CPPFLAGS) $(ILIBS) -c $(SRC_DIR)/huffman.cpp

clean:
	rm -rf *.o main
