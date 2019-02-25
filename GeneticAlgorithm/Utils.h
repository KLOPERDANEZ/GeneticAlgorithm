#pragma once

#include "stable.h"

namespace GA
{

template<
        typename PopulationIterator,
        typename SampleIterator,
        typename Distance,
        typename UniformRandomBitGenerator>
SampleIterator sample_iterator(
        PopulationIterator first,
        PopulationIterator last,
        SampleIterator out,
        Distance n,
        UniformRandomBitGenerator&& g);

} // GeneticAlgorithm

#include "Utils.inl"
