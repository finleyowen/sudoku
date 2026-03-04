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
#include <list>

/// @brief The number of cells in each row and each column
#define NCELLS 9

/// @brief Number of tests puzzles in the tests folder
#define NTESTS 5 // uncomment to run tests 1-5 only
// #define NTESTS 7 // uncomment run all tests

/// @brief End of the first third of the puzzle
#define T1 3

/// @brief End of the second third of the puzzle
#define T2 6

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
	void set_intval(int value)
	{
		if (value >= 0 && value <= NCELLS)
			this->value = static_cast<CellValue>(value);
		else
			throw std::runtime_error("Invalid value!");
	}
};

/// @brief Represents a Sudoku puzzle state. Also represents a vertex in the
/// 	implicit graph.
class Puzzle
{
private:
	Cell cells[NCELLS][NCELLS];

	Puzzle(Cell cells[NCELLS][NCELLS])
	{
		memcpy(this->cells, cells, NCELLS * NCELLS * sizeof(Cell));
	}

	bool validate_row(int i)
	{
		std::unordered_set<int> vals;

		for (int j = 0; j < NCELLS; j++)
		{
			Cell *sq = &cells[i][j];

			// ignore unset cells
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

		for (int i = 0; i < NCELLS; i++)
		{
			Cell *sq = &cells[i][j];

			// ignore unset cells
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
				Cell *sq = &cells[i][j];

				// ignore unset cells
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
		Cell newCells[NCELLS][NCELLS];
		memcpy(newCells, cells, NCELLS * NCELLS * sizeof(Cell));
		return Puzzle(newCells);
	}

public:
	/// @brief Parse a puzzle from an input stream.
	/// @param stream Input stream to parse the puzzle from.
	/// @return Puzzle parsed from the input stream.
	static Puzzle
	from_stream(std::istream &stream)
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

	/// @brief Output the puzzle to an output stream.
	/// @param stream Output stream to output the puzzle to.
	void to_stream(std::ostream &stream) const
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

	/// @brief Get a bool indicating whether the puzzle is valid.
	/// @return A bool indicating whether the puzzle is valid.
	bool is_valid()
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

	/// @brief Get a bool indicating whether the puzzle is valid.
	/// @return A bool indicating whether the puzzle is valid.
	/// @note This method doesn't check for validity. For the puzzle to be
	///		complete, both \ref Puzzle::is_valid and \ref Puzzle::is_full
	///		must return `true`.
	bool is_full()
	{
		for (int i = 0; i < NCELLS; i++)
			for (int j = 0; j < NCELLS; j++)
				if (cells[i][j].is_empty())
					return false;
		return true;
	}

	/// @brief Get a vector containing the puzzle states that can be achieved
	///		by altering the value in 1 empty cell in the puzzle. These are the
	/// 	neighbouring nodes in the implicit graph.
	/// @return The neighbouring nodes in the implicit grapb.
	std::vector<Puzzle> get_neighbours()
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

	/// @brief Append the neighbouring nodes to this node in the implicit graph
	/// 	to the back of the queue `q`.
	/// @param q The queue to append the neighbouring nodes to.
	void enqueue_neighbours(std::list<Puzzle> &q)
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

	bool operator==(const Puzzle &other) const
	{
		return memcmp(cells, other.cells, NCELLS * NCELLS * sizeof(Cell)) == 0;
	}
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

/// @brief Solve a puzzle using recursive DFS.
/// @param puzzle The puzzle to solve (the source node in the DFS).
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be
/// 	solved
std::optional<Puzzle> dfs(Puzzle &u, size_t &nodes_explored)
{
	// incremenet nodes_explored
	nodes_explored++;

	// invalid puzzles shouldn't be passed to this function
	if (!u.is_valid())
		throw std::runtime_error("Couldn't solve invalid puzzle!");

	// already checked the puzzle is valid, so if it's full we're done
	if (u.is_full())
		return u;

	// recursively solve the puzzle
	auto neighbours = u.get_neighbours();

	// base case: no neighbours => no solution
	if (!neighbours.size())
		return std::nullopt;

	// recursive case: check if neighbours have solutions
	for (auto v : neighbours)
	{
		auto soln = dfs(v, nodes_explored);
		if (soln.has_value())
			return soln;
	}

	// no solution in recursive tree
	return std::nullopt;
}

/// @brief Solve a puzzle using recursive DFS, tracking the nodes that have been
/// 	explored.
/// @param u The puzzle to solve (the source node in the DFS).
/// @param seen Set of nodes that have been explored.
/// @param nodes_explored The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be
/// 	solved
std::optional<Puzzle> dfs_tracked(Puzzle &u,
								  std::unordered_set<Puzzle> seen,
								  size_t &nodes_explored)
{
	// incremenet nodes_explored
	nodes_explored++;

	// invalid puzzles shouldn't be passed to this function
	if (!u.is_valid())
		throw std::runtime_error("Couldn't solve invalid puzzle!");

	// already checked the puzzle is valid, so if it's full we're done
	if (u.is_full())
		return u;

	// recursively solve the puzzle
	auto neighbours = u.get_neighbours();

	// base case: no neighbours => no solution
	if (!neighbours.size())
		return std::nullopt;

	// recursive case: check if neighbours have solutions
	for (auto v : neighbours)
	{
		// ignore states that have already been explored
		if (seen.count(v))
			continue;
		seen.insert(v);

		auto soln = dfs(v, nodes_explored);
		if (soln.has_value())
			return soln;
	}

	// no solution in recursive tree
	return std::nullopt;
}

/// @brief Solve a puzzle using queue-based BFS.
/// @param u The puzzle to solve (the source node in the BFS).
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be
/// 	solved
std::optional<Puzzle> bfs(Puzzle &u, size_t &nodes_explored)
{
	// create the queue and add the source node
	std::list<Puzzle> q;
	q.push_back(u);

	// loop while the queue has elements
	while (!q.empty())
	{
		// incremenet nodes_explored
		nodes_explored++;

		// pop a node from the front of the queue
		auto v = q.front();
		q.pop_front();

		// invalid puzzles shouldn't be passed to this function
		if (!v.is_valid())
			throw std::runtime_error("Couldn't solve invalid puzzle!");

		// already checked the puzzle is valid, so if it's full we're done
		if (v.is_full())
			return v;

		// enqueue neighbours for searching
		v.enqueue_neighbours(q);
	}

	// couldn't solve puzzle
	return std::nullopt;
}

/// @brief Solve a puzzle using queue-based BFS and a hashed set to track which
/// 	nodes have been visited.
/// @param u The puzzle to solve (the source node in the BFS).
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be
/// 	solved
std::optional<Puzzle> bfs_tracked(Puzzle &u, size_t &nodes_explored)
{
	// create the queue and add the source node
	std::list<Puzzle> q;
	q.push_back(u);

	// create a hashed set to track the states that have already been examined
	std::unordered_set<Puzzle> seen;

	// loop while the queue has elements
	while (!q.empty())
	{
		// pop a node from the front of the queue
		auto v = q.front();
		q.pop_front();

		// skip the node if it has already been explored
		if (seen.count(v))
			continue;

		// explore the node
		seen.insert(v);
		nodes_explored++;

		// invalid puzzles shouldn't be passed to this function
		if (!v.is_valid())
			throw std::runtime_error("Couldn't solve invalid puzzle!");

		// already checked the puzzle is valid, so if it's full we're done
		if (v.is_full())
			return v;

		// enqueue neighbours for searching
		v.enqueue_neighbours(q);
	}

	// couldn't solve puzzle
	return std::nullopt;
}

/// @brief Solves a puzzle using a specified algorithm.
/// @param u The puzzle to solve.
/// @param alg The algorithm to use to solve the puzzle.
/// @param alg The number of nodes explored.
/// @return The solved puzzle, or `std::nullopt` if the puzzle couldn't be
///		solved.
std::optional<Puzzle> solve(Puzzle &u, std::string alg, size_t &nodes_explored)
{
	if (alg == "bfs")
		return bfs(u, nodes_explored);
	else if (alg == "bfs_tracked")
		return bfs_tracked(u, nodes_explored);
	else if (alg == "dfs")
		return dfs(u, nodes_explored);
	else if (alg == "dfs_tracked")
	{
		std::unordered_set<Puzzle> seen;
		return dfs_tracked(u, seen, nodes_explored);
	}
	else
		throw std::runtime_error("Unrecognised algorithm!");
}

/// @brief Program entry point.
int main(int argc, char **argv)
{
	if (argc <= 1)
		throw std::runtime_error("First argument should be the algorithm name!");

	for (int i = 1; i <= NTESTS; i++)
	{
		// load input file
		std::ostringstream infile_name;
		infile_name << "./tests/inputs/input" << i;
		std::ifstream infile(infile_name.str());

		// parse puzzle
		Puzzle puzzle = Puzzle::from_stream(infile);

		// validate puzzle
		std::cout << "Validating puzzle " << i << ":\n";
		puzzle.to_stream(std::cout);
		std::cout << std::endl;

		if (!puzzle.is_valid())
		{
			throw std::runtime_error("The puzzle was invalid!");
		}

		// find solution
		std::cout << "The puzzle was valid!\nSolving it now using "
				  << argv[1] << std::endl;
		size_t nodes_explored = 0;
		std::optional<Puzzle> output = solve(puzzle, argv[1], nodes_explored);

		if (output.has_value())
		{
			// a solution was found
			std::cout << "Found a solution after exploring "
					  << nodes_explored << " nodes.\n";

			// load expected output from file to puzzle
			std::ostringstream outfile_name;
			outfile_name << "./tests/outputs/output" << i;

			std::ifstream outfile(outfile_name.str());

			std::optional<Puzzle> expected = Puzzle::from_stream(outfile);

			if (!expected.has_value())
				// if no solution was found
				throw std::runtime_error("Couldn't parse expected output!");

			else if (output == expected)
				// if the expected solution was found
				std::cout << "Found expected solution!" << std::endl;

			else
			{
				// an unexpected solution was found
				std::cout << "Found unexpected solution:\n";
				output->to_stream(std::cout);
				throw std::runtime_error("Found unexpected solution!");
			}
		}
		else
			// no solution was found
			throw std::runtime_error("No solution found!");

		if (i <= NTESTS - 1)
			std::cout << "\n\n";
	}
}

//