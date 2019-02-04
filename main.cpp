#include <iomanip>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "GeneticAlgorithmImpl/GeneticAlgorithmSolverFactory.h"

namespace po = boost::program_options;

template <typename Type>
Type check_and_get_param(const po::variables_map& vm, const std::string& field)
{
    if (!vm.count(field))
    {
        throw std::runtime_error("\"" + field + "\", argument not required");
    }

    return vm[field].as<Type>();
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

        const auto& result = solver->Calculation(mutation_part, crossingover_part, limit);

        std::ofstream stream(out_file_name, std::ios::out);
        for (size_t index = 0; index < genotype_size; ++index)
        {
            stream << result.first[index].x() << "\t" << result.first[index].y() << "\t" << result.second[index] << std::endl;
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
