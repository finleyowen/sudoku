CC = g++
CFLAGS = -Wall

VALGRIND = valgrind
DOXYGEN = doxygen

SRC = src/sudoku.cpp
EXE = sudoku

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $(EXE) $(SRC)

run_dfs: $(EXE)
	./$(EXE) dfs

run_dfs_tracked: $(EXE)
	./$(EXE) dfs_tracked

run_bfs: $(EXE)
	./$(EXE) bfs

run_bfs_tracked: $(EXE)
	./$(EXE) bfs_tracked

valgrind: $(EXE)
	$(VALGRIND) ./$(EXE)

docs: $(SRC)
	$(DOXYGEN) Doxyfile

clean: $(EXE)
	rm $(EXE)