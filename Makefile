CXX = g++
CXXFLAGS = -Wall

VALGRIND = valgrind
DOXYGEN = doxygen

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DOC_DIR = doc

SRC = $(SRC_DIR)/main.cc $(SRC_DIR)/algs.cc $(SRC_DIR)/sudoku.cc
OBJ = $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRC))
EXE = $(BIN_DIR)/sudoku

$(EXE): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ) -o $(EXE)

obj/%.o: src/%.cc
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

run_dfs: $(EXE)
	$(EXE) dfs

run_dfs_tracked: $(EXE)
	$(EXE) dfs_tracked

run_bfs: $(EXE)
	$(EXE) bfs

run_bfs_tracked: $(EXE)
	$(EXE) bfs_tracked

valgrind_dfs: $(EXE)
	$(VALGRIND) $(EXE) dfs

valgrind_dfs_tracked: $(EXE)
	$(VALGRIND) $(EXE) dfs_tracked

valgrind_bfs: $(EXE)
	$(VALGRIND) $(EXE) bfs

valgrind_bfs_tracked: $(EXE)
	$(VALGRIND) $(EXE) bfs_tracked

doc: $(SRC)
	$(DOXYGEN) Doxyfile

clean: $(EXE)
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DOC_DIR)