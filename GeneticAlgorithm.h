#ifndef LAB_GENETICALGORITHM_H
#define LAB_GENETICALGORITHM_H

#include <array>
#include <random>
#include <vector>
#include "ISelectionFunction.h"
#include "IParentChoiser.h"

class Randomizer {
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<size_t> dis;

public:
    Randomizer(size_t start, size_t finish)
    {
        gen = std::mt19937(rd);
        dis = std::uniform_int_distribution<size_t>(start, finish);
    }
    size_t GetNumber()
    {
        return dis(gen);
    }
};

/*
 * Класс реализующий решение оптимизационной задачи генетическим алгоритмом
 * Реализован посредством паттерна шаблонный метод
 * https://ru.wikipedia.org/wiki/Генетический_алгоритм
 * \param Genotype генотип минимизируемой функции
 * \param M процент мутируемых особей
 * \param N размер популяции
 * \param Value тип результата вычисления оценки качества гетотипа
 * \param S процент выживших генов в популяции
 */
template <typename Genotype, size_t M = 10, size_t N = 1000, typename Value = double, size_t S = 20>
class GeneticAlgorithm {

    using Population = std::array<Genotype, N>;
    using ScorePopulation = std::array<Value, N>;
    using Survivors = std::array<bool, N>;

public:
    Population Calculation()
    {
        current_population_ = CreateStartPopulation();
        CalculationCurrentPopulationFitness();
        while (!IsCorrectResult(current_population_, current_population_score_))
        {
            const Survivors& survivors = selector_->Selection(current_population_);
            ApplyCrossingoverToPopulation(survivors, current_population_);
            ApplyMutationToPopulation(current_population_);
            CalculationCurrentPopulationFitness();
        }
    }

protected:
    virtual Population CreateStartPopulation() const = 0;
    virtual Genotype& Mutation(const Genotype&) const = 0;
    virtual Genotype& Crossingover(const Genotype&, const Genotype&) const = 0;
    virtual Value FitnessFunction(const Genotype &) const = 0;
    virtual bool IsCorrectResult(const Population&, const ScorePopulation&) const = 0;

private:

    void CalculationCurrentPopulationFitness()
    {
        ApplyFitnessFuntionToPopulation(current_population_, current_population_score_);
    }

    void ApplyFitnessFuntionToPopulation(const Population& population,
            const ScorePopulation& score_population) const
    {
        for (size_t index = 0; index < population.size(); ++index)
        {
            const auto& genotype = population[index];
            score_population[index] = FitnessFunction(genotype);
        }
    }

    void ApplyMutationToPopulation(Population& population) const
    {
        Randomizer randomizer(0, population.size());
        for (size_t counter = 0; counter < mutation_count; ++counter)
        {
            const size_t index = randomizer.GetNumber();
            const Genotype &genotype = population[index];
            population[index] = Mutation(genotype);
        }
    }

    void ApplyCrossingoverToPopulation(const Survivors& survivors, Population& population) const
    {
        if (survivors.size() != population.size())
        {
            throw std::runtime_error("Incorrect size survivors");
        }

        auto& [survivors_index, survivor_count] = GetSurvivorIndex();
        Randomizer survivor_randomizer(0, population.size());
        for (size_t index = 0; index < survivors.size(); ++index)
        {
            if (!survivors[index])
            {
                const size_t survivor_index = survivor_randomizer.GetNumber();
                const size_t genotype_index = survivors_index[survivor_index];
                const Genotype& parent = population[genotype_index]
                population[index] = Crossingover(parent, parent_choiser_->ChoiseParent(parent, population));
            }
        }
    }

    std::pair<std::array<size_t, N>, size_t> GetSurvivorIndex(const Survivors& survivors)
    {
        std::array<size_t, N> survivor_index;
        size_t survivor_count = 0;
        for (size_t index = 0; index < survivors.size(); ++index)
        {
            if (survivors[index])
            {
                survivor_index[survivor_count++] = index;
            }
        }
        return { survivor_index, survivor_count };
    }

    const size_t mutation_count = static_cast<const size_t>(N / 100.0 * M);

    ISelectionFunctionPtr<Value, S> selector_;
    IParentChoiserPtr<Genotype, Value> parent_choiser_;
    Population current_population_;
    ScorePopulation current_population_score_;
};


#endif //LAB_GENETICALGORITHM_H
