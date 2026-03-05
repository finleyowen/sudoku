# C++ sudoku solver

A sudoku solver written in C++ using graphing algorithms on an implicit unweighted graph.

**This project is under active development. It is currently working, however most the algorithms are very slow and none have been successfully executed on all of the tests yet for this reason. The stack-based DFS seems to be the fastest of the algorithms currently implemented.**

## Overview and usage

The algorithms currently implemented are:
- `dfs` - depth-first search
- `dfs_tracked` - depth-first search that avoids exploring duplicate states
- `dfs_spec` - specialised depth-first search that only explores states that are guaranteed to be closer to the solution that the current state assuming the current state is valid and solvable
- `dfs_stack` - stack-based depth-first search that only explores states unique states
- `bfs` - breadth-first search
- `bfs_tracked` - breadth-first search that avoids exploring duplicate states

After building from source with `make`, there are two options for running the code:
- `bin/sudoku test [alg] [test_number]` tests the algorithm `alg` on the input/output pair `tests/inputs/input[test_number]`, `tests/outputs/output[test_number]`.
- `bin/sudoku run [alg] [file_path]` runs the algorithm `alg` on the incomplete puzzle stored at `file_path`. Outputs the solution to `std::cout` if one can be found.
- `bin/sudoku run [alg]` runs the algorithm `alg` on an incomplete puzzle passed into `std::cin`. Outputs the solution to `std::cout` if one can be found.

Test puzzles are provided in the [tests](./tests/) folder. The incomplete puzzles are in the inputs subdirectory and the completed puzzles are in the outputs subdirectory.

## Documentation

Comprehensive API documentation has been written using Doxygen-style doc comments. It can be generated from the repository by running `make docs` from the root directory.

## Getting started

Note the [Makefile](./Makefile) is setup to compile using `g++`. Either ensure you have it installed or edit the Makefile to use your desired C++ compiler. Then, choose from one of the following options:

- Build the executable: `make`
- Build and run the executable: `make run_[alg]` where `alg` is either `bfs`, `dfs`, `bfs_tracked`, or `dfs_tracked`
- Build and run the executable through Valgrind: `make valgrind_[alg]` (requires [Valgrind](https://valgrind.org/))
- Build the API docs: `make doc` (requires [Doxygen](https://www.doxygen.nl/); find output in `docs` directory)

You can also `make clean` to remove the object files, executable, and docs if they are built.

## Copyright notice

Copyright Finley Owen, 2026. All rights reserved.
