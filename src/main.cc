/*!
 * @file main.cc
 * @brief Program entry point.
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "algs.hh"
#include "sudoku.hh"

static void print_usage(const char *prog)
{
	std::cerr << "Usage:\n"
			  << "  " << prog << " test  <alg> <test_number>       -- test alg against tests/inputs/input<N> and tests/outputs/output<N>\n"
			  << "  " << prog << " run   <alg> [file_path]         -- run alg on a puzzle from file_path, or stdin if omitted\n"
			  << "  " << prog << " check <puzzle_path> <soln_path>  -- check whether soln_path is a valid solution to the puzzle in puzzle_path\n"
			  << "  " << prog << " suite [output_csv_path] [timeout_seconds]  -- run all tests against all algorithms, write nodes_explored to CSV (defaults: benchmarks.csv, 5s; null on timeout)\n";
}

/// @brief Run a single algorithm on a puzzle with a timeout.
/// @param puzzle The puzzle to solve.
/// @param alg    The algorithm name.
/// @param timeout_seconds Maximum seconds to wait.
/// @return nodes_explored, or std::nullopt if timed out or failed.
static std::optional<size_t> run_with_timeout(Puzzle puzzle, const std::string &alg, int timeout_seconds)
{
	// Shared state between this thread and the worker.
	// We use a mutex + condvar so the worker can signal completion,
	// and the main thread can give up after the timeout without blocking.
	auto mutex = std::make_shared<std::mutex>();
	auto cv = std::make_shared<std::condition_variable>();
	auto done = std::make_shared<bool>(false);
	auto result = std::make_shared<std::optional<size_t>>(std::nullopt);

	// Launch a detached thread — we never join it, so it truly runs independently
	// and the main thread can return freely once the timeout fires.
	std::thread worker([puzzle, alg, mutex, cv, done, result]() mutable
					   {
		std::optional<size_t> local_result;
		try
		{
			size_t nodes_explored = 0;
			solve(puzzle, alg, nodes_explored);
			local_result = nodes_explored;
		}
		catch (...) {}

		std::lock_guard<std::mutex> lock(*mutex);
		*result = local_result;
		*done = true;
		cv->notify_one(); });
	worker.detach();

	// Wait up to timeout_seconds for the worker to finish.
	std::unique_lock<std::mutex> lock(*mutex);
	cv->wait_for(lock, std::chrono::seconds(timeout_seconds), [&done]
				 { return *done; });

	if (*done)
		return *result;

	// Timed out — worker is still running in the background; we just move on.
	return std::nullopt;
}

/// @brief Discover all test input files in ./tests/inputs/ and return their indices.
static std::vector<int> find_test_indices()
{
	std::vector<int> indices;
	const std::string dir = "./tests/inputs";
	for (const auto &entry : std::filesystem::directory_iterator(dir))
	{
		const std::string name = entry.path().filename().string();
		const std::string prefix = "input";
		if (name.rfind(prefix, 0) == 0)
		{
			try
			{
				int idx = std::stoi(name.substr(prefix.size()));
				indices.push_back(idx);
			}
			catch (...)
			{
			}
		}
	}
	std::sort(indices.begin(), indices.end());
	return indices;
}

/// @brief Run all tests against all algorithms and write a CSV of nodes_explored.
static void suite_mode(const std::string &csv_path, int timeout_seconds)
{
	static const std::vector<std::string> ALGS = {
		"bfs", "bfs_tracked", "dfs", "dfs_spec", "dfs_stack", "dfs_tracked"};

	std::vector<int> test_indices = find_test_indices();
	if (test_indices.empty())
	{
		std::cerr << "No test inputs found in ./tests/inputs/\n";
		return;
	}

	// Collect results: results[test_idx][alg_idx] = nodes_explored or nullopt
	std::vector<std::vector<std::optional<size_t>>> results(
		test_indices.size(), std::vector<std::optional<size_t>>(ALGS.size()));

	for (size_t ti = 0; ti < test_indices.size(); ++ti)
	{
		int n = test_indices[ti];
		std::ostringstream infile_name;
		infile_name << "./tests/inputs/input" << n;
		std::ifstream infile(infile_name.str());
		if (!infile.is_open())
		{
			std::cerr << "Warning: could not open " << infile_name.str() << ", skipping.\n";
			continue;
		}

		Puzzle puzzle = Puzzle::from_stream(infile);
		if (!puzzle.is_valid())
		{
			std::cerr << "Warning: puzzle " << n << " is invalid, skipping.\n";
			continue;
		}

		for (size_t ai = 0; ai < ALGS.size(); ++ai)
		{
			const std::string &alg = ALGS[ai];
			std::cerr << "Running " << alg << " on test " << n << "... " << std::flush;
			auto res = run_with_timeout(puzzle, alg, timeout_seconds);
			if (res.has_value())
				std::cerr << res.value() << " nodes\n";
			else
				std::cerr << "timeout\n";
			results[ti][ai] = res;
		}
	}

	// Write CSV to the specified file path
	std::ofstream csv(csv_path);
	if (!csv.is_open())
		throw std::runtime_error("Could not open CSV output file: " + csv_path);

	// Header row: test, <alg1>, <alg2>, ...
	csv << "test";
	for (const auto &alg : ALGS)
		csv << "," << alg;
	csv << "\n";

	for (size_t ti = 0; ti < test_indices.size(); ++ti)
	{
		csv << test_indices[ti];
		for (size_t ai = 0; ai < ALGS.size(); ++ai)
		{
			csv << ",";
			if (results[ti][ai].has_value())
				csv << results[ti][ai].value();
			// else: empty field (null)
		}
		csv << "\n";
	}

	std::cerr << "Results written to " << csv_path << "\n";
}

/// @brief Run an algorithm on a puzzle read from `stream`, print the result.
static void run_mode(const std::string &alg, std::istream &stream)
{
	Puzzle puzzle = Puzzle::from_stream(stream);

	if (!puzzle.is_valid())
		throw invalid_puzzle_error(puzzle);

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

/// @brief Check whether the puzzle in `soln_path` is a valid solution to the puzzle in `puzzle_path`.
static void check_mode(const std::string &puzzle_path, const std::string &soln_path)
{
	std::ifstream puzzle_file(puzzle_path);
	if (!puzzle_file.is_open())
		throw std::runtime_error("Could not open puzzle file: " + puzzle_path);

	std::ifstream soln_file(soln_path);
	if (!soln_file.is_open())
		throw std::runtime_error("Could not open solution file: " + soln_path);

	Puzzle puzzle = Puzzle::from_stream(puzzle_file);
	Puzzle soln = Puzzle::from_stream(soln_file);

	if (puzzle.solved_by(soln))
	{
		std::cout << "PASS: " << soln_path << " is a valid solution to " << puzzle_path << "." << std::endl;
	}
	else
	{
		std::cout << "FAIL: " << soln_path << " is NOT a valid solution to " << puzzle_path << "." << std::endl;
		throw std::runtime_error("Solution check failed.");
	}
}

/// @brief Program entry point.
int main(int argc, char **argv)
{

	if (argc < 2)
	{
		print_usage(argv[0]);
		return 1;
	}

	const std::string subcommand = argv[1];

	if (subcommand == "suite")
	{
		if (argc > 4)
		{
			std::cerr << "Error: 'suite' takes at most two optional arguments: [output_csv_path] [timeout_seconds]\n";
			print_usage(argv[0]);
			return 1;
		}
		const std::string csv_path = (argc >= 3) ? argv[2] : "benchmarks.csv";
		int timeout_seconds = 5;
		if (argc == 4)
		{
			try
			{
				timeout_seconds = std::stoi(argv[3]);
				if (timeout_seconds <= 0)
					throw std::invalid_argument("must be positive");
			}
			catch (const std::exception &)
			{
				std::cerr << "Error: timeout_seconds must be a positive integer, got \"" << argv[3] << "\"\n";
				return 1;
			}
		}
		suite_mode(csv_path, timeout_seconds);
		return 0;
	}

	if (argc < 3)
	{
		print_usage(argv[0]);
		return 1;
	}

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
	else if (subcommand == "check")
	{
		if (argc != 4)
		{
			std::cerr << "Error: 'check' requires exactly two arguments: <puzzle_path> <soln_path>\n";
			print_usage(argv[0]);
			return 1;
		}

		const std::string puzzle_path = argv[2];
		const std::string soln_path = argv[3];
		check_mode(puzzle_path, soln_path);
	}
	else
	{
		std::cerr << "Error: unknown subcommand \"" << subcommand << "\"\n";
		print_usage(argv[0]);
		return 1;
	}

	return 0;
}