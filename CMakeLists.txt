cmake_minimum_required(VERSION 3.12)

project(loltaxx)

###
# Find packages
###
find_package(Threads REQUIRED)
###

###
# Base directory relative includes for everyone
###
include_directories(.)
include_directories(third_party)
###

###
# Global Flags
###
add_compile_options(-Wall -Wextra -Wpedantic)
###

###
# Flags
###
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -march=native -flto")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
###

###
# Targets
###
# engine
add_executable(
    loltaxx
    app/main.cpp
)
target_link_libraries(loltaxx PRIVATE Threads::Threads)

# perft
add_executable(
    perft
    app/perft.cpp
)
target_link_libraries(perft PRIVATE Threads::Threads)
