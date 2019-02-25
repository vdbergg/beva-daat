cmake_minimum_required(VERSION 3.13)
project(beva)

set(CMAKE_CXX_STANDARD 14)

add_executable(beva main.cpp cpp/Node.cpp header/Node.h cpp/Trie.cpp header/Trie.h cpp/EditVector.cpp header/EditVector.h cpp/utils.cpp header/utils.h cpp/ActiveNode.cpp header/ActiveNode.h cpp/EditVectorAutomata.cpp header/EditVectorAutomata.h cpp/State.cpp header/State.h cpp/Framework.cpp header/Framework.h cpp/Beva.cpp header/Beva.h header/C.h)