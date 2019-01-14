cmake_minimum_required(VERSION 3.13)
project(beva)

set(CMAKE_CXX_STANDARD 14)

add_executable(beva main.cpp Node.cpp Node.h Trie.cpp Trie.h EditDistance.cpp EditDistance.h)