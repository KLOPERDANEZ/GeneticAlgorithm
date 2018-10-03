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

    const auto& factory = std::make_shared<GeneticAlgorithm::GeneticAlgorithmFactoryPointToDouble<20, 100, 10>>();
    const auto& result = factory->GetGeneticAlgorithm(parameter).Calculation(100);
    const auto& strategy = factory->GetStrategy(parameter);
    const auto& min_point_value_iter = std::min_element(result.second.begin(), result.second.end());
    const auto& min_point_index = std::distance(result.second.begin(), min_point_value_iter);
    std::cout << *min_point_value_iter << " : " << result.first[min_point_index].to_string() << std::endl;
    return 0;
}
