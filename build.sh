#!/bin/bash

echo "Compiling CPU Scheduling Simulator..."

g++ main.cpp Scheduler.cpp FCFS.cpp SJF.cpp PriorityScheduler.cpp RoundRobin.cpp -o simulator

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
