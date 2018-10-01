#ifndef LAB_IPARENTCHOISER_H
#define LAB_IPARENTCHOISER_H

#include <memory>

/**
 * @brief Реализует стратегию выбора родителя
 * @tparam Genotype - генотип
 * @tparam Value - тип результата вычисления функции от гетотипа
 * @tparam N - размер популяции
 */
template <typename Genotype, typename Value, size_t N = 1000>
class IParentChoiser
{
    using Population = std::array<Genotype, N>;
public:
    /**
     * @brief Выбирает родителя
     * @param parent - первый родитель, выбран случайно
     * @param population - среди кого будем искать
     * @return индекс второго родителя
     */
    virtual size_t ChoiseParent(const Genotype& parent, const Population& population) const = 0;
};

template <typename Genotype, typename Value, size_t N = 1000>
using IParentChoiserPtr = std::shared_ptr<IParentChoiser<Genotype, Value, N>>;


#endif //LAB_IPARENTCHOISER_H
