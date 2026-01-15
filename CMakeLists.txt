cmake_minimum_required(VERSION 3.16)
project(sudoku_genetic_algorithms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files (everything in src and main.cpp)
file(GLOB_RECURSE SOURCES "src/*.cpp" "main.cpp")
set(SOURCES ${SOURCES})


# Executable
add_executable(${PROJECT_NAME} ${SOURCES})
