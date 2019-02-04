// #include "Utils.h"

namespace GA
{

//! From gcc, for iterator
//! my adaptation https://github.com/KLOPERDANEZ/different/blob/master/sample_iterator.h

template<
		typename InputIterator,
		typename RandomAccessIterator,
		typename Size,
		typename UniformRandomBitGenerator>
RandomAccessIterator sample_iter(
		InputIterator first,
		InputIterator last,
		std::input_iterator_tag,
		RandomAccessIterator out,
		std::random_access_iterator_tag,
		Size n,
		UniformRandomBitGenerator&& g)
{
	using distrib_type = std::uniform_int_distribution<Size>;
	using param_type = typename distrib_type::param_type;
	distrib_type d{};
	Size sample_sz = 0;

	while (first != last && sample_sz != n)
	{
		out[sample_sz++] = first;
		++first;
	}

	for (auto pop_sz = sample_sz; first != last; ++first, (void) ++pop_sz)
	{
		const auto k = d(g, param_type{0, pop_sz});
		if (k < n)
		{
			out[k] = first;
		}
	}
	return out + sample_sz;
}

template<typename IntType, typename UniformRandomBitGenerator>
std::pair <IntType, IntType> gen_two_uniform_ints(
		IntType b0,
		IntType b1,
		UniformRandomBitGenerator&& g)
{
	IntType x = std::uniform_int_distribution < IntType > {0, (b0 * b1) - 1}(g);
	return std::make_pair(x / b1, x % b1);
}

template<
		typename ForwardIterator,
		typename OutputIterator,
		typename Cat,
		typename Size,
		typename UniformRandomBitGenerator>
OutputIterator sample_iter(
		ForwardIterator first,
		ForwardIterator last,
		std::forward_iterator_tag,
		OutputIterator out,
		Cat,
		Size n,
		UniformRandomBitGenerator&& g)
{
	using distrib_type = std::uniform_int_distribution<Size>;
	using param_type = typename distrib_type::param_type;
	using USize = std::make_unsigned_t<Size>;
	using Gen = std::remove_reference_t<UniformRandomBitGenerator>;
	using uc_type = std::common_type_t<typename Gen::result_type, USize>;

	distrib_type d{};
	Size unsampled_size = std::distance(first, last);
	n = std::min(n, unsampled_size);

	const uc_type urngrange = g.max() - g.min();
	if (urngrange / uc_type(unsampled_size) >= uc_type(unsampled_size))
	{
		while (n != 0 && unsampled_size >= 2)
		{
			const std::pair <Size, Size> p = gen_two_uniform_ints(unsampled_size, unsampled_size - 1, g);

			--unsampled_size;
			if (p.first < n)
			{
				*out = first;
				out++;
				--n;
			}

			++first;

			if (n == 0)
			{
				break;
			}

			--unsampled_size;
			if (p.second < n)
			{
				*out = first;
				out++;
				--n;
			}

			++first;
		}
	}

	for (; n != 0; ++first)
	{
		if (d(g, param_type{0, --unsampled_size}) < n)
		{
			*out = first;
			out++;
			--n;
		}
	}

	return out;
}


template<typename PopulationIterator, typename SampleIterator, typename Distance, typename UniformRandomBitGenerator>
SampleIterator sample_iterator(PopulationIterator first, PopulationIterator last, SampleIterator out, Distance n,
							   UniformRandomBitGenerator&& g)
{
	using pop_cat = typename std::iterator_traits<PopulationIterator>::iterator_category;
	using samp_cat = typename std::iterator_traits<SampleIterator>::iterator_category;

	static_assert(
			std::__or_ < std::is_convertible < pop_cat, std::forward_iterator_tag > ,
			std::is_convertible < samp_cat, std::random_access_iterator_tag >> ::value,
			"output range must use a RandomAccessIterator when input range"
			" does not meet the ForwardIterator requirements");

	static_assert(std::is_integral<Distance>::value,
				  "sample size must be an integer type");

	typename std::iterator_traits<PopulationIterator>::difference_type d = n;
	return sample_iter(first, last, pop_cat{}, out, samp_cat{}, d, std::forward<UniformRandomBitGenerator>(g));
}

} // GeneticAlgorithm
