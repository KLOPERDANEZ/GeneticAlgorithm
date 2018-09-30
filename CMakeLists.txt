cmake_minimum_required(VERSION 3.12)
project(lab)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab main.cpp GeneticAlgorithm.h ISelectionFunction.h IParentChoiser.h)