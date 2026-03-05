/*
 * C++ Sudoku solver
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include "sudoku.hh"

// =============================================================================
// Cell
// =============================================================================

bool Cell::is_empty()
{
	return value == CellValue::UNSET;
}

int Cell::get_intval() const
{
	return static_cast<int>(value);
}

void Cell::set_intval(int value)
{
	if (value >= 0 && value <= NCELLS)
		this->value = static_cast<CellValue>(value);
	else
		throw std::runtime_error("Invalid value!");
}

// ─── Puzzle ──────────────────────────────────────────────────────────────────

Puzzle::Puzzle(Cell cells[NCELLS][NCELLS])
{
	memcpy(this->cells, cells, NCELLS * NCELLS * sizeof(Cell));
}

bool Puzzle::validate_row(int i)
{
	std::unordered_set<int> vals;

	for (int j = 0; j < NCELLS; j++)
	{
		Cell *sq = &cells[i][j];

		// ignore unset cells
		if (!sq->is_empty())
		{
			if (vals.count(sq->get_intval()))
				return false; // duplicate values in a row => invalid
			vals.insert(sq->get_intval());
		}
	}

	return true;
}

bool Puzzle::validate_col(int j)
{
	std::unordered_set<int> vals;

	for (int i = 0; i < NCELLS; i++)
	{
		Cell *sq = &cells[i][j];

		// ignore unset cells
		if (!sq->is_empty())
		{
			if (vals.count(sq->get_intval()))
				return false; // duplicate values in a column => invalid
			vals.insert(sq->get_intval());
		}
	}

	return true;
}

bool Puzzle::validate_subgrid(int start_i, int end_i, int start_j, int end_j)
{
	std::unordered_set<int> vals;

	for (int i = start_i; i < end_i; i++)
		for (int j = start_j; j < end_j; j++)
		{
			Cell *sq = &cells[i][j];

			// ignore unset cells
			if (!sq->is_empty())
			{
				if (vals.count(sq->get_intval()))
					return false; // duplicate values in subgrid => invalid
				vals.insert(sq->get_intval());
			}
		}

	return true;
}

Puzzle Puzzle::clone()
{
	Cell newCells[NCELLS][NCELLS];
	memcpy(newCells, cells, NCELLS * NCELLS * sizeof(Cell));
	return Puzzle(newCells);
}

Puzzle Puzzle::from_stream(std::istream &stream)
{
	Cell cells[NCELLS][NCELLS];

	std::string line;
	while (std::getline(stream, line) && line[0] != '-')
		;

	for (int i = 0; i < NCELLS; i++)
	{
		for (int j = 0; j < NCELLS; j++)
		{
			char c;
			stream >> c;
			stream.ignore(1, ' ');

			int val = c == 'x' ? 0 : c - '0';

			cells[i][j].set_intval(val);
		}

		stream.ignore(1, '\n');
	}

	auto puzzle = Puzzle(cells);
	if (!puzzle.is_valid())
		throw std::runtime_error("Invalid puzzle!");

	return puzzle;
}

void Puzzle::to_stream(std::ostream &stream) const
{
	for (int i = 0; i < NCELLS; i++)
	{
		stream << i + 1 << ": ";
		for (int j = 0; j < NCELLS; j++)
		{
			int intval = cells[i][j].get_intval();
			if (intval == 0)
				stream << 'x';
			else
				stream << intval;
			stream << ' ';
		}

		// no newline after last row
		if (i < NCELLS - 1)
			stream << std::endl;
	}
}

bool Puzzle::is_valid()
{
	// check each row and column
	for (int i = 0; i < NCELLS; i++)
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

bool Puzzle::is_full()
{
	for (int i = 0; i < NCELLS; i++)
		for (int j = 0; j < NCELLS; j++)
			if (cells[i][j].is_empty())
				return false;
	return true;
}

std::vector<Puzzle> Puzzle::get_neighbours()
{
	std::vector<Puzzle> neighbours;
	for (int i = 0; i < NCELLS; i++)
		for (int j = 0; j < NCELLS; j++)
			if (cells[i][j].is_empty())
				for (int k = 1; k <= NCELLS; k++)
				{
					Puzzle neighbour = clone();
					neighbour.cells[i][j].set_intval(k);

					if (neighbour.is_valid())
						neighbours.push_back(neighbour);
				}
	return neighbours;
}

void Puzzle::enqueue_neighbours(std::list<Puzzle> &q)
{
	for (int i = 0; i < NCELLS; i++)
		for (int j = 0; j < NCELLS; j++)
			if (cells[i][j].is_empty())
				for (int k = 1; k <= NCELLS; k++)
				{
					Puzzle neighbour = clone();
					neighbour.cells[i][j].set_intval(k);

					if (neighbour.is_valid())
						q.push_back(neighbour);
				}
}

bool Puzzle::operator==(const Puzzle &other) const
{
	return memcmp(cells, other.cells, NCELLS * NCELLS * sizeof(Cell)) == 0;
}
