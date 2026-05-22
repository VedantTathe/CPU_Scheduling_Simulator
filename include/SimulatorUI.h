#pragma once

#include "Process.h"
#include "Scheduler.h"
#include "Comparison.h"
#include "FileHandler.h"
#include <vector>
#include <string>

/**
 * @class SimulatorUI
 * @brief Interactive command-line interface for the CPU Scheduling Simulator.
 * 
 * Provides:
 * - Interactive menu system
 * - Dynamic process input
 * - Algorithm selection
 * - Results display and comparison
 * - Simulation rerun capability
 */
class SimulatorUI {
public:
    /**
     * @brief Constructor
     */
    SimulatorUI();

    /**
     * @brief Start the interactive simulator.
     * Main loop that handles user interaction.
     */
    void run();

private:
    // UI State
    std::vector<Process> processes;

    // Main menu options
    enum MenuChoice {
        INPUT_PROCESSES = 1,
        LOAD_FROM_FILE = 2,
        RUN_SIMULATION = 3,
        VIEW_PROCESSES = 4,
        CLEAR_PROCESSES = 5,
        EXIT = 6
    };

    // Algorithm selection
    enum AlgorithmChoice {
        FCFS = 1,
        SJF = 2,
        PRIORITY = 3,
        BACK = 4
    };

    /**
     * @brief Display main menu and get user choice.
     * @return Selected menu option
     */
    int displayMainMenu();

    /**
     * @brief Get input for processes from user.
     */
    void inputProcesses();

    /**
     * @brief Load processes from a file.
     */
    void loadProcessesFromFile();

    /**
     * @brief Display current processes.
     */
    void viewProcesses() const;

    /**
     * @brief Run selected scheduling algorithm.
     */
    void runSimulation();

    /**
     * @brief Display algorithm selection menu.
     * @return Selected algorithm
     */
    int selectAlgorithm();

    /**
     * @brief Run single scheduling algorithm.
     * @param choice Algorithm to run
     */
    void executeSingleAlgorithm(int choice);

    /**
     * @brief Run all three algorithms for comparison.
     */
    void runAllAlgorithmsComparison();

    /**
     * @brief Clear all processes.
     */
    void clearAllProcesses();

    /**
     * @brief Ask user if they want to clear processes after simulation.
     */
    void promptClearAfterSimulation();

    /**
     * @brief Print welcome message.
     */
    void printWelcome() const;

    /**
     * @brief Print goodbye message.
     */
    void printGoodbye() const;

    /**
     * @brief Wait for user to press Enter.
     */
    void waitForUser() const;

    /**
     * @brief Validate input integer.
     * @param input String to validate
     * @return true if valid integer
     */
    bool isValidInteger(const std::string& input) const;

    /**
     * @brief Clear screen (cross-platform).
     */
    void clearScreen() const;
};
