cmake_minimum_required(VERSION 3.25.1)
project(advent-of-code-2025 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(advent-of-code-2025 advent-of-code-2025.cpp)
target_include_directories(advent-of-code-2025 PRIVATE include)

add_library(AdventOfCode2025)
target_sources(AdventOfCode2025 
    PRIVATE 
        AdventOfCode2025/ssl.cpp
        AdventOfCode2025/https_client.cpp
        AdventOfCode2025/day.cpp
        AdventOfCode2025/day1.cpp
    PUBLIC
    FILE_SET HEADERS
        BASE_DIRS
            include
        FILES 
            include/advent_of_code.h
)
find_package(OpenSSL COMPONENTS SSL)
target_link_libraries(advent-of-code-2025
    PRIVATE
        AdventOfCode2025
        OpenSSL::SSL
)