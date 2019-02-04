cmake_minimum_required(VERSION 3.12)
project(genetic_algorithm)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.65.1 COMPONENTS program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(genetic_algorithm main.cpp
            GeneticAlgorithm/GeneticAlgorithm.h
            GeneticAlgorithm/ISelectionFunction.h
            GeneticAlgorithm/IGeneticAlgorithmStrategy.h
            GeneticAlgorithm/Utils.h
            GeneticAlgorithm/Utils.inl
            GeneticAlgorithmImpl/ForwardSelectionFunction.h
            GeneticAlgorithmImpl/GeneticAlgorithmSolverFactory.h
            GeneticAlgorithmImpl/RosenbrokFunctionStrategy.h
            GeneticAlgorithmImpl/GeneticAlgorithmSolverFactory.cpp
            GeneticAlgorithmImpl/RosenbrokFunctionStrategy.cpp)
    target_link_libraries(genetic_algorithm ${Boost_LIBRARIES})
endif()