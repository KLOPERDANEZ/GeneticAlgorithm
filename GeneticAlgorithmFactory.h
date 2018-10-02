#ifndef LAB_GENETICALGOTIHMFACTORY_H
#define LAB_GENETICALGOTIHMFACTORY_H

#include <memory>

#include "GeneticAlgorithm/GeneticAlgorithm.h"
#include "TestGeneticAlgorithm.h"

namespace GeneticAlgorithm
{

/**
 * @brief Параметры создания объекта решения генетического алгоритма
 */
struct GeneticAlgorithmParameter
{
    std::string chooser_;
    std::string strategy_;
    std::string selector_;
};

using GeneticAlgorithmParameterPtr = std::shared_ptr<GeneticAlgorithmParameter>;

/**
 * @brief Фабрика для создания объекта решения генетического алгоритма и его параметров
 */
class GeneticAlgorithmFactory
{
public:

    /**
     * @brief возвращает функцию селекции
     * @tparam N - размер популяции
     * @tparam Value - тип результата вычисления функции от гетотипа
     * @tparam S - процент выживших в одной популяции
     * @param parameter - параметр создания функции селекции
     * @return - функция селекции
     */
    template <size_t N, typename Value, size_t S>
    static ISelectionFunctionPtr<Value, S, N> GetSelector(const GeneticAlgorithmParameterPtr& parameter)
    {
        if (parameter->selector_ == "simple_selector")
        {
            return std::make_shared<SimpleSelectionFunction<Value, S, N>>(
                    SimpleSelectionFunction<Value, S, N>());
        }

        throw std::runtime_error("Can't create selector, incorrect parameter");
    }

    /**
     * @brief возвращает стратегию выбора родителя
     * @tparam Genotype - генотип
     * @tparam Value - тип результата вычисления функции от гетотипа
     * @tparam N - размер популяции
     * @param parameter - параметр создания стратегии выбора родителя
     * @return стратегии выбора родителя
     */
    template <typename Genotype, typename Value, size_t N>
    static IParentChoiserPtr<Genotype, Value, N> GetChooser(const GeneticAlgorithmParameterPtr& parameter)
    {
        if (parameter->chooser_ == "simple_chooser")
        {
            return std::make_shared<SimpleParentChooser<Genotype, Value, N>>(
                    SimpleParentChooser<Genotype, Value, N>(Point::GetLength));
        }

        throw std::runtime_error("Can't create parent chooser, incorrect parameter");
    }

    /**
     *
     * @tparam Genotype
     * @tparam Value
     * @tparam N
     * @param parameter
     * @return
     */
    template <typename Genotype, typename Value, size_t N>
    static IGeneticAlgorithmStrategyPtr<Genotype, Value, N> GetStrategy(const GeneticAlgorithmParameterPtr& parameter)
    {
        if (parameter->strategy_ == "simple_strategy")
        {
            return std::make_shared<SimpleGeneticAlgorithmStrategy<Genotype, Value, N>>(
                    SimpleGeneticAlgorithmStrategy<Genotype, Value, N>());
        }

        throw std::runtime_error("Can't create strategy, incorrect parameter");
    }

    /**
     * @brief возвращает объект решения оптимизационной задачи
     * @tparam Genotype - генотип минимизируемой функции
     * @tparam M - процент мутируемых особей
     * @tparam N - размер популяции
     * @tparam Value - тип результата вычисления функции от гетотипа
     * @tparam S - процент выживших генов в популяции
     * @param parameter - параметр задачи
     * @return объект решения оптимизационной задачи
     */
    template <typename Genotype, size_t M, size_t N, typename Value, size_t S>
    static GeneticAlgorithm<Genotype, M, N, Value, S> GetGeneticAlgorithm(
            const GeneticAlgorithmParameterPtr& parameter)
    {
        if (!parameter)
        {
            throw std::runtime_error("Empty factory parameter");
        }

        return GeneticAlgorithm(
                GetSelector<N, Value, S>(parameter),
                GetChooser<Genotype, Value, N>(parameter),
                GetStrategy<Genotype, Value, N>(parameter));
    }
};

using GeneticAlgotihmFactoryPtr = std::shared_ptr<GeneticAlgorithmFactory>;

} // GeneticAlgorithm

#endif //LAB_GENETICALGOTIHMFACTORY_H
