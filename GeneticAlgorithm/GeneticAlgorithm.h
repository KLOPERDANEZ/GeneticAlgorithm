#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <map>
#include "ISelectionFunction.h"
#include "IGeneticAlgorithmStrategy.h"
#include "Utils.h"

namespace GA
{

template <typename Genotype, typename ScoreValue>
class GeneticAlgorithm
{
    using Population = std::vector<Genotype>;
    using ScorePopulation = std::vector<ScoreValue>;
    using Index = size_t;
    using GreatDoubleIndexMap = std::multimap<double, Index, std::greater<>>;

public:

    explicit GeneticAlgorithm(
            const ISelectionFunctionPtr<ScoreValue>& selector,
            const IGeneticAlgorithmStrategyPtr<Genotype, ScoreValue>& strategy)
            : selector_(selector)
            , strategy_(strategy)
    {
        if (!selector)
        {
            throw std::runtime_error("Empty selector");
        }
        if (!strategy)
        {
            throw std::runtime_error("Empty strategy");
        }
    }

    std::pair<Population, ScorePopulation> Calculation(
            const double mutation_part,
            const double crossingover_part,
            const size_t limit = 1000) const
    {
        assert(mutation_part > 0.0 && mutation_part < 1.0);
        assert(crossingover_part > 0.0 && mutation_part < 1.0);

        size_t iteration_number = 0;
        current_population_ = strategy_->CreateStartPopulation();
        current_population_score_.resize(current_population_.size());
        ApplyFitnessFuntionToPopulation();

		bool is_not_result_correct;
	 	bool is_not_iter_limit;

		do
        {
            ApplyCrossingoverToPopulation(crossingover_part);
            ApplyMutationToPopulation(mutation_part, iteration_number);
			ApplyFitnessFuntionToPopulation();
            iteration_number++;

            is_not_result_correct = !strategy_->IsCorrectResult(current_population_, current_population_score_);
            is_not_iter_limit = static_cast<const bool>(iteration_number < limit);
        }
        while (is_not_result_correct && is_not_iter_limit);

        return { current_population_, current_population_score_ };
    }

protected:

    void ApplyFitnessFuntionToPopulation() const
    {
        std::transform(current_population_.begin(), current_population_.end(), current_population_score_.begin(),
                [this](const auto& genotype)
                {
                    return strategy_->FitnessFunction(genotype);
                });
    }

    void ApplyMutationToPopulation(const double mutation_part, const size_t iteration_count) const
    {
        const size_t mutation_count = static_cast<size_t>(mutation_part * current_population_.size());

        std::vector<typename Population::iterator> sampler(mutation_count);
        sample_iterator(current_population_.begin(), current_population_.end(),
                sampler.begin(), mutation_count, std::mt19937{std::random_device{}()});

        for (auto& sample : sampler)
        {
            *sample = strategy_->Mutation(*sample, iteration_count);
        }
    }

    void ApplyCrossingoverToPopulation(const double crossingover_part) const
    {
        const size_t crossingover_count = static_cast<const size_t>(current_population_.size() * crossingover_part);
		const size_t not_crossingover_count = current_population_.size() - crossingover_count;

		const GreatDoubleIndexMap& distribution_population = GetSurviveDistributionPopulation();
		const std::vector<Index>& survives = GetSurviveIndices(crossingover_count, not_crossingover_count, distribution_population);

		Crossingover(crossingover_count, not_crossingover_count, distribution_population, survives);
	}

private:

	void Crossingover(
			const size_t crossingover_count,
			const size_t not_crossingover_count,
			const GreatDoubleIndexMap& distribution_population,
			const std::vector<Index>& survives) const
	{
		std::mt19937 generator(std::random_device{}());
		std::uniform_int_distribution<Index> distribution(0, crossingover_count - 1);

		size_t count = 0;
		auto distribution_population_iter = distribution_population.begin();
		do
		{
			const auto& [score, index] = *distribution_population_iter++;

			Index first_parent_index = distribution(generator);
			Index second_parent_index = distribution(generator);

			const Genotype& first_parent = current_population_[survives[first_parent_index]];
			const Genotype& second_parent = current_population_[survives[second_parent_index]];

			const double first_score = current_population_score_[survives[first_parent_index]];
			const double second_score = current_population_score_[survives[second_parent_index]];

			current_population_[index] = strategy_->Crossingover(first_parent, first_score, second_parent, second_score);
		} while (count++ != not_crossingover_count);
	}

	std::vector<Index> GetSurviveIndices(
			const size_t crossingover_count,
			const size_t not_crossingover_count,
			const GreatDoubleIndexMap& distribution_population) const
	{
		std::vector<Index> survives(crossingover_count);
		auto start_survive_genotype_iterator = distribution_population.begin();
		std::advance(start_survive_genotype_iterator, not_crossingover_count);
		std::transform(
				start_survive_genotype_iterator,
				distribution_population.end(),
				survives.begin(),
				[](const auto& score_genotype)
				{
					return score_genotype.second;
				});
		return survives;
	}

	GreatDoubleIndexMap GetSurviveDistributionPopulation() const
	{
		const auto& survive_chance = selector_->Selection(current_population_score_);
		GreatDoubleIndexMap distribution_population;
		{
			for (size_t index = 0; index < current_population_.size(); ++index)
			{
				distribution_population.emplace(survive_chance[index], index);
			}
		}
		return distribution_population;
	}

    ISelectionFunctionPtr<ScoreValue> selector_;
    IGeneticAlgorithmStrategyPtr<Genotype, ScoreValue> strategy_;

    mutable Population current_population_;
    mutable ScorePopulation current_population_score_;
};

template <typename Genotype, typename ScoreValue>
using GeneticAlgorithmPtr = std::shared_ptr<GeneticAlgorithm<Genotype, ScoreValue>>;

} // GeneticAlgorithm
