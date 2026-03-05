/*!
 * @file sudoku.hh
 * @brief Declares classes and methods for representing Sudoku puzzle states.
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */
#pragma once

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <list>
#include <optional>

/// @brief The number of cells in each row and each column
#define NCELLS 9

/// @brief End of the first third of the puzzle
#define T1 3

/// @brief End of the second third of the puzzle
#define T2 6

/// @brief Number of sub-grids that form the main grid
#define NSUBGRIDS 3

/// @brief Number of cells in each sub-grid
#define SUBGRID_SIZE 3

/// @brief The bounds of the subgrids; simply expands out to `{0, 3, 6, 9}`.
constexpr int subgrid_bounds[] = {0, T1, T2, NCELLS};

/// @brief A nullable integer between 1 and 9; represents the value inside a
///		cell.
enum class CellValue
{
	/// @brief Describes the value of a cell that is empty.
	UNSET = 0,
	ONE = 1,
	TWO = 2,
	THREE = 3,
	FOUR = 4,
	FIVE = 5,
	SIX = 6,
	SEVEN = 7,
	EIGHT = 8,
	NINE = 9,
	TEN = 10
};

/// @brief Represents a cell in the puzzle.
class Cell
{
private:
	CellValue value;

	Cell(CellValue value) : value(value) {}

public:
	/// @brief Default constructor; creates empty cell.
	Cell() : value(CellValue::UNSET) {}

	/// @brief Returns a boolean indicating whether the cell is empty.
	bool is_empty();

	/// @brief Gets the value as an integer.
	/// @return The value as an integer.
	int get_intval() const;

	/// @brief Set the value given an integer.
	/// @param value The new value as an integer
	void set_intval(int value);
};

/// @brief Represents a Sudoku puzzle state. Also represents a vertex in the
/// 	implicit graph.
class Puzzle
{
private:
	Cell cells[NCELLS][NCELLS];

	Puzzle(Cell cells[NCELLS][NCELLS]);

	bool validate_row(int i);
	bool validate_col(int j);
	bool validate_subgrid(int start_i, int end_i, int start_j, int end_j);
	Puzzle clone();

public:
	/// @brief Parse a puzzle from an input stream.
	/// @param stream Input stream to parse the puzzle from.
	/// @return Puzzle parsed from the input stream.
	static Puzzle from_stream(std::istream &stream);

	/// @brief Output the puzzle to an output stream.
	/// @param stream Output stream to output the puzzle to.
	void to_stream(std::ostream &stream) const;

	/// @brief Get a bool indicating whether the puzzle is valid.
	/// @return A bool indicating whether the puzzle is valid.
	bool is_valid();

	/// @brief Get a bool indicating whether all cells are filled.
	/// @return A bool indicating whether all cells are filled.
	/// @note This method doesn't check for validity. For the puzzle to be
	///		complete, both \ref Puzzle::is_valid and \ref Puzzle::is_full
	///		must return `true`.
	bool is_full();

	/// @brief Get a vector containing the puzzle states that can be achieved
	///		by altering the value in 1 empty cell in the puzzle. These are the
	/// 	neighbouring nodes in the implicit graph.
	/// @return The neighbouring nodes in the implicit graph.
	std::vector<Puzzle> get_neighbours();

	/// @brief Append the neighbouring nodes to this node in the implicit graph
	/// 	to the back of the queue `q`.
	/// @param q The queue to append the neighbouring nodes to.
	void enqueue_neighbours(std::list<Puzzle> &q);

	/// @brief Get a set of values that could not validly go into the cell in
	///		row `i` and column `j`.
	/// @param i Row index of the cell.
	/// @param j Column index of the cell.
	/// @return A set values that cannot be validly placed in the cell given
	/// 	the current puzzle state.
	std::unordered_set<int> get_invalid(int i, int j);

	/// @brief Get a puzzle state that is guaranteed to be closer to solved than
	///		this one assuming this one is valid and solvable.
	/// @return
	std::optional<Puzzle> get_next_state();

	bool
	operator==(const Puzzle &other) const;
};

// implement a hash function for the Puzzle class so we can store it in an
// unordered_set
namespace std
{
	template <>
	struct hash<Puzzle>
	{
		size_t operator()(const Puzzle &p) const
		{
			std::ostringstream ss;
			p.to_stream(ss);
			return std::hash<string>{}(ss.str());
		}
	};
}
