# C++ sudoku solver

A sudoku solver written in C++ using graphing algorithms on an implicit unweighted graph.

**This project is under active development. It is currently working, however all the algorithms are extremely slow and none have been successfully executed on test 6 ([input](./tests/inputs/input6), [output](./tests/outputs/output6)) or test 7 ([input](./tests/inputs/), [output](./tests/outputs/output7)) yet for this reason. To attempt to run tests 6 and 7, changed the value of `NTESTS` from 5 to 7 in [sudoku.cpp](./src/sudoku.cpp).**

## Overview

Currently, the program iterates over the test puzzles in the [test folder](./tests) and attempts solve them. The test folder contains the inputs (unsolved puzzles) and corresponding expected outputs (solved puzzles).  

The program throws a runtime error if any of the following occur:
- Either the input or output don't exist or cannot be parsed into a puzzle (see below)
- The algorithm fails to solve the puzzle, e.g., if the puzzle is impossible to solve
- The algorithm solves the puzzle, but the solution is different from the expected solution.

Puzzles are represented textually with columns separated by whitespaces and rows separated
by newlines. Blank cells are represented using the character `x`. The puzzle must be preceded by a line beginning with a dash (`-`); everyhing before this line is a header comment describing the puzzle. See examples in the [test folder](./tests)

## Documentation

Comprehensive API documentation has been written using Doxygen-style doc comments. It can be generated from the repository by running `make docs` from the root directory.

## Getting started

Note the [Makefile](./Makefile) is setup to compile using `g++`. Either ensure you have it installed or edit the Makefile to use your desired C++ compiler. Then, choose from one of the following options:

- Build the executable: `make`
- Build and run the executable: `make run_[alg]` where `alg` is either `bfs`, `dfs`, `bfs_tracked`, or `dfs_tracked`
- Build and run the executable through Valgrind: `make valgrind` (requires [Valgrind](https://valgrind.org/))
- Build the API docs: `make docs` (requires [Doxygen](https://www.doxygen.nl/); find output in `docs` directory)

## Copyright notice

Copyright Finley Owen, 2026. All rights reserved.
