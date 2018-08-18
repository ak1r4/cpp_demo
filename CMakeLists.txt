cmake_minimum_required(VERSION 3.9)
project(cpp-demo)

add_executable(test-false-sharing test_false_sharing.cpp)
target_link_libraries(test-false-sharing pthread benchmark)
