#include "GeneticAlgorithmSolverFactory.h"

#include "ForwardSelectionFunction.h"
#include "RosenbrokFunctionStrategy.h"

GA::GeneticAlgorithmPtr<Point2d, double>
GeneticAlgorithmSolverFactory::CreateGeneticAlgorithmPoint2dSolver(
		const std::string& function_name,
		const std::string& selection_function_type_name,
		const size_t genotype_size)
{
	GA::ISelectionFunctionPtr<double> selection_function;
	if (selection_function_type_name == "simple-forward")
	{
		selection_function = std::make_shared<ForwardSelectionFunction<double>>();
	}

	GA::IGeneticAlgorithmStrategyPtr<Point2d, double> strategy;
	if (function_name == "rosenbrok")
	{
		strategy = std::make_shared<RosenbrokFunctionStrategy>(genotype_size);
	}

	if (!selection_function)
	{
		throw std::runtime_error("Can't create selection function, incorrect parameter: " + selection_function_type_name);
	}

	if (!strategy)
	{
		throw std::runtime_error("Can't create strategy, incorrect parameter: " + function_name);
	}

	return std::make_shared<GA::GeneticAlgorithm<Point2d, double>>(selection_function, strategy);
}
