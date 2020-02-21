cmake_minimum_required(VERSION 3.15)
project(audio)

set(CMAKE_CXX_STANDARD 17)

add_executable(audio main.cpp FileHandler.cpp FileHandler.h)