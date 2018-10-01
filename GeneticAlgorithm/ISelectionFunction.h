#ifndef LAB_ISELECTIONFUNCTION_H
#define LAB_ISELECTIONFUNCTION_H

#include <array>
#include <memory>

/**
 * \brief Интерфейс класса для функции селекции
 * @tparam Value - тип результата вычисления функции от гетотипа
 * @tparam S - процент выживших в одной популяции
 * @tparam N - размер популяции
 */
template <typename Value = double, size_t S = 20, size_t N = 1000>
class ISelectionFunction
{
    using ScorePopulation = std::array<Value, N>;
public:
    /**
     * @brief Вычисляет шанс остаться живым
     * @param score_population[in] - оценка генома
     * @return возвращает шанс остаться в живых
     */
    virtual std::array<double, N> Selection(const ScorePopulation& score_population) const = 0;
};

template <typename Value = double, size_t S = 20, size_t N = 1000>
using ISelectionFunctionPtr = std::shared_ptr<ISelectionFunction<Value, S, N>>;

#endif //LAB_ISELECTIONFUNCTION_H
