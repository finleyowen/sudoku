# C++ sudoku solver

**This project is under active development however the initial version is currently working.**

A sudoku solver written in C++ using graphing algorithms on an implicit unweighted graph.

## Overview

Currently, the program iterates over the test puzzles in the [test folder](./tests) and attempts to solve them, outputting the result to `std::cout`.

To add your own test puzzle, add a new file to this directory ensuring the filename copies the format of the other test puzzles (`test{i}` with no file extension), then edit the value of `NTESTS` in [sudoku.cpp](./src/sudoku.cpp) accordingly. 

Puzzles are represented textually with columns separated by whitespaces and rows separated
by newlines. Blank cells are represented using the character `x`. See examples in the [test folder](./tests)

## Documentation

Comprehensive API documentation has been written using Doxygen-style doc comments. It can be generated from the repository by running `make docs` from the root directory.

## Building from source

Note the [Makefile](./Makefile) is setup to compile using `g++`. Either ensure you have it installed or edit the Makefile to use your desired C++ compiler. Then, choose from one of the following options:

- Build the executable: `make`
- Build and run the executable: `make run`
- Build and run the executable through Valgrind: `make valgrind` (requires [Valgrind](https://valgrind.org/))
- Build the API docs: `make docs` (requires [Doxygen](https://www.doxygen.nl/); find output in `docs` directory)

## Copyright notice

Copyright Finley Owen, 2026. All rights reserved.
