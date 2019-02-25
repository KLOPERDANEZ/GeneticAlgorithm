#include <iomanip>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "GeneticAlgorithmImpl/GeneticAlgorithmSolverFactory.h"

namespace po = boost::program_options;

void check_param(const po::variables_map& vm, const std::string& field)
{
    if (!vm.count(field))
    {
        throw std::runtime_error("\"" + field + "\", argument not required");
    }
}

template <typename Type>
Type check_and_get_param(const po::variables_map& vm, const std::string& field)
{
    check_param(vm, field);
    return vm[field].as<Type>();
}


void WriteState(std::ofstream& stream, const GA::State<Point2d, double>& state)
{
    for (size_t index = 0; index < state.current_population_score_.size(); ++index)
    {
        stream
                << state.current_population_[index].x() << "\t"
                << state.current_population_[index].y() << "\t"
                << state.current_population_score_[index] << std::endl;
    }
}

void DumpProcess(
        const std::string& basic_string,
        const GA::GeneticAlgorithmResult<Point2d, double>& result)
{
    std::ofstream dump_file(basic_string, std::ios::out);

    if (result.times_ != std::nullopt)
    {
        dump_file << "Calculation time: " << *result.times_ << " ns." << std::endl;
    }

    dump_file << "Iteration count: " << result.iteration_count_ << std::endl;

    if (!result.states_)
    {
        return;
    }

    dump_file << std::endl;

    for (size_t index = 0; index < result.states_->size(); ++index)
    {
        const auto& state = (*result.states_)[index];
        dump_file << "Iteration number = " << index << std::endl;
        WriteState(dump_file, state);
        dump_file << std::endl << std::endl;
    }
}

auto CallCalculate(
        const bool is_dumping_process,
        const bool is_measuring_time,
        const double mutation_part,
        const double crossingover_part,
        const size_t limit,
        const GA::GeneticAlgorithm<Point2d, double>& solver)
{
    if (is_dumping_process)
    {
        if (is_measuring_time)
        {
            return solver.Calculation<true, true>(mutation_part, crossingover_part, limit);
        }
        else
        {
            return solver.Calculation<true, false>(mutation_part, crossingover_part, limit);
        }
    }
    else
    {
        if (is_measuring_time)
        {
            return solver.Calculation<false, true>(mutation_part, crossingover_part, limit);
        }
        else
        {
            return solver.Calculation<false, false>(mutation_part, crossingover_part, limit);
        }
    }
}

int main(int argc, char* argv[])
{
    po::options_description desc("Options");
    desc.add_options()
            ("help", "Print help messages")
            ("function-type", po::value<std::string>()->required(), "Research function : rosenbrock")
            ("genotype-size", po::value<size_t>()->required(), "Genotype size")
            ("mutation-part", po::value<double>()->required(), "Mutation part")
            ("crossingover-part", po::value<double>()->required(), "Crossingover part")
            ("max-iteration-count", po::value<size_t>()->required(), "Max iteration count")
            ("result-file", po::value<std::string>()->required(), "Result file")
            ("dump-file", po::value<std::string>(), "Dump file")
            ("save-state", "Save state")
            ("measuring-time", "Measuring time")
            ("selection-function-type", po::value<std::string>(), "Selection function type: simple-forward");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help"))
        {
            std::cout << "Basic Command Line Parameter App" << std::endl << desc << std::endl;
            return 0;
        }

        const auto& function_type = check_and_get_param<std::string>(vm, "function-type");
        const auto& out_file_name = check_and_get_param<std::string>(vm, "result-file");
        const size_t genotype_size = check_and_get_param<size_t>(vm, "genotype-size");
        const double mutation_part = check_and_get_param<double>(vm, "mutation-part");
        const double crossingover_part = check_and_get_param<double>(vm, "crossingover-part");

        const std::string& selection_function_type = vm.count("selection-function-type")
                ? vm["selection-function-type"].as<std::string>() : "";

        const size_t limit = vm.count("max-iteration-count")
                ? vm["max-iteration-count"].as<size_t>() : 1000;

        const auto solver = GeneticAlgorithmSolverFactory::CreateGeneticAlgorithmPoint2dSolver(
                function_type,
                selection_function_type,
                genotype_size);

        if (!solver)
        {
            throw std::runtime_error("Empty solver");
        }

        const bool is_measuring_time = static_cast<const bool>(vm.count("measuring-time"));
        const bool is_save_state = static_cast<const bool>(vm.count("save_state"));

        const auto& result =
                CallCalculate(is_save_state, is_measuring_time, mutation_part, crossingover_part, limit, *solver);

        std::ofstream stream(out_file_name, std::ios::out);
        WriteState(stream, result.final_state_);

        if (vm.count("dump-file"))
        {
            DumpProcess(vm["dump-file"].as<std::string>(), result);
        }
    }
    catch (const po::error& program_option)
    {
        std::cerr << "ERROR: " << program_option.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return -1;
    }
    catch (const std::exception& error)
    {
        std::cerr << "ERROR: " << error.what() << std::endl;
        return -2;
    }

    return 0;
}
