#ifndef LAB_ISELECTIONFUNCTION_H
#define LAB_ISELECTIONFUNCTION_H

#include <array>
#include <memory>

/*
 * Интерфейс класса для функции селекции
 * \param S процент выживших генов в популяции
 */
template <typename Value = double, size_t S = 20, size_t N = 1000>
class ISelectionFunction {
public:
    virtual std::array<bool, N> Selection(std::array<Value, N>) = 0;
};

template <typename Value = double, size_t S = 20, size_t N = 1000>
using ISelectionFunctionPtr = std::shared_ptr<ISelectionFunction<Value, S, N>>;

#endif //LAB_ISELECTIONFUNCTION_H
