#pragma once

#include "../GeneticAlgorithm/GeneticAlgorithm.h"
#include "Point2d.h"

struct GeneticAlgorithmSolverFactory
{
	static GA::GeneticAlgorithmPtr<Point2d, double> CreateGeneticAlgorithmPoint2dSolver(
			const std::string& function_name,
			const std::string& selection_function_type_name,
			const size_t genotype_size);
};
