cmake_minimum_required(VERSION 3.16)
project(arkane C CXX)

add_subdirectory("libs/SDL")
add_subdirectory("libs/imgui")

add_subdirectory("src")

