/*
 * C++ Sudoku solver
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <optional>

/// @brief The number of squares on the puzzle
#define NSQUARES 9

/// @brief Number of tests puzzles in the tests folder
#define NTESTS 5

/// @brief End of the first third of the puzzle
#define T1 3

/// @brief End of the second third of the puzzle
#define T2 6

/// @brief The bounds of the subgrids; simply expands out to `{0, 3, 6, 9}`.
constexpr int subgrid_bounds[] = {0, T1, T2, NSQUARES};

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
	bool is_empty()
	{
		return value == CellValue::UNSET;
	};

	/// @brief Gets the value as an integer.
	/// @return The value as an integer.
	int get_intval() const
	{
		return static_cast<int>(value);
	}

	/// @brief Set the value given an integer.
	/// @param value The new value as an integer
	void setintval(int value)
	{
		if (value >= 0 && value <= NSQUARES)
		{
			this->value = static_cast<CellValue>(value);
		}
	}
};

/// @brief Represents a Sudoku puzzle state.
class Puzzle
{
private:
	Cell squares[NSQUARES][NSQUARES];

	Puzzle(Cell squares[NSQUARES][NSQUARES])
	{
		memcpy(this->squares, squares, NSQUARES * NSQUARES * sizeof(Cell));
	}

	bool validate_row(int i)
	{
		std::unordered_set<int> vals;

		for (int j = 0; j < NSQUARES; j++)
		{
			Cell *sq = &squares[i][j];

			// ignore unset squares
			if (!sq->is_empty())
			{
				if (vals.count(sq->get_intval()))
				{
					// duplicate values in a row => invalid
					return false;
				}
				vals.insert(sq->get_intval());
			}
		}

		return true;
	}

	bool validate_col(int j)
	{
		std::unordered_set<int> vals;

		for (int i = 0; i < NSQUARES; i++)
		{
			Cell *sq = &squares[i][j];

			// ignore unset squares
			if (!sq->is_empty())
			{
				if (vals.count(sq->get_intval()))
				{
					// duplicate values in a row => invalid
					return false;
				}
				vals.insert(sq->get_intval());
			}
		}

		return true;
	}

	bool validate_subgrid(int start_i, int end_i, int start_j, int end_j)
	{
		std::unordered_set<int> vals;

		for (int i = start_i; i < end_i; i++)
			for (int j = start_j; j < end_j; j++)
			{
				Cell *sq = &squares[i][j];

				// ignore unset squares
				if (!sq->is_empty())
				{
					if (vals.count(sq->get_intval()))
						// duplicate values in a row => invalid
						return false;
					vals.insert(sq->get_intval());
				}
			}

		return true;
	}

	Puzzle clone()
	{
		Cell newSquares[NSQUARES][NSQUARES];
		memcpy(newSquares, squares, sizeof(newSquares));
		return Puzzle(newSquares);
	}

public:
	/// @brief Parse a puzzle from an input stream.
	/// @param stream Input stream to parse the puzzle from.
	/// @return Puzzle parsed from the input stream.
	static Puzzle
	from_stream(std::istream &stream)
	{
		Cell squares[NSQUARES][NSQUARES];

		for (int i = 0; i < NSQUARES; i++)
		{
			for (int j = 0; j < NSQUARES; j++)
			{
				char c;
				stream >> c;
				stream.ignore(1, ' ');

				int val = c == 'x' ? 0 : c - '0';

				squares[i][j].setintval(val);
			}

			stream.ignore(1, '\n');
		}

		return Puzzle(squares);
	}

	/// @brief Output the puzzle to an output stream.
	/// @param stream Output stream to output the puzzle to.
	void to_stream(std::ostream &stream)
	{
		for (int i = 0; i < NSQUARES; i++)
		{
			for (int j = 0; j < NSQUARES; j++)
			{
				int intval = squares[i][j].get_intval();
				if (intval == 0)
					std::cout << 'x';
				else
					std::cout << intval;
				std::cout << ' ';
			}

			// no newline after last row
			if (i < NSQUARES - 1)
				stream << std::endl;
		}
	}

	/// @brief Get a bool indicating whether the puzzle is valid.
	/// @return A bool indicating whether the puzzle is valid.
	bool is_valid()
	{
		// check each row and column
		for (int i = 0; i < NSQUARES; i++)
			if (!(validate_row(i) && validate_col(i)))
				return false;

		// check each 3x3 subgrid
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (!validate_subgrid(subgrid_bounds[i], subgrid_bounds[i + 1],
									  subgrid_bounds[j], subgrid_bounds[j + 1]))
					return false;

		return true;
	}

	/// @brief Get a bool indicating whether the puzzle is valid.
	/// @return A bool indicating whether the puzzle is valid.
	/// @note This method doesn't check for validity. For the puzzle to be
	///		complete, both \ref Puzzle::is_valid and \ref Puzzle::is_full
	///		must return `true`.
	bool is_full()
	{
		for (int i = 0; i < NSQUARES; i++)
			for (int j = 0; j < NSQUARES; j++)
				if (squares[i][j].is_empty())
					return false;
		return true;
	}

	/// @brief Get a vector containing the puzzle states that can be achieved
	///		by altering the value in 1 empty cell in the puzzle. These are the
	/// 	neighbouring nodes in the implicit graph.
	/// @return The neighbouring nodes in the implicit grapb.
	std::vector<Puzzle> getneighbours()
	{
		std::vector<Puzzle> neighbours;
		for (int i = 0; i < NSQUARES; i++)
			for (int j = 0; j < NSQUARES; j++)
				if (squares[i][j].is_empty())
					for (int k = 1; k <= NSQUARES; k++)
					{
						Puzzle neighbour = clone();
						neighbour.squares[i][j].setintval(k);

						if (neighbour.is_valid())
							neighbours.push_back(neighbour);
					}
		return neighbours;
	}
};

/// @brief Solve a puzzle using recursive DFS.
/// @param puzzle The puzzle to solve.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be
/// 	solved
std::optional<Puzzle> dfs(Puzzle *puzzle)
{
	// invalid puzzles shouldn't be passed to this funciton
	if (!puzzle->is_valid())
	{
		throw std::runtime_error("Couldn't solve invalid puzzle!");
	}

	// already checked the puzzle is valid, so if it's full we're done
	if (puzzle->is_full())
		return *puzzle;

	// recursively solve the puzzle
	std::vector<Puzzle> neighbours = puzzle->getneighbours();

	// base case: no neighbours => no solution
	if (!neighbours.size())
		return std::nullopt;

	// recursive case: check if neighbours have solutions
	for (long unsigned int i = 0; i < neighbours.size(); i++)
	{
		std::optional<Puzzle> soln = dfs(&neighbours[i]);
		if (soln.has_value())
		{
			return soln;
		}
	}

	// no solution in recursive tree
	return std::nullopt;
}

/// @brief Program entry point.
int main(void)
{
	for (int i = 1; i <= NTESTS; i++)
	{
		// load file
		std::ostringstream fname;
		fname << "./tests/test" << i;
		std::ifstream file(fname.str());

		// parse sudoku puzzle
		Puzzle puzzle = Puzzle::from_stream(file);

		// validate puzzle
		std::cout << "Validating puzzle " << i << ":\n";
		puzzle.to_stream(std::cout);
		std::cout << std::endl;

		if (!puzzle.is_valid())
		{
			throw std::runtime_error("The puzzle was invalid!");
		}

		// find solution
		std::cout << "The puzzle was valid!\nSolving it now.\n";
		std::optional<Puzzle> soln = dfs(&puzzle);

		if (soln.has_value())
		{
			std::cout << "Found solution:\n";
			soln->to_stream(std::cout);
			std::cout << std::endl;
		}
		else
			throw std::runtime_error("Couldn't find solution!");

		if (i <= NTESTS - 1)
			std::cout << "\n\n";
	}
}