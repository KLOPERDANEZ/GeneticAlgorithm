#include "RosenbrokFunctionStrategy.h"

#include <algorithm>

RosenbrokFunctionStrategy::RosenbrokFunctionStrategy(const size_t genotype_size)
		: genotype_size_(genotype_size)
{

}

std::vector<Point2d> RosenbrokFunctionStrategy::CreateStartPopulation() const
{
	std::mt19937 gen{std::random_device{}()};
	std::uniform_real_distribution<> dis(min_border_, max_border_);

	std::vector<Point2d> points(genotype_size_);
	std::generate(points.begin(), points.end(),
			[&dis, &gen]()
			{
				return Point2d(dis(gen), dis(gen));
			});

	std::vector<double> score_points(points.size());
	std::transform(points.begin(), points.end(), score_points.begin(),
			[this](const auto& point)
			{
				return this->FitnessFunction(point);
			});

	last_mean_element_ = std::accumulate(score_points.begin(), score_points.end(), 0.0) / score_points.size();

	return points;
}

Point2d RosenbrokFunctionStrategy::Mutation(const Point2d& genotype, const size_t iteration_count) const
{
	static std::mt19937 gen{std::random_device{}()};
	std::uniform_real_distribution<> dis(min_border_ / (iteration_count + 1), max_border_ / (iteration_count + 1));

	return Point2d(genotype.x() + dis(gen), genotype.y() + dis(gen));
}

Point2d RosenbrokFunctionStrategy::Crossingover(const Point2d& first_parent, const double first_score,
												const Point2d& second_parent, const double second_score) const
{
	return first_score > second_score ? second_parent : first_parent;
}

double RosenbrokFunctionStrategy::FitnessFunction(const Point2d& genotype) const
{
	const double x = genotype.x();
	const double y = genotype.y();
	const double first_part = (1.0 - x) * (1.0 - x);
	const double sqr_x = x * x;
	const double second_part = 100.0 * (y - sqr_x) * (y - sqr_x);
	return first_part + second_part;
}

bool RosenbrokFunctionStrategy::IsCorrectResult(
		const std::vector<Point2d>& population,
		const std::vector<double>& score_population) const
{
	const double mean_val =
			std::accumulate(score_population.begin(), score_population.end(), 0.0) / score_population.size();
	const double delta = std::abs(last_mean_element_ - mean_val);
	last_mean_element_ = mean_val;
	return delta < precise_;
}
