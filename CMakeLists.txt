cmake_minimum_required(VERSION 3.10)

project(CPUSchedulingSimulator
    DESCRIPTION "A modular CPU scheduling simulator for OS algorithms"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    main.cpp
)

# Create executable
add_executable(simulator ${SOURCES})

# Enable compiler warnings
if(MSVC)
    target_compile_options(simulator PRIVATE /W4)
else()
    target_compile_options(simulator PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional: Add tests subdirectory (when tests are ready)
# enable_testing()
# add_subdirectory(tests)

# Optional: Add algorithms subdirectory (when ready)
# add_subdirectory(algorithms)

# Print build configuration
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
