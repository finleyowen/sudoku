# Sudoku solver makefile

# Copyright Finley Owen, 2026. All rights reserved.

CXX = g++
CXXFLAGS = -Wall

SRC_DIR = src
OBJ_DIR = obj
DOC_DIR = doc

SRC = $(SRC_DIR)/main.cc $(SRC_DIR)/algs.cc $(SRC_DIR)/sudoku.cc
OBJ = $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRC))
EXE = sudoku

$(EXE): $(OBJ)
	$(CXX) $(OBJ) -o $(EXE)

obj/%.o: src/%.cc
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

doc: $(SRC)
	$(DOXYGEN) Doxyfile

clean: $(EXE)
	rm -rf $(OBJ_DIR) $(DOC_DIR)