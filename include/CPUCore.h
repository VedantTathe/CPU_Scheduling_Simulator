#pragma once

#include "CPUProcess.h"
#include <thread>
#include <mutex>
#include <atomic>

/**
 * @class CPUCore
 * @brief Simulates a physical CPU Core running concurrently on a dedicated std::thread.
 * 
 * Executes an assigned process in clock cycles (ticks) and coordinates
 * with the central CPURuntime scheduler.
 */
class CPUCore {
private:
    int coreId;
    std::thread workerThread;
    std::atomic<bool> running;
    
    // Process currently assigned to this core
    Process* activeProcess;
    mutable std::mutex coreMutex;
    
    // Core timing parameters
    std::atomic<int> tickIntervalMs;
    
    // Signal flag that is set when active process finishes
    std::atomic<bool> processJustFinished;

    // Context switching flag
    std::atomic<bool> contextSwitching;

public:
    /**
     * @brief Constructor.
     * @param id The identifier for this core (e.g. 1, 2)
     */
    CPUCore(int id);
    
    /**
     * @brief Destructor. Ensures worker thread is stopped and joined.
     */
    ~CPUCore();

    int getId() const { return coreId; }
    
    /**
     * @brief Check if the core has no active process.
     */
    bool isIdle() const;
    
    /**
     * @brief Assign a process to this core for execution.
     * @param process Pointer to the process to execute
     * @param tickInterval Real-time tick duration in ms
     * @return True if assignment succeeded, false if core was busy
     */
    bool assignProcess(Process* process, int tickInterval);
    
    /**
     * @brief Retrieve the currently assigned process.
     */
    Process* getActiveProcess();
    
    /**
     * @brief Terminate and release the current process from the core.
     */
    void releaseProcess();
    
    /**
     * @brief Check and clear the processJustFinished flag.
     * @return True if a process completed on the last tick
     */
    bool checkAndClearFinished();

    /**
     * @brief Check if the core is currently context switching.
     */
    bool isSwitching() const;

    /**
     * @brief Set context switching state.
     */
    void setSwitching(bool switching);

    /**
     * @brief Spawns the worker thread.
     */
    void start();
    
    /**
     * @brief Signals the worker thread to stop and joins it.
     */
    void stop();

private:
    /**
     * @brief The thread function executed by workerThread.
     */
    void coreLoop();
};
