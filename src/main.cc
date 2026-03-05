/*
 * C++ Sudoku solver
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include "algs.hh"
#include "sudoku.hh"

static void print_usage(const char *prog)
{
	std::cerr << "Usage:\n"
			  << "  " << prog << " test <alg> <test_number>  -- test alg against tests/inputs/input<N> and tests/outputs/output<N>\n"
			  << "  " << prog << " run  <alg> [file_path]    -- run alg on a puzzle from file_path, or stdin if omitted\n";
}

/// @brief Run an algorithm on a puzzle read from `stream`, print the result.
static void run_mode(const std::string &alg, std::istream &stream)
{
	Puzzle puzzle = Puzzle::from_stream(stream);

	if (!puzzle.is_valid())
		throw std::runtime_error("The puzzle is invalid!");

	std::cout << "Solving puzzle using " << alg << "...\n";

	size_t nodes_explored = 0;
	std::optional<Puzzle> output = solve(puzzle, alg, nodes_explored);

	if (!output.has_value())
		throw std::runtime_error("No solution found!");

	std::cout << "Found a solution after exploring " << nodes_explored << " nodes.\n";
	output->to_stream(std::cout);
	std::cout << std::endl;
}

/// @brief Test an algorithm against the expected output for a given test number.
static void test_mode(const std::string &alg, int test_number)
{
	// Load input
	std::ostringstream infile_name;
	infile_name << "./tests/inputs/input" << test_number;
	std::ifstream infile(infile_name.str());
	if (!infile.is_open())
		throw std::runtime_error("Could not open input file: " + infile_name.str());

	Puzzle puzzle = Puzzle::from_stream(infile);

	std::cout << "Puzzle " << test_number << ":\n";
	puzzle.to_stream(std::cout);
	std::cout << std::endl;

	if (!puzzle.is_valid())
		throw std::runtime_error("The puzzle is invalid!");

	std::cout << "Solving using " << alg << "...\n";

	size_t nodes_explored = 0;
	std::optional<Puzzle> output = solve(puzzle, alg, nodes_explored);

	if (!output.has_value())
		throw std::runtime_error("No solution found!");

	std::cout << "Found a solution after exploring " << nodes_explored << " nodes.\n";

	// Load expected output
	std::ostringstream outfile_name;
	outfile_name << "./tests/outputs/output" << test_number;
	std::ifstream outfile(outfile_name.str());
	if (!outfile.is_open())
		throw std::runtime_error("Could not open output file: " + outfile_name.str());

	std::optional<Puzzle> expected = Puzzle::from_stream(outfile);

	if (!expected.has_value())
		throw std::runtime_error("Couldn't parse expected output!");

	if (output == expected)
	{
		std::cout << "PASS: solution matches expected output." << std::endl;
	}
	else
	{
		std::cout << "FAIL: unexpected solution:\n";
		output->to_stream(std::cout);
		std::cout << std::endl;
		throw std::runtime_error("Solution does not match expected output!");
	}
}

/// @brief Program entry point.
int main(int argc, char **argv)
{
	if (argc < 3)
	{
		print_usage(argv[0]);
		return 1;
	}

	const std::string subcommand = argv[1];
	const std::string alg = argv[2];

	if (subcommand == "test")
	{
		if (argc != 4)
		{
			std::cerr << "Error: 'test' requires exactly two arguments: <alg> <test_number>\n";
			print_usage(argv[0]);
			return 1;
		}

		int test_number;
		try
		{
			test_number = std::stoi(argv[3]);
		}
		catch (const std::exception &)
		{
			std::cerr << "Error: test_number must be an integer, got \"" << argv[3] << "\"\n";
			return 1;
		}

		test_mode(alg, test_number);
	}
	else if (subcommand == "run")
	{
		if (argc == 3)
		{
			// Read from stdin
			run_mode(alg, std::cin);
		}
		else if (argc == 4)
		{
			// Read from file
			const std::string file_path = argv[3];
			std::ifstream file(file_path);
			if (!file.is_open())
				throw std::runtime_error("Could not open file: " + file_path);
			run_mode(alg, file);
		}
		else
		{
			std::cerr << "Error: 'run' takes at most one optional argument: [file_path]\n";
			print_usage(argv[0]);
			return 1;
		}
	}
	else
	{
		std::cerr << "Error: unknown subcommand \"" << subcommand << "\"\n";
		print_usage(argv[0]);
		return 1;
	}

	return 0;
}
