#ifndef LAB_TESTGENETICALGORITHM_H
#define LAB_TESTGENETICALGORITHM_H

#include <functional>
#include <limits>

#include "GeneticAlgorithm/IParentChoiser.h"
#include "GeneticAlgorithm/ISelectionFunction.h"
#include "GeneticAlgorithm/IGeneticAlgorithmStrategy.h"
#include "GeneticAlgorithm/Utils.h"

class Point
{
    double x_, y_, z_;

public:

    Point() : x_(0.0), y_(0.0), z_(0.0)
    {}

    explicit Point(double x, double y, double z) : x_(x), y_(y), z_(z)
    {}

    Point operator+(const Point& other) const
    {
        return Point(x_ + other.x_, y_ + other.y_, z_ + other.z_);
    }

    static Point GetRandomPoint(Point low, Point high)
    {
        static std::random_device rd;
        static std::mt19937 gen;
        double x = std::uniform_real_distribution<double>(low.x_, high.x_)(gen);
        double y = std::uniform_real_distribution<double>(low.y_, high.y_)(gen);
        double z = std::uniform_real_distribution<double>(low.z_, high.z_)(gen);
        return Point(x, y, z);
    }

    static double GetLength(const Point& first, const Point& second)
    {
        const double xx = (first.x_ - second.x_) * (first.x_ - second.x_);
        const double yy = (first.y_ - second.y_) * (first.y_ - second.y_);
        const double zz = (first.z_ - second.z_) * (first.z_ - second.z_);

        return std::sqrt(xx + yy + zz);
    }

    std::string to_string() const
    {
        return std::to_string(x_) + ";" + std::to_string(y_) + ";" + std::to_string(z_);
    }

    double GetX() const 
    {
        return x_;
    }

    double GetY() const 
    {
        return y_;
    }

    double GetZ() const 
    {
        return z_;
    }
};

template <typename Genotype, typename Value, size_t N = 1000>
class SimpleParentChooser : public GeneticAlgorithm::IParentChoiser<Genotype, Value, N>
{
    using Population = std::array<Genotype, N>;
public:

    explicit SimpleParentChooser(const std::function<double(const Genotype&, const Genotype&)>& length)
        : length_(length)
    {}

    size_t ChoiseParent(const Genotype& parent, const Population& population) const override
    {
        std::array<double, N> length_score;
        for (size_t index = 0; index < N; ++index)
        {
            length_score[index] = length_(parent, population[index]);
        }
        const double zero_length = 0.0;
        const auto& parent_iter = std::find(length_score.begin(), length_score.end(), zero_length);
        *parent_iter = std::numeric_limits<double>::min();
        return static_cast<size_t>(std::distance(
                std::begin(length_score),
                std::max_element(length_score.begin(), length_score.end())));
    }

private:
    const std::function<double(const Genotype&, const Genotype&)>& length_;
};

template <typename Value = double, size_t S = 20, size_t N = 1000>
class SimpleSelectionFunction : public GeneticAlgorithm::ISelectionFunction<Value, S, N>
{
    using ScorePopulation = std::array<Value, N>;
public:
    std::array<double, N> Selection(const ScorePopulation &score_population) const override
    {
        return score_population;
    }
};

template <typename Genotype = Point, typename Value = double, size_t N = 1000>
class SimpleGeneticAlgorithmStrategy : public GeneticAlgorithm::IGeneticAlgorithmStrategy<Genotype, Value, N>
{
    using Population = std::array<Genotype, N>;
    using ScorePopulation = std::array<Value, N>;
    using Survivors = std::array<bool, N>;

public:
    Population CreateStartPopulation() const override
    {
        Population start_population;
        const Point low(-100.0, -100.0, -100.0);
        const Point high(100.0, 100.0, 100.0);

        for (auto& val : start_population)
        {
            val = Point::GetRandomPoint(low, high);
        }
        return start_population;
    }

    Genotype Mutation(const Genotype &genotype) const override 
    {
        static const double radius_x2 = 10.0;
        const int shift_coeff = 100000;
        const double shift_coeffd = shift_coeff + 0.0;
        const double shift_x = radius_x2 / shift_coeffd * randomizer_.GetNumber(0, shift_coeff) - radius_x2 / 2;
        const double shift_y = radius_x2 / shift_coeffd * randomizer_.GetNumber(0, shift_coeff) - radius_x2 / 2;
        const double shift_z = radius_x2 / shift_coeffd * randomizer_.GetNumber(0, shift_coeff) - radius_x2 / 2;
        return genotype + Point(shift_x, shift_y, shift_z);
    }

    Genotype Crossingover(const Genotype &first_parent, const Genotype &second_parent) const override 
    {
        Value first_val = FitnessFunction(first_parent);
        Value second_val = FitnessFunction(second_parent);
        return first_val > second_val ? first_parent : second_parent;
    }

    Value FitnessFunction(const Genotype &genotype) const override 
    {
        return std::sin(genotype.GetX()) + std::sin(genotype.GetY()) + std::sin(genotype.GetZ());
    }

    bool IsCorrectResult(const Population &population, const ScorePopulation &score_population) const override 
    {
        Value min_value = *std::min_element(score_population.begin(), score_population.end());
        if ((last_result_ - min_value) < 1E-6)
        {
            return true;
        }
        last_result_ = min_value;
        return false;
    }

private:
    mutable Value last_result_ = 0.0;
    const Randomizer& randomizer_ = Randomizer::GetInstance();
};

#endif //LAB_TESTGENETICALGORITHM_H
