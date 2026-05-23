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
    src/CPUProcess.cpp
    src/Scheduler.cpp
    src/Utils.cpp
    src/Comparison.cpp
    src/SimulatorUI.cpp
    src/FileHandler.cpp
    src/ReportGenerator.cpp
    src/ContextSwitch.cpp
    src/CPUCore.cpp
    src/CPURuntime.cpp
    src/DashboardRenderer.cpp
    src/SchedulerRegistry.cpp
    src/PluginManager.cpp
    src/AIWorkloadGenerator.cpp
    algorithms/FCFS.cpp
    algorithms/SJF.cpp
    algorithms/PriorityScheduler.cpp
)

find_package(Threads REQUIRED)

# Create executable
add_executable(simulator ${SOURCES})

target_link_libraries(simulator PRIVATE Threads::Threads ${CMAKE_DL_LIBS})

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

# Add Round Robin Plugin as a shared dynamic library
add_library(RoundRobinScheduler SHARED 
    plugins/RoundRobinScheduler.cpp 
    src/CPUProcess.cpp 
    src/Scheduler.cpp 
    src/Utils.cpp
    src/SchedulerRegistry.cpp
    src/ContextSwitch.cpp
)
target_include_directories(RoundRobinScheduler PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(WIN32)
    set_target_properties(RoundRobinScheduler PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
    )
else()
    set_target_properties(RoundRobinScheduler PROPERTIES
        PREFIX "lib"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
    )
endif()

# Copy compiled plugin directly to base plugins directory for runtime scanning
add_custom_command(TARGET RoundRobinScheduler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RoundRobinScheduler> ${CMAKE_SOURCE_DIR}/plugins/RoundRobinScheduler.dll
)

# Add EDF Plugin as a shared dynamic library
add_library(EDFScheduler SHARED 
    plugins/EDFScheduler.cpp 
    src/CPUProcess.cpp 
    src/Scheduler.cpp 
    src/Utils.cpp
    src/SchedulerRegistry.cpp
    src/ContextSwitch.cpp
)
target_include_directories(EDFScheduler PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(WIN32)
    set_target_properties(EDFScheduler PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
    )
else()
    set_target_properties(EDFScheduler PROPERTIES
        PREFIX "lib"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins
    )
endif()

# Copy compiled plugin directly to base plugins directory for runtime scanning
add_custom_command(TARGET EDFScheduler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:EDFScheduler> ${CMAKE_SOURCE_DIR}/plugins/EDFScheduler.dll
)

# Print build configuration
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
