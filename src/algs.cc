/*!
 * @file algs.cc
 * @brief Implements algorithms declared in `algs.hh`.
 *
 * Copyright Finley Owen, 2026. All rights reserved.
 */

#include "algs.hh"

std::optional<Puzzle> dfs(Puzzle &u, size_t &nodes_explored)
{
	// increment nodes_explored
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

std::optional<Puzzle> dfs_tracked(Puzzle &u,
								  std::unordered_set<Puzzle> seen,
								  size_t &nodes_explored)
{
	// increment nodes_explored
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

std::optional<Puzzle> bfs(Puzzle &u, size_t &nodes_explored)
{
	// create the queue and add the source node
	std::list<Puzzle> q;
	q.push_back(u);

	// loop while the queue has elements
	while (!q.empty())
	{
		// increment nodes_explored
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
