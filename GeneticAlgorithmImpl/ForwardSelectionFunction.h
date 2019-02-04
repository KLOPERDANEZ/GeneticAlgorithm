#pragma once

#include <vector>

#include "../GeneticAlgorithm/ISelectionFunction.h"

template <typename Value>
class ForwardSelectionFunction : public GA::ISelectionFunction<Value>
{
	using ScorePopulation = std::vector<Value>;

public:

	ForwardSelectionFunction() = default;

	std::vector<double> Selection(const ScorePopulation& score_population) const override
	{
		return score_population;
	}
};
