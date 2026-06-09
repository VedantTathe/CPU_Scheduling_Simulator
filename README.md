# CPU Scheduling Simulator

A fast, modular C++ simulator that models the behavior of common CPU scheduling algorithms. 

This project was built to demonstrate proficiency in C++, object-oriented design, and standard development workflows (CMake).

## Supported Algorithms
- **FCFS** (First-Come, First-Served)
- **SJF** (Shortest Job First - Non-preemptive)
- **SRTF** (Shortest Remaining Time First - Preemptive)
- **Priority** (Preemptive / Non-preemptive)
- **Round Robin**

## Prerequisites
- **C++17** or higher
- **CMake** (v3.10+)
- A standard C++ compiler (GCC, Clang, or MSVC)

## Build Instructions (Out-of-Source Build)
This project uses CMake to ensure cross-platform compatibility and clean source directories. 

```bash
# 1. Create a build directory
mkdir build
cd build

# 2. Generate the build files
cmake ..

# 3. Compile the project
cmake --build .
```

## Running the Simulator
Once built, you can run the executable from the `build` directory:

**On Windows (Visual Studio / MSVC)**:
```bash
.\Debug\simulator.exe
```

**On Linux / Mac (or MinGW/GCC)**:
```bash
./simulator
```

## Input File Format
By default, the simulator may read from `input.txt` (if configured to do so). Ensure your input files follow the structure expected by the simulator.

## Project Structure
- `/include`: Header files defining the interfaces of the schedulers and processes.
- `/src`: Implementation files for the simulator logic.
- `CMakeLists.txt`: Build configuration file.
- `ALGORITHMS.md`: Detailed breakdown of the algorithms implemented.
