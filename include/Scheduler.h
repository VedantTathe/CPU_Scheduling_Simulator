#pragma once

#include "Process.h"
#include "Metrics.h"
#include <vector>
#include <string>

/**
 * @class Scheduler
 * @brief Abstract base class for CPU scheduling algorithms.
 * 
 * This class defines the interface that all scheduling algorithms must implement.
 * It provides a framework for:
 * - Managing process queues
 * - Calculating scheduling metrics
 * - Displaying results
 * 
 * Design Pattern: Strategy Pattern + Template Method
 * - Virtual functions allow different algorithm implementations
 * - Common functionality stays in base class
 * - Derived classes implement specific scheduling logic
 */
class Scheduler {
public:
    /**
     * @brief Virtual destructor.
     * Essential for polymorphic base classes to allow proper cleanup.
     */
    virtual ~Scheduler() = default;

    /**
     * @brief Add a process to the scheduler.
     * @param process Process to add
     */
    void addProcess(const Process& process);

    /**
     * @brief Add a process with parameters.
     * @param pid Process ID
     * @param arrival Arrival time
     * @param burst Burst time
     * @param priority Priority level (default 0)
     */
    void addProcess(int pid, int arrival, int burst, int priority = 0);

    /**
     * @brief Clear all processes from the scheduler.
     */
    void clearProcesses();

    /**
     * @brief Get the number of processes.
     * @return Number of processes in queue
     */
    int getProcessCount() const;

    /**
     * @brief Execute the scheduling algorithm.
     * Pure virtual function - must be implemented by derived classes.
     */
    virtual void run() = 0;

    /**
     * @brief Calculate scheduling metrics.
     * Must be called after run() to populate metrics.
     */
    virtual void calculateMetrics() = 0;

    /**
     * @brief Display scheduling results.
     * Shows processes, Gantt chart, and metrics.
     */
    virtual void displayResults() const = 0;

    /**
     * @brief Get algorithm name.
     * @return String name of the scheduling algorithm
     */
    virtual std::string getAlgorithmName() const = 0;

    /**
     * @brief Get calculated metrics.
     * @return Reference to Metrics structure
     */
    const Metrics& getMetrics() const;

    /**
     * @brief Reset all processes and metrics.
     * Allows running multiple scheduling simulations.
     */
    void reset();

protected:
    std::vector<Process> processes;        // Queue of processes to schedule
    Metrics metrics;                       // Calculated performance metrics
    int totalExecutionTime;                // Total time to complete all processes

    /**
     * @brief Protected constructor.
     * Prevents direct instantiation of abstract class.
     */
    Scheduler();

    /**
     * @brief Find process by ID.
     * @param pid Process ID to search for
     * @return Pointer to Process or nullptr if not found
     */
    Process* findProcessByPid(int pid);

    /**
     * @brief Find process by ID (const version).
     * @param pid Process ID to search for
     * @return Const pointer to Process or nullptr if not found
     */
    const Process* findProcessByPid(int pid) const;

    /**
     * @brief Get all completed processes.
     * @return Vector of completed processes
     */
    std::vector<Process> getCompletedProcesses() const;

    /**
     * @brief Sort processes by arrival time.
     */
    void sortByArrivalTime();

    /**
     * @brief Calculate average metrics from processes.
     * Helper method for derived classes.
     */
    void computeAverageMetrics();
};
