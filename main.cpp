#include <iostream>

#include "GeneticAlgorithm/GeneticAlgorithm.h"
#include "TestGeneticAlgorithm.h"

int main()
{
    auto selection_function =
            std::make_shared<SimpleSelectionFunction<double>>(
                    SimpleSelectionFunction<double>());
    auto parent_chooser =
            std::make_shared<SimpleParentChooser<Point, double>>(
                    SimpleParentChooser<Point, double>(Point::GetLength));
    auto genetic_algorithm_strategy =
            std::make_shared<SimpleGeneticAlgorithmStrategy<Point, double>>(
                    SimpleGeneticAlgorithmStrategy<Point, double>());
    GeneticAlgorithm<Point, 10, 1000, double, 20> ga(selection_function, parent_chooser, genetic_algorithm_strategy);
    const std::array<Point, 1000> &result = ga.Calculation(1000);
    std::cout << std::min_element(result.begin(), result.end()) << std::endl;
    return 0;
}
