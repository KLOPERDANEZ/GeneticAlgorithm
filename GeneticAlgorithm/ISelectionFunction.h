#pragma once

#include "stable.h"

namespace GA
{

template <typename Value = double>
class ISelectionFunction
{
    using ScorePopulation = std::vector<Value>;
public:
    virtual std::vector<double> Selection(const ScorePopulation& score_population) const = 0;

    virtual ~ISelectionFunction() = default;
};

template <typename Value = double>
using ISelectionFunctionPtr = std::shared_ptr<ISelectionFunction<Value>>;

}
