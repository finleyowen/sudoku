/**
 * C++ Sudoku solver
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>

/// The number of squares on the board
#define N_SQUARES 9

/// Number of tests puzzles in the tests folder
#define N_TESTS 1

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

/// @brief Represents a square on the board.
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

	SquareValue *getvalue()
	{
		return &value;
	}

	int getintvalue()
	{
		return static_cast<int>(value);
	}

	void setvalue(SquareValue value)
	{
		this->value = value;
	}

	void setintvalue(int value)
	{
		if (value >= 0 && value <= N_SQUARES)
		{
			this->value = static_cast<SquareValue>(value);
		}
	}
};

/// @brief Represents a Sudoke board state.
class SudokuBoard
{
private:
	Square squares[N_SQUARES][N_SQUARES];

	SudokuBoard(Square squares[N_SQUARES][N_SQUARES])
	{
		memcpy(this->squares, squares, N_SQUARES * N_SQUARES * sizeof(Square));
	}

	Square *square(int i, int j)
	{
		return &squares[i][j];
	}

public:
	static SudokuBoard fromistream(std::istream &stream)
	{
		Square squares[N_SQUARES][N_SQUARES];

		for (int i = 0; i < N_SQUARES; i++)
		{
			for (int j = 0; j < N_SQUARES; j++)
			{
				char c;
				stream >> c;
				stream.ignore(1, ' ');

				int val = c == ' ' ? 0 : c - '0';

				squares[i][j].setintvalue(val);
			}

			stream.ignore(1, '\n');
		}

		return SudokuBoard(squares);
	}

	std::ostringstream toostream()
	{
		std::ostringstream ss;
		for (int i = 0; i < N_SQUARES; i++)
		{
			for (int j = 0; j < N_SQUARES; j++)
			{
				ss << squares[i][j].getintvalue() << ' ';
			}
			ss << std::endl;
		}
		return ss;
	}

	SudokuBoard clone()
	{
		Square newSquares[N_SQUARES][N_SQUARES];
		memcpy(newSquares, squares, sizeof(newSquares));
		return SudokuBoard(newSquares);
	}
};

int main(void)
{
	for (int i = 1; i <= N_TESTS; i++)
	{
		std::ostringstream fnameSS;
		fnameSS << "./tests/test" << i;

		std::ifstream file(fnameSS.str());

		SudokuBoard board = SudokuBoard::fromistream(file);

		std::cout << board.toostream().str() << std::endl;
	}
}