#include <iostream>

#include "GeneticAlgorithmFactory.h"
#include "TestGeneticAlgorithm.h"

int main()
{
    const auto& parameter = std::make_shared<GeneticAlgorithm::GeneticAlgorithmParameter>();
    {
        parameter->strategy_ = "simple_strategy";
        parameter->chooser_ = "simple_chooser";
        parameter->selector_ = "simple_selector";
    }

    const auto& factory = std::make_shared<GeneticAlgorithm::GeneticAlgorithmFactory>();
    const std::array<Point, 100> &result =
            factory->GetGeneticAlgorithm<Point, 10, 100, double, 20>(parameter).Calculation(100);

    const auto& strategy = factory->GetStrategy<Point, double, 100>(parameter);

    const auto& result_point = std::min_element(result.begin(), result.end(),
            [strategy](const auto& one, const auto& two)
            {
                return strategy->FitnessFunction(one) < strategy->FitnessFunction(two);
            });
    std::cout << result_point->to_string() << std::endl;
    return 0;
}
