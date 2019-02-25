#pragma once

#include <functional>
#include <variant>
#include <chrono>
#include "stable.h"

#include "ISelectionFunction.h"
#include "IGeneticAlgorithmStrategy.h"
#include "Utils.h"

namespace GA
{

template <typename Genotype>
using Population = std::vector<Genotype>;

template <typename ScoreValue>
using ScorePopulation = std::vector<ScoreValue>;

template <typename Genotype, typename ScoreValue>
struct State
{
	Population<Genotype> current_population_;
	ScorePopulation<ScoreValue> current_population_score_;

	State& operator=(const State<Genotype, ScoreValue>& state)
	{
		if (&state == this)
		{
			return *this;
		}

		current_population_score_ = state.current_population_score_;
		current_population_ = state.current_population_;

		return *this;
	}
};

template <typename Genotype, typename ScoreValue>
using States = std::vector<State<Genotype, ScoreValue>>;

template <typename Genotype, typename ScoreValue>
struct GeneticAlgorithmResult
{
	State<Genotype, ScoreValue> final_state_;
	size_t iteration_count_ = 0;
	std::optional<States<Genotype, ScoreValue>> states_ = std::nullopt;
	std::optional<long> times_ = std::nullopt;
};

template <typename Genotype, typename ScoreValue>
class GeneticAlgorithm
{
    using Index = size_t;
    using Indexes = std::vector<Index>;
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

    template <bool IsSaveState = false, bool IsMeasuringTime = false>
    GeneticAlgorithmResult<Genotype, ScoreValue> Calculation(
            const double mutation_part,
            const double crossingover_part,
            const size_t limit = 1000) const
    {
        assert(mutation_part > 0.0 && mutation_part < 1.0);
        assert(crossingover_part > 0.0 && mutation_part < 1.0);

		long start_time = 0;
        if constexpr (IsMeasuringTime)
		{
			const long current_time_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			start_time = current_time_nanoseconds;
		}

		GeneticAlgorithmResult<Genotype, ScoreValue> result;

		result.final_state_.current_population_ = strategy_->CreateStartPopulation();
		result.final_state_.current_population_score_.resize(result.final_state_.current_population_.size());

		if constexpr (IsSaveState)
		{
			result.states_ = std::make_optional<States<Genotype, ScoreValue>>();
		}

		ApplyFitnessFuntionToPopulation(result.final_state_);

		bool is_not_result_correct;
	 	bool is_not_iter_limit;

		do
        {
			if constexpr (IsSaveState)
			{
				result.states_->push_back(result.final_state_);
			}

            ApplyCrossingoverToPopulation(crossingover_part, result.final_state_);
            ApplyMutationToPopulation(mutation_part, result.iteration_count_, result.final_state_);
			ApplyFitnessFuntionToPopulation(result.final_state_);
			result.iteration_count_++;

            is_not_result_correct = !strategy_->
            		IsCorrectResult(result.final_state_.current_population_, result.final_state_.current_population_score_);
            is_not_iter_limit = static_cast<const bool>(result.iteration_count_ < limit);
        }
        while (is_not_result_correct && is_not_iter_limit);

		if constexpr (IsMeasuringTime)
		{
			const long current_time_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			result.times_ = current_time_nanoseconds - start_time;
		}

        return result;
    }

protected:

    void ApplyFitnessFuntionToPopulation(State<Genotype, ScoreValue>& state) const
    {
        std::transform(
        		state.current_population_.begin(),
				state.current_population_.end(),
				state.current_population_score_.begin(),
                [this](const auto& genotype)
                {
                    return strategy_->FitnessFunction(genotype);
                });
    }

    void ApplyMutationToPopulation(
    		const double mutation_part,
    		const size_t iteration_count,
			State<Genotype, ScoreValue>& state) const
    {
        const size_t mutation_count = static_cast<size_t>(mutation_part * state.current_population_.size());

        std::vector<typename Population<Genotype>::iterator> sampler(mutation_count);
        sample_iterator(state.current_population_.begin(), state.current_population_.end(),
                sampler.begin(), mutation_count, std::mt19937{std::random_device{}()});

        for (auto& sample : sampler)
        {
            *sample = strategy_->Mutation(*sample, iteration_count);
        }
    }

    void ApplyCrossingoverToPopulation(
    		const double crossingover_part,
			State<Genotype, ScoreValue>& state) const
    {
        const size_t crossingover_count =
        		static_cast<const size_t>(state.current_population_.size() * crossingover_part);
		const size_t not_crossingover_count = state.current_population_.size() - crossingover_count;

		const GreatDoubleIndexMap& distribution_population = GetSurviveDistributionPopulation(state);
		const Indexes& survives = GetSurviveIndices(crossingover_count, not_crossingover_count, distribution_population);

		Crossingover(crossingover_count, not_crossingover_count, distribution_population, survives, state);
	}

private:

	void Crossingover(
			const size_t crossingover_count,
			const size_t not_crossingover_count,
			const GreatDoubleIndexMap& distribution_population,
			const Indexes& survives,
			State<Genotype, ScoreValue>& state) const
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

			const Genotype& first_parent = state.current_population_[survives[first_parent_index]];
			const Genotype& second_parent = state.current_population_[survives[second_parent_index]];

			const double first_score = state.current_population_score_[survives[first_parent_index]];
			const double second_score = state.current_population_score_[survives[second_parent_index]];

			state.current_population_[index] =
					strategy_->Crossingover(first_parent, first_score, second_parent, second_score);
		}
		while (count++ != not_crossingover_count);
	}

	Indexes GetSurviveIndices(
			const size_t crossingover_count,
			const size_t not_crossingover_count,
			const GreatDoubleIndexMap& distribution_population) const
	{
		Indexes survives(crossingover_count);
		auto start_survive_genotype_iterator = distribution_population.begin();
		std::advance(start_survive_genotype_iterator, not_crossingover_count);
		std::transform(start_survive_genotype_iterator, distribution_population.end(), survives.begin(),
				std::mem_fn(&GreatDoubleIndexMap::value_type::second));

		return survives;
	}

	GreatDoubleIndexMap GetSurviveDistributionPopulation(const State<Genotype, ScoreValue>& state) const
	{
		const auto& survive_chance = selector_->Selection(state.current_population_score_);
		GreatDoubleIndexMap distribution_population;
		{
			for (size_t index = 0; index < state.current_population_.size(); ++index)
			{
				distribution_population.emplace(survive_chance[index], index);
			}
		}
		return distribution_population;
	}

    ISelectionFunctionPtr<ScoreValue> selector_;
    IGeneticAlgorithmStrategyPtr<Genotype, ScoreValue> strategy_;
};

template <typename Genotype, typename ScoreValue>
using GeneticAlgorithmPtr = std::shared_ptr<GeneticAlgorithm<Genotype, ScoreValue>>;

} // GeneticAlgorithm
