/*
 * C++ Sudoku solver
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#pragma once

#include "sudoku.hh"
#include <optional>
#include <string>
#include <unordered_set>

/// @brief Solve a puzzle using recursive DFS.
/// @param u The puzzle to solve (the source node in the DFS).
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be solved.
std::optional<Puzzle> dfs(Puzzle &u, size_t &nodes_explored);

/// @brief Solve a puzzle using recursive DFS, tracking the nodes that have been explored.
/// @param u The puzzle to solve (the source node in the DFS).
/// @param seen Set of nodes that have been explored.
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be solved.
std::optional<Puzzle> dfs_tracked(Puzzle &u,
								  std::unordered_set<Puzzle> seen,
								  size_t &nodes_explored);

/// @brief Solve a puzzle using queue-based BFS.
/// @param u The puzzle to solve (the source node in the BFS).
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be solved.
std::optional<Puzzle> bfs(Puzzle &u, size_t &nodes_explored);

/// @brief Solve a puzzle using queue-based BFS and a hashed set to track which
/// 	nodes have been visited.
/// @param u The puzzle to solve (the source node in the BFS).
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be solved.
std::optional<Puzzle> bfs_tracked(Puzzle &u, size_t &nodes_explored);

/// @brief Solves a puzzle using a specified algorithm.
/// @param u The puzzle to solve.
/// @param alg The algorithm to use ("bfs", "bfs_tracked", "dfs", "dfs_tracked").
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be solved.
std::optional<Puzzle> solve(Puzzle &u, std::string alg, size_t &nodes_explored);
