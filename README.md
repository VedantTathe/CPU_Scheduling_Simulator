# CPU Scheduling Simulator

A modular CPU scheduling simulator built in C++ implementing classical operating system scheduling algorithms.

## Overview

This project simulates various CPU scheduling algorithms used in operating systems, providing insights into process scheduling, CPU utilization, and performance metrics.

## Supported Scheduling Algorithms

- **FCFS** (First Come First Served) - Non-preemptive
- **SJF** (Shortest Job First) - Non-preemptive
- **Priority Scheduling** - Preemptive and Non-preemptive
- **Round Robin** - Preemptive with time quantum

## Features

- ✅ Multiple scheduling algorithm implementations
- ✅ Gantt chart visualization
- ✅ Scheduling metrics calculation (waiting time, turnaround time, etc.)
- ✅ CPU utilization analysis
- ✅ Process execution simulation
- ✅ Extensible architecture for new algorithms

## Tech Stack

- **Language**: C++17
- **Build System**: CMake 3.10+
- **Standard Library**: STL
- **CI/CD**: GitHub Actions (planned)

## Project Structure

```
cpu-scheduling-simulator/
├── src/                  # Source files
├── include/              # Header files
├── algorithms/           # Algorithm implementations
├── tests/                # Unit tests
├── docs/                 # Documentation
├── data/                 # Input/output data files
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10 or higher

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the simulator
./simulator
```

## Usage

```cpp
// Example usage (detailed documentation coming soon)
#include "scheduler.h"

int main() {
    // Create scheduler instance
    Scheduler scheduler;
    
    // Add processes
    scheduler.addProcess(1, 8, 0);   // PID, BurstTime, ArrivalTime
    scheduler.addProcess(2, 4, 1);
    scheduler.addProcess(3, 2, 2);
    
    // Run scheduling algorithm
    scheduler.runFCFS();
    
    // Display results
    scheduler.displayGanttChart();
    scheduler.displayMetrics();
    
    return 0;
}
```

## Planned Enhancements

- [ ] Preemptive scheduling algorithms
- [ ] Multilevel queue scheduling
- [ ] I/O operations support
- [ ] Performance metrics dashboard
- [ ] File-based configuration input
- [ ] Comparative analysis tool
- [ ] Visualization library integration

## Contributing

This is an educational project. Contributions are welcome! Please follow the coding standards and ensure all tests pass.

## License

MIT License - See LICENSE file for details

## Author

Development started: May 2026

---

**Note**: This project is designed for educational purposes to understand CPU scheduling algorithms in operating systems.
