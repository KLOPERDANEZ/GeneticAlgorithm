#ifndef LAB_IGENETICALGORITHMSTRATEGY_H
#define LAB_IGENETICALGORITHMSTRATEGY_H

#include <memory>

namespace GeneticAlgorithm
{

/**
 * @brief Описание функции для которой вызывается генетический алгоритм
 * @tparam Genotype - генотип
 * @tparam Value - тип результата вычисления функции от гетотипа
 * @tparam N - размер популяции
 */
template <typename Genotype, typename Value, size_t N = 1000>
class IGeneticAlgorithmStrategy
{
    using Population = std::array<Genotype, N>;
    using ScorePopulation = std::array<Value, N>;
    using Survivors = std::array<bool, N>;

public:

    /**
     * @brief Создает стартовую популяцию
     * @return возвращает массив стартовых геномов
     */
    virtual Population CreateStartPopulation() const = 0;

    /**
     * @brief Возвращает объект подвергшийся мутации
     * @param genotype[in] - объект который нужно мутировать
     * @return мутировавший объект
     */
    virtual Genotype Mutation(const Genotype& genotype) const = 0;

    /**
     * @brief Применяет операцию кроссинговер для двух родителей
     * @param first_parent[in] - родитель
     * @param second_parent[in] - родитель
     * @return объект потомок, содержащий признак обоих родителей
     */
    virtual Genotype Crossingover(const Genotype& first_parent, const Genotype& second_parent) const = 0;

    /**
     * @brief Возвращает значение функции для генотипа
     * @param genotype[in] - объект для которого вычисляется функция
     * @return значение функции
     */
    virtual Value FitnessFunction(const Genotype &genotype) const = 0;

    /**
     * @brief Говорит, нужно ли закончить поиск
     * @param population[in] - популяция
     * @param score_population[in] - значения функции для популяции
     * @return нужно ли закончить поиск
     */
    virtual bool IsCorrectResult(const Population& population, const ScorePopulation& score_population) const = 0;
};

template <typename Genotype, typename Value = double, size_t N = 1000>
using IGeneticAlgorithmStrategyPtr = std::shared_ptr<IGeneticAlgorithmStrategy<Genotype, Value, N>>;

} // GeneticAlgorithm

#endif //LAB_IGENETICALGORITHMSTRATEGY_H
