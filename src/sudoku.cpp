/*!
 * @mainpage C++ Sudoku solver
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <optional>

/// The number of squares on the puzzle
#define NSQUARES 9

/// Number of tests puzzles in the tests folder
#define NTESTS 5

/// End of the first third of the puzzle
#define T1 3

/// End of the last third of the puzzle
#define T2 6

/// @brief The bounds of the subgrids; simply expands out to `{0, 3, 6, 9}`.
constexpr int subgrid_bounds[] = {0, T1, T2, NSQUARES};

/// @brief A nullable integer between 1 and 9; represents the value inside a
///		square.
enum class SquareValue
{
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

/// @brief Represents a square on the puzzle.
class Square
{
private:
	SquareValue value;

	Square(SquareValue value) : value(value) {}

public:
	Square() : value(SquareValue::UNSET) {}

	bool isunset()
	{
		return value == SquareValue::UNSET;
	};

	SquareValue *getval()
	{
		return &value;
	}

	int getintval() const
	{
		return static_cast<int>(value);
	}

	void setval(SquareValue value)
	{
		this->value = value;
	}

	void setintval(int value)
	{
		if (value >= 0 && value <= NSQUARES)
		{
			this->value = static_cast<SquareValue>(value);
		}
	}
};

/// @brief Represents a Sudoke puzzle state.
class Puzzle
{
private:
	Square squares[NSQUARES][NSQUARES];

	Puzzle(Square squares[NSQUARES][NSQUARES])
	{
		memcpy(this->squares, squares, NSQUARES * NSQUARES * sizeof(Square));
	}

	Square *square(int i, int j)
	{
		return &squares[i][j];
	}

	bool validate_row(int i)
	{
		std::unordered_set<int> vals;

		for (int j = 0; j < NSQUARES; j++)
		{
			Square *sq = &squares[i][j];

			// ignore unset squares
			if (!sq->isunset())
			{
				if (vals.count(sq->getintval()))
				{
					// duplicate values in a row => invalid
					return false;
				}
				vals.insert(sq->getintval());
			}
		}

		return true;
	}

	bool validate_col(int j)
	{
		std::unordered_set<int> vals;

		for (int i = 0; i < NSQUARES; i++)
		{
			Square *sq = &squares[i][j];

			// ignore unset squares
			if (!sq->isunset())
			{
				if (vals.count(sq->getintval()))
				{
					// duplicate values in a row => invalid
					return false;
				}
				vals.insert(sq->getintval());
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
				Square *sq = &squares[i][j];

				// ignore unset squares
				if (!sq->isunset())
				{
					if (vals.count(sq->getintval()))
						// duplicate values in a row => invalid
						return false;
					vals.insert(sq->getintval());
				}
			}

		return true;
	}

public:
	static Puzzle
	fromstream(std::istream &stream)
	{
		Square squares[NSQUARES][NSQUARES];

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

	void tostream(std::ostream &os)
	{
		for (int i = 0; i < NSQUARES; i++)
		{
			for (int j = 0; j < NSQUARES; j++)
			{
				int intval = squares[i][j].getintval();
				if (intval == 0)
					std::cout << 'x';
				else
					std::cout << intval;
				std::cout << ' ';
			}

			// no newline after last row
			if (i < NSQUARES - 1)
				os << std::endl;
		}
	}

	Puzzle clone()
	{
		Square newSquares[NSQUARES][NSQUARES];
		memcpy(newSquares, squares, sizeof(newSquares));
		return Puzzle(newSquares);
	}

	bool isvalid()
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

	bool isfull()
	{
		for (int i = 0; i < NSQUARES; i++)
			for (int j = 0; j < NSQUARES; j++)
				if (squares[i][j].isunset())
					return false;
		return true;
	}

	std::vector<Puzzle> getneighbours()
	{
		std::vector<Puzzle> neighbours;
		for (int i = 0; i < NSQUARES; i++)
			for (int j = 0; j < NSQUARES; j++)
				if (squares[i][j].isunset())
					for (int k = 1; k <= NSQUARES; k++)
					{
						Puzzle neighbour = clone();
						neighbour.squares[i][j].setintval(k);

						if (neighbour.isvalid())
							neighbours.push_back(neighbour);
					}
		return neighbours;
	}
};

std::optional<Puzzle> solve(Puzzle *puzzle)
{
	// invalid puzzles shouldn't be passed to this funciton
	if (!puzzle->isvalid())
	{
		throw std::runtime_error("Couldn't solve invalid puzzle!");
	}

	// already checked the puzzle is valid, so if it's full we're done
	if (puzzle->isfull())
		return *puzzle;

	// recursively solve the puzzle
	std::vector<Puzzle> neighbours = puzzle->getneighbours();

	// base case: no neighbours => no solution
	if (!neighbours.size())
		return std::nullopt;

	// recursive case: check if neighbours have solutions
	for (long unsigned int i = 0; i < neighbours.size(); i++)
	{
		std::optional<Puzzle> soln = solve(&neighbours[i]);
		if (soln.has_value())
		{
			return soln;
		}
	}

	// no solution in recursive tree
	return std::nullopt;
}

int main(void)
{
	for (int i = 1; i <= NTESTS; i++)
	{
		// load file
		std::ostringstream fname;
		fname << "./tests/test" << i;
		std::ifstream file(fname.str());

		// parse sudoku puzzle
		Puzzle puzzle = Puzzle::fromstream(file);

		// validate puzzle
		std::cout << "Validating puzzle " << i << ":\n";
		puzzle.tostream(std::cout);
		std::cout << std::endl;

		if (!puzzle.isvalid())
		{
			throw std::runtime_error("The puzzle was invalid!");
		}

		// find solution
		std::cout << "The puzzle was valid!\nSolving it now.\n";
		std::optional<Puzzle> soln = solve(&puzzle);

		if (soln.has_value())
		{
			std::cout << "Found solution:\n";
			soln->tostream(std::cout);
			std::cout << std::endl;
		}
		else
			throw std::runtime_error("Couldn't find solution!");

		if (i <= NTESTS - 1)
			std::cout << "\n\n";
	}
}