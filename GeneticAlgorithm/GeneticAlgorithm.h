#ifndef LAB_GENETICALGORITHM_H
#define LAB_GENETICALGORITHM_H

#include <array>
#include <algorithm>
#include "ISelectionFunction.h"
#include "IParentChoiser.h"
#include "IGeneticAlgorithmStrategy.h"
#include "Utils.h"

namespace GeneticAlgorithm
{

/**
 * @brief Класс реализующий решение оптимизационной задачи генетическим алгоритмом
 * Реализован посредством паттерна стратегия
 * https://ru.wikipedia.org/wiki/Генетический_алгоритм
 * @tparam Genotype - генотип минимизируемой функции
 * @tparam M - процент мутируемых особей
 * @tparam N - размер популяции
 * @tparam Value - тип результата вычисления функции от генотипа
 * @tparam S - процент выживших генов в популяции
 */
template <typename Genotype, size_t M = 10, size_t N = 1000, typename Value = double, size_t S = 20>
class GeneticAlgorithm
{
    using Population = std::array<Genotype, N>;
    using ScorePopulation = std::array<Value, N>;
    using Survivors = std::array<double, N>;

public:

    /**
     * @brief Конструктор объекта для рассчета генетического алгоритма
     * @param selector - объект дающий оцеку качества геномов
     * @param parent_choiser - алгоритм выбора родителя
     * @param strategy - описание исследуемой функции
     */
    explicit GeneticAlgorithm(
            const ISelectionFunctionPtr<Value, S, N>& selector,
            const IParentChoiserPtr<Genotype, Value, N>& parent_choiser,
            const IGeneticAlgorithmStrategyPtr<Genotype, Value, N>& strategy)
            : selector_(selector)
            , parent_choiser_(parent_choiser)
            , strategy_(strategy)
    {
        static_assert(M < 100 && S < 100, "Incorrect parameter genetic algorithm");

        if (!selector)
        {
            throw std::runtime_error("Empty selector");
        }
        if (!parent_choiser)
        {
            throw std::runtime_error("Empty parent chooser");
        }
        if (!strategy)
        {
            throw std::runtime_error("Empty strategy");
        }
    }

    /**
     * @brief Метод выполняет расчет оптимизационной задачи
     * @param limit - максимальное количество итераций, по умолчанию 1000
     * @return возвращает лучшую популяцию
     */
    std::pair<Population, ScorePopulation> Calculation(size_t limit = 1000)
    {
        current_population_ = strategy_->CreateStartPopulation();
        CalculationCurrentPopulationFitness();
        while (!strategy_->IsCorrectResult(current_population_, current_population_score_) && limit--)
        {
            const Survivors& survivors = selector_->Selection(current_population_score_);
            ApplyCrossingoverToPopulation(survivors, current_population_);
            ApplyMutationToPopulation(current_population_);
            CalculationCurrentPopulationFitness();
        }
        return { current_population_, current_population_score_ };
    }

protected:

    /**
     * @brief Вычисляет значение функции во всей текущей популяции
     */
    void CalculationCurrentPopulationFitness()
    {
        ApplyFitnessFuntionToPopulation(current_population_, current_population_score_);
    }

    /**
     * @brief Вычисляет значение функции у популяции
     * @param population[in] - популяция для которой вычисляем функцию
     * @param score_population[out] - значение функции
     */
    void ApplyFitnessFuntionToPopulation(const Population& population,
            ScorePopulation& score_population) const
    {
        for (size_t index = 0; index < population.size(); ++index)
        {
            const auto& genotype = population[index];
            score_population[index] = strategy_->FitnessFunction(genotype);
        }
    }

    /**
     * @brief Применение мутаций к mutation_count генотипам в популяции
     * @param population[in, out] - популяция к которой применяем мутации
     */
    void ApplyMutationToPopulation(Population& population) const
    {
        for (size_t counter = 0; counter < mutation_count; ++counter)
        {
            const size_t index = randomizer_.GetNumber(0, population.size());
            const Genotype &genotype = population[index];
            population[index] = strategy_->Mutation(genotype);
        }
    }

    /**
     * @brief Применение кроссинговера ко всей выжившей популяции
     * @param survivors[in] - массив с оценками насколько приспособлен геном
     * @param population[out] - популяция
     */
    void ApplyCrossingoverToPopulation(const Survivors& survivors, Population& population) const
    {
        const auto& survivors_indices = GetSurvivorIndices(survivors);
        for (size_t index = 0; index < survivors.size(); ++index)
        {
            if (std::find(survivors_indices.begin(), survivors_indices.begin() + survivor_count, index) ==
            		survivors_indices.begin() + survivor_count)
            {
                const size_t survivor_index = randomizer_.GetNumber(0, survivor_count);
                const size_t genotype_index = survivors_indices[survivor_index];
                const Genotype& first_parent = population[genotype_index];
                size_t index_second_parent = parent_choiser_->ChoiseParent(first_parent, population);
                if (index_second_parent > N)
                {
                    throw std::runtime_error("Incorrect index second parent");
                }
                const Genotype& second_parent = population[index_second_parent];
                population[index] = strategy_->Crossingover(first_parent, second_parent);
            }
        }
    }

private:

    /**
     * @brief Вычисляет индексы survivor_count выживших особей
     * @param survivors[in] - массив с оценками насколько приспособлен геном
     * @return возвращает индексов массив выживших особей
     */
    std::array<size_t, N> GetSurvivorIndices(const Survivors& survivors) const
    {
        std::array<std::pair<size_t, double>, N> score;
        for (size_t index = 0; index < survivors.size(); ++index)
        {
            score[index] = { index, survivors[index] };
        }

        std::sort(score.begin(), score.end(),
                [](const auto& left, const auto& right) { return left.second < right.second; });

        std::array<size_t, N> survivor_index;
        for (size_t index = 0; index < survivor_count; ++index)
        {
            survivor_index[index] = score[index].first;
        }
        return survivor_index;
    }

    static constexpr size_t mutation_count = static_cast<const size_t>(N / 100.0 * M);
    static constexpr size_t survivor_count = static_cast<const size_t>(N / 100.0 * S);
    const Randomizer& randomizer_ = Randomizer::GetInstance();

    ISelectionFunctionPtr<Value, S, N> selector_ = nullptr;
    IParentChoiserPtr<Genotype, Value, N> parent_choiser_ = nullptr;
    IGeneticAlgorithmStrategyPtr<Genotype, Value, N> strategy_ = nullptr;

    Population current_population_;
    ScorePopulation current_population_score_;
};

} // GeneticAlgorithm

#endif //LAB_GENETICALGORITHM_H
