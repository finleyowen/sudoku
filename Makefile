CC = g++
CFLAGS = -Wall

VALGRIND = valgrind
DOXYGEN = doxygen

SRC = src/sudoku.cpp
OUT = sudoku

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	$(VALGRIND) ./$(OUT)

docs: $(SRC)
	$(DOXYGEN) Doxyfile

clean: $(OUT)
	rm $(OUT)