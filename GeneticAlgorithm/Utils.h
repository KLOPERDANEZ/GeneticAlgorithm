#ifndef LAB_UTILS_H
#define LAB_UTILS_H

#include <random>
#include <memory>

class Randomizer;
using RandomizerPtr = std::shared_ptr<Randomizer>;

/**
 * \brief Объект для получения случайных равномерно распределенных положительных целых чисел в диапазоне
 */
class Randomizer
{
    std::random_device rd;
    mutable std::mt19937 gen;

    Randomizer()
    {
        gen = std::mt19937(rd());
    }

    Randomizer(const Randomizer&) = delete;
    Randomizer& operator=(Randomizer&) = delete;

public:

    /**
     * @brief получить число
     * @param start[in] - начало диапазона
     * @param finish[in] - конец диапазона
     * @return случайное число
     */
    size_t GetNumber(size_t start, size_t finish) const
    {
        return std::uniform_int_distribution<size_t>(start, finish - 1)(gen);
    }

    /**
     * @brief Получение инстанса объекта (синглтон Майерса)
     * @return ссылка на экземпляр объекта
     */
    static Randomizer& GetInstance()
    {
        static Randomizer instance;
        return instance;
    }
};

#endif //LAB_UTILS_H
