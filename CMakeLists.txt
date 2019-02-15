cmake_minimum_required(VERSION 3.13)
project(beva)

set(CMAKE_CXX_STANDARD 14)

add_executable(beva main.cpp Node.cpp Node.h Trie.cpp Trie.h EditVector.cpp EditVector.h utils.cpp utils.h ActiveNode.cpp ActiveNode.h EditVectorAutomaton.cpp EditVectorAutomaton.h State.cpp State.h Framework.cpp Framework.h Beva.cpp Beva.h)