#pragma once

#include <iostream>

/**
 * @class Process
 * @brief Represents a process in the CPU scheduling simulator.
 * 
 * This class models a process with scheduling-related attributes.
 * It supports both preemptive and non-preemptive scheduling algorithms.
 */
class Process {
public:
    // Attributes
    int pid;                    // Process ID
    int arrivalTime;           // Time when process arrives in ready queue
    int burstTime;             // Total CPU time needed (original burst time)
    int priority;              // Priority level (lower = higher priority)
    int remainingTime;         // Remaining burst time (for preemptive algorithms)
    int completionTime;        // Time when process completes
    int waitingTime;           // Total time spent waiting
    int turnaroundTime;        // Total time from arrival to completion
    int responseTime;          // Time from arrival to first execution

    /**
     * @brief Default constructor.
     * Initializes all fields to 0.
     */
    Process();

    /**
     * @brief Parameterized constructor.
     * @param id Process identifier
     * @param arrival Time when process arrives
     * @param burst Total CPU burst time needed
     * @param prio Priority level
     */
    Process(int id, int arrival, int burst, int prio = 0);

    /**
     * @brief Reset process metrics.
     * Resets timing metrics while preserving scheduling parameters.
     * Useful for running multiple scheduling simulations on same process set.
     */
    void reset();

    /**
     * @brief Display process information.
     */
    void display() const;

    /**
     * @brief Check if process has completed execution.
     * @return true if remaining time is 0
     */
    bool isCompleted() const;
};
