#pragma once

#include "CPUProcess.h"
#include "Scheduler.h"
#include "Comparison.h"
#include "FileHandler.h"
#include "PluginManager.h"
#include "AIWorkloadGenerator.h"
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

    // Context Switch Simulation settings
    bool contextSwitchEnabledSetting;
    int contextSwitchDelaySetting;
    int contextSwitchRealTimeDelaySetting;
    int numCoresSetting;

    // Main menu options
    enum MenuChoice {
        INPUT_PROCESSES = 1,
        LOAD_FROM_FILE = 2,
        AI_GENERATION = 3,
        RUN_SIMULATION = 4,
        VIEW_PROCESSES = 5,
        CLEAR_PROCESSES = 6,
        EXIT = 7
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
     * @brief Generate realistic processes using AI workload engine.
     */
    void generateAIWorkload();

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
     * @return Selected algorithm name (empty if Back selected)
     */
    std::string selectAlgorithm();

    /**
     * @brief Run single scheduling algorithm.
     * @param algoName Unique name of algorithm to run
     */
    void executeSingleAlgorithm(const std::string& algoName);

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
     * @brief Configure the context switch settings interactively.
     */
    void configureContextSwitchSettings();

    /**
     * @brief Run simulated multi-core CPU scheduler live.
     */
    void runMultiCoreSimulation();

    /**
     * @brief Automatically analyze and run the optimal scheduling algorithm.
     */
    void runAdaptiveSimulation();

    /**
     * @brief Clear screen (cross-platform).
     */
    void clearScreen() const;

    PluginManager pluginManager;
};
