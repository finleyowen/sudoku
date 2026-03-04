CC = g++
CFLAGS = -Wall

SRC = src/sudoku.cpp
OUT = sudoku

$(OUT): $(SRC)
   	$(CC) $(CFLAGS) -o $(OUT) $(SRC)