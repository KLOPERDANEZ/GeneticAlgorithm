#pragma once

#include "stable.h"

namespace GA
{

template <typename Genotype, typename Value>
class IGeneticAlgorithmStrategy
{
    using Population = std::vector<Genotype>;
    using ScorePopulation = std::vector<Value>;

public:

    virtual Population CreateStartPopulation() const = 0;

    virtual Genotype Mutation(
    		const Genotype& genotype,
    		const size_t iteration_count) const = 0;

    virtual Genotype Crossingover(
    		const Genotype& first_parent, const Value first_score,
    		const Genotype& second_parent, const Value second_score) const = 0;

    virtual Value FitnessFunction(const Genotype &genotype) const = 0;

    virtual bool IsCorrectResult(
    		const Population& population,
    		const ScorePopulation& score_population) const = 0;

    virtual ~IGeneticAlgorithmStrategy() {};
};

template <typename Genotype, typename Value = double>
using IGeneticAlgorithmStrategyPtr = std::shared_ptr<IGeneticAlgorithmStrategy<Genotype, Value>>;

} // GeneticAlgorithm

