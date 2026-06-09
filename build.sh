#!/bin/bash

echo "Compiling CPU Scheduling Simulator..."

g++ -Iinclude src/main.cpp src/Scheduler.cpp src/FCFS.cpp src/SJF.cpp src/PriorityScheduler.cpp src/RoundRobin.cpp src/SRTF.cpp -o simulator

if [ $? -ne 0 ]; then
    echo ""
    echo "Compilation FAILED! Please check the errors above."
    exit 1
fi

echo "Compilation SUCCESSFUL!"
echo "Running the simulator now..."
echo "========================================================"
echo ""

./simulator
