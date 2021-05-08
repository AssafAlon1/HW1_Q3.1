# The first line states the minimum CMake version to use with this project
cmake_minimum_required(VERSION 3.0.0)

# We set hw0 as the project name, C as the target language
# A project can contain multiple build products
# When needed, can add CXX here (like so LANGUAGES C CXX)
project(map VERSION 0.1.0 LANGUAGES C)

# (Optionally uncomment): see more output from cmake during build
# including specific gcc commands
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set variables holding flags for gcc
set(MTM_FLAGS_DEBUG "-std=c99 --pedantic-errors -Wall -Werror")
set(MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG")  # MIGHT BE NDEBUG???

# Set the flags for gcc (can also be done using target_compile_options and a couple of other ways)
set(CMAKE_C_FLAGS ${MTM_FLAGS_DEBUG})

# Tell CMake to build an executable named "my_executable" 
# The executable will be created in the build/ directory
# If we were compiling multiple files, we could add them separated by spaces,
# eg add_executable(my_executable hey.c ho.c its.c me.c)
add_executable(my_executable map.c main.c)