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
template <typename Genotype, size_t M, size_t N, typename Value, size_t S>
class GeneticAlgorithmFactory
{
protected:

    /**
     * @brief возвращает функцию селекции
     * @tparam N - размер популяции
     * @tparam Value - тип результата вычисления функции от генотипа
     * @tparam S - процент выживших в одной популяции
     * @param parameter - параметр создания функции селекции
     * @return - функция селекции
     */
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
     * @tparam Value - тип результата вычисления функции от генотипа
     * @tparam N - размер популяции
     * @param parameter - параметр создания стратегии выбора родителя
     * @return стратегии выбора родителя
     */
    static IParentChoiserPtr<Genotype, Value, N> GetChooser(const GeneticAlgorithmParameterPtr& parameter)
    {
        if (parameter->chooser_ == "simple_chooser")
        {
            return std::make_shared<SimpleParentChooser<Genotype, Value, N>>(
                    SimpleParentChooser<Genotype, Value, N>(Genotype::GetLength));
        }

        throw std::runtime_error("Can't create parent chooser, incorrect parameter");
    }

public:

    /**
    * @brief возвращает стратегию для алгоритма
    * @tparam Genotype - генотип
    * @tparam Value - тип результата вычисления функции от генотипа
    * @tparam N - размер популяции
    * @param parameter - параметр создания стратегии для алгоритма
    * @return стратегия для алгоритма
    */
    virtual IGeneticAlgorithmStrategyPtr<Genotype, Value, N> GetStrategy(const GeneticAlgorithmParameterPtr& parameter) const = 0;

    /**
     * @brief возвращает объект решения оптимизационной задачи
     * @tparam Genotype - генотип минимизируемой функции
     * @tparam M - процент мутируемых особей
     * @tparam N - размер популяции
     * @tparam Value - тип результата вычисления функции от генотипа
     * @tparam S - процент выживших генов в популяции
     * @param parameter - параметр задачи
     * @return объект решения оптимизационной задачи
     */
    virtual GeneticAlgorithm<Genotype, M, N, Value, S> GetGeneticAlgorithm(const GeneticAlgorithmParameterPtr& parameter) const = 0;
};

template <typename Genotype, size_t M, size_t N, typename Value, size_t S>
using GeneticAlgotihmFactoryPtr = std::shared_ptr<GeneticAlgorithmFactory<Genotype, M, N, Value, S>>;

template <size_t M, size_t N, size_t S>
class GeneticAlgorithmFactoryPointToDouble : public GeneticAlgorithmFactory<Point, M, N, double, S>
{
public:
    IGeneticAlgorithmStrategyPtr<Point, double, N> GetStrategy(const GeneticAlgorithmParameterPtr& parameter) const override
    {
        if (parameter->strategy_ == "simple_strategy")
        {
            return std::make_shared<SimpleGeneticAlgorithmStrategy<Point, double , N>>(
                    SimpleGeneticAlgorithmStrategy<Point, double , N>());
        }

        throw std::runtime_error("Can't create strategy, incorrect parameter");
    }

    GeneticAlgorithm<Point, M, N, double, S> GetGeneticAlgorithm(const GeneticAlgorithmParameterPtr& parameter) const override
    {
        if (!parameter)
        {
            throw std::runtime_error("Empty factory parameter");
        }

        const ISelectionFunctionPtr<double, S, N>& selector = GeneticAlgorithmFactory<Point, M, N, double, S>::GetSelector(parameter);
        const IParentChoiserPtr<Point, double, N>& chooser = GeneticAlgorithmFactory<Point, M, N, double, S>::GetChooser(parameter);
        const IGeneticAlgorithmStrategyPtr<Point, double, N>& strategy = GetStrategy(parameter);
        return GeneticAlgorithm<Point, M, N, double, S>(selector, chooser, strategy);
    }
};

} // GeneticAlgorithm

#endif //LAB_GENETICALGOTIHMFACTORY_H
