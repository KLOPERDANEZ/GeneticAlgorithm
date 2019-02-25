#pragma once

#include <vector>
#include <random>

#include "../GeneticAlgorithm/IGeneticAlgorithmStrategy.h"
#include "Point2d.h"

class RosenbrokFunctionStrategy : public GA::IGeneticAlgorithmStrategy<Point2d, double>
{
	using Population = std::vector<Point2d>;
	using ScorePopulation = std::vector<double >;

public:

	explicit RosenbrokFunctionStrategy(const size_t genotype_size);

	Population CreateStartPopulation() const override;

	Point2d Mutation(const Point2d& genotype, const size_t iteration_count) const override;

	Point2d Crossingover(
			const Point2d& first_parent, const double first_score,
			const Point2d& second_parent, const double second_score) const override;

	double FitnessFunction(const Point2d& genotype) const override;

	bool IsCorrectResult(const Population& population, const ScorePopulation& score_population) const override;

	~RosenbrokFunctionStrategy() override = default;

private:
	size_t genotype_size_;

	const double min_border_ = -3.0;
	const double max_border_ = 3.0;

	constexpr static double precise_ = 1E-6;

	mutable double last_mean_element_;
};
