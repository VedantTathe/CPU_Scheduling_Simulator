#include "SimulatorUI.h"
#include "Utils.h"
#include "Comparison.h"
#include "CPURuntime.h"
#include "SchedulerRegistry.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cstdlib>

SimulatorUI::SimulatorUI()
    : contextSwitchEnabledSetting(true),
      contextSwitchDelaySetting(1),
      contextSwitchRealTimeDelaySetting(200),
      numCoresSetting(2) {
    pluginManager.scanPlugins("./plugins");
}

void SimulatorUI::run() {
    printWelcome();

    bool running = true;
    while (running) {
        int choice = displayMainMenu();

        switch (choice) {
            case INPUT_PROCESSES:
                inputProcesses();
                break;

            case LOAD_FROM_FILE:
                loadProcessesFromFile();
                break;

            case RUN_SIMULATION:
                if (processes.empty()) {
                    std::cout << "\n❌ ERROR: No processes to schedule. Please input processes first.\n" << std::endl;
                } else {
                    runSimulation();
                }
                break;

            case VIEW_PROCESSES:
                viewProcesses();
                break;

            case CLEAR_PROCESSES:
                clearAllProcesses();
                break;

            case EXIT:
                running = false;
                break;

            default:
                std::cout << "\n❌ Invalid choice. Please try again.\n" << std::endl;
        }

        if (running && choice >= INPUT_PROCESSES && choice <= CLEAR_PROCESSES) {
            waitForUser();
        }
    }

    printGoodbye();
}

int SimulatorUI::displayMainMenu() {
    clearScreen();
    Utils::printHeader("CPU SCHEDULING SIMULATOR - Main Menu");

    std::cout << "Current Processes: " << processes.size() << std::endl;
    std::cout << "\n1. Input Processes Manually" << std::endl;
    std::cout << "2. Load Processes from File" << std::endl;
    std::cout << "3. Run Simulation" << std::endl;
    std::cout << "4. View Processes" << std::endl;
    std::cout << "5. Clear Processes" << std::endl;
    std::cout << "6. Exit" << std::endl;

    std::cout << "\nSelect option (1-6): ";

    std::string input;
    if (!std::getline(std::cin, input)) {
        return EXIT;
    }

    if (isValidInteger(input)) {
        int choice = std::stoi(input);
        if (choice >= 1 && choice <= 6) {
            return choice;
        }
    }

    return -1;  // Invalid choice
}

void SimulatorUI::inputProcesses() {
    clearScreen();
    Utils::printHeader("Input Processes");

    std::cout << "Enter number of processes: ";
    std::string numInput;
    std::getline(std::cin, numInput);

    if (!isValidInteger(numInput)) {
        std::cout << "❌ Invalid input. Please enter a positive integer." << std::endl;
        return;
    }

    int numProcesses = std::stoi(numInput);
    if (numProcesses <= 0) {
        std::cout << "❌ Number of processes must be greater than 0." << std::endl;
        return;
    }

    processes.clear();

    for (int i = 0; i < numProcesses; ++i) {
        std::cout << "\n--- Process " << (i + 1) << " ---" << std::endl;

        // PID
        std::cout << "Process ID: ";
        std::string pidInput;
        std::getline(std::cin, pidInput);
        if (!isValidInteger(pidInput) || std::stoi(pidInput) <= 0) {
            std::cout << "❌ Invalid PID. Skipping." << std::endl;
            --i;
            continue;
        }
        int pid = std::stoi(pidInput);

        // Arrival Time
        std::cout << "Arrival Time: ";
        std::string arrInput;
        std::getline(std::cin, arrInput);
        if (!isValidInteger(arrInput) || std::stoi(arrInput) < 0) {
            std::cout << "❌ Invalid arrival time. Skipping." << std::endl;
            --i;
            continue;
        }
        int arrival = std::stoi(arrInput);

        // Burst Time
        std::cout << "Burst Time: ";
        std::string burstInput;
        std::getline(std::cin, burstInput);
        if (!isValidInteger(burstInput) || std::stoi(burstInput) <= 0) {
            std::cout << "❌ Invalid burst time. Skipping." << std::endl;
            --i;
            continue;
        }
        int burst = std::stoi(burstInput);

        // Priority
        std::cout << "Priority (1=highest, higher number=lower priority): ";
        std::string prioInput;
        std::getline(std::cin, prioInput);
        if (!isValidInteger(prioInput) || std::stoi(prioInput) <= 0) {
            std::cout << "❌ Invalid priority. Skipping." << std::endl;
            --i;
            continue;
        }
        int priority = std::stoi(prioInput);

        processes.emplace_back(pid, arrival, burst, priority);
        std::cout << "[OK] Process " << pid << " added successfully." << std::endl;
    }

    std::cout << "\n[OK] Total processes added: " << processes.size() << std::endl;
}

void SimulatorUI::loadProcessesFromFile() {
    clearScreen();
    Utils::printHeader("Load Processes from File");

    std::cout << "Enter filename (or full path): ";
    std::string filename;
    std::getline(std::cin, filename);

    // Trim whitespace from input
    filename.erase(0, filename.find_first_not_of(" \t\r\n"));
    filename.erase(filename.find_last_not_of(" \t\r\n") + 1);

    if (filename.empty()) {
        std::cout << "❌ Filename cannot be empty." << std::endl;
        return;
    }

    try {
        std::vector<Process> loadedProcesses = FileHandler::loadProcessesFromFile(filename);
        
        // Clear existing processes and add new ones
        processes.clear();
        processes = loadedProcesses;
        
        std::cout << "\n[OK] Successfully loaded " << processes.size() << " processes from '" << filename << "'." << std::endl;
        
        // Display preview of loaded processes
        std::cout << "\n" << "Loaded Processes:" << std::endl;
        std::cout << std::left
                  << std::setw(6) << "PID"
                  << std::setw(12) << "Arrival"
                  << std::setw(10) << "Burst"
                  << std::setw(12) << "Priority"
                  << std::endl;
        Utils::printDivider(40, '-');
        
        for (const auto& p : processes) {
            std::cout << std::left
                      << std::setw(6) << p.pid
                      << std::setw(12) << p.arrivalTime
                      << std::setw(10) << p.burstTime
                      << std::setw(12) << p.priority
                      << std::endl;
        }
        
    } catch (const std::runtime_error& e) {
        std::cout << "\n❌ " << e.what() << std::endl;
    }
}

void SimulatorUI::viewProcesses() const {
    clearScreen();
    Utils::printHeader("Current Processes");

    if (processes.empty()) {
        std::cout << "No processes currently loaded." << std::endl;
        return;
    }

    std::cout << std::left
              << std::setw(6) << "PID"
              << std::setw(12) << "Arrival"
              << std::setw(10) << "Burst"
              << std::setw(12) << "Priority"
              << std::endl;
    Utils::printDivider(40, '-');

    for (const auto& p : processes) {
        std::cout << std::left
                  << std::setw(6) << p.pid
                  << std::setw(12) << p.arrivalTime
                  << std::setw(10) << p.burstTime
                  << std::setw(12) << p.priority
                  << std::endl;
    }
}

void SimulatorUI::runSimulation() {
    clearScreen();
    Utils::printHeader("Run Simulation");

    std::cout << "1. Run Single Algorithm (Standard)" << std::endl;
    std::cout << "2. Compare All Algorithms (Standard)" << std::endl;
    std::cout << "3. Run Multi-Core Live Simulation" << std::endl;
    std::cout << "4. Configure Context Switch Settings" << std::endl;
    std::cout << "5. Back to Main Menu" << std::endl;
    std::cout << "\nSelect option (1-5): ";

    std::string choice;
    if (!std::getline(std::cin, choice)) return;

    if (choice == "1") {
        std::string algoName = selectAlgorithm();
        if (!algoName.empty()) {
            executeSingleAlgorithm(algoName);
        }
    } else if (choice == "2") {
        runAllAlgorithmsComparison();
    } else if (choice == "3") {
        runMultiCoreSimulation();
    } else if (choice == "4") {
        configureContextSwitchSettings();
        runSimulation(); // Redraw menu after configuring
    } else if (choice == "5") {
        return;
    } else {
        std::cout << "❌ Invalid choice." << std::endl;
    }
}

std::string SimulatorUI::selectAlgorithm() {
    clearScreen();
    Utils::printHeader("Select Scheduling Algorithm");

    auto registered = SchedulerRegistry::getInstance().getRegisteredSchedulers();
    if (registered.empty()) {
        std::cout << "❌ No scheduling algorithms registered! Please verify plugins or built-in classes." << std::endl;
        return "";
    }

    // Sort registered algorithms so FCFS, SJF, Priority come in a clean order if they are registered
    std::sort(registered.begin(), registered.end(), [](const auto& a, const auto& b) {
        return a.name < b.name;
    });

    for (size_t i = 0; i < registered.size(); ++i) {
        std::cout << (i + 1) << ". " << registered[i].name << " (" << registered[i].description << ")" << std::endl;
    }
    std::cout << (registered.size() + 1) << ". Back to Selection Menu" << std::endl;

    std::cout << "\nSelect algorithm (1-" << (registered.size() + 1) << "): ";

    std::string choice;
    std::getline(std::cin, choice);

    if (isValidInteger(choice)) {
        int idx = std::stoi(choice);
        if (idx >= 1 && idx <= static_cast<int>(registered.size())) {
            return registered[idx - 1].name;
        } else if (idx == static_cast<int>(registered.size() + 1)) {
            return ""; // Back selected
        }
    }

    std::cout << "❌ Invalid choice." << std::endl;
    return "";
}

void SimulatorUI::executeSingleAlgorithm(const std::string& algoName) {
    if (algoName.empty()) return;
    clearScreen();

    auto scheduler = SchedulerRegistry::getInstance().createScheduler(algoName);
    if (!scheduler) {
        std::cout << "❌ Error: Could not instantiate scheduler for '" << algoName << "'." << std::endl;
        return;
    }

    // Add processes to scheduler
    for (const auto& p : processes) {
        scheduler->addProcess(p);
    }

    // Apply context switch settings
    scheduler->setContextSwitchSettings(contextSwitchEnabledSetting,
                                       contextSwitchDelaySetting,
                                       contextSwitchRealTimeDelaySetting,
                                       contextSwitchEnabledSetting);

    if (contextSwitchEnabledSetting) {
        Utils::printSectionHeader("Live OS Execution Trace");
    }

    // Run scheduler
    scheduler->run();
    scheduler->calculateMetrics();

    if (contextSwitchEnabledSetting) {
        std::cout << std::endl;
    }

    // Display results
    scheduler->displayResults();

    std::cout << "\n[OK] Simulation completed for " << scheduler->getAlgorithmName() << std::endl;

    // Ask if user wants to clear processes after simulation
    promptClearAfterSimulation();
}

void SimulatorUI::runAllAlgorithmsComparison() {
    clearScreen();
    Utils::printHeader("Running All Registered Algorithms...");

    auto registered = SchedulerRegistry::getInstance().getRegisteredSchedulers();
    if (registered.empty()) {
        std::cout << "❌ No registered scheduling algorithms found." << std::endl;
        return;
    }

    std::vector<std::unique_ptr<Scheduler>> instances;
    AlgorithmComparison comparison;

    for (const auto& info : registered) {
        auto scheduler = SchedulerRegistry::getInstance().createScheduler(info.name);
        if (scheduler) {
            // Add processes
            for (const auto& p : processes) {
                scheduler->addProcess(p);
            }
            // Apply context switch settings (no live display)
            scheduler->setContextSwitchSettings(contextSwitchEnabledSetting, contextSwitchDelaySetting, 0, false);
            
            // Run and calculate
            scheduler->run();
            scheduler->calculateMetrics();
            
            // Add to comparison list
            comparison.addScheduler(scheduler.get(), processes);
            
            // Store instance so memory stays valid during comparison display
            instances.push_back(std::move(scheduler));
        }
    }

    // Display individual results
    for (const auto& scheduler : instances) {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        scheduler->displayResults();
    }

    // Display comparison dashboard
    std::cout << "\n" << std::string(80, '=') << std::endl;
    comparison.runAllSchedulers();
    comparison.displayProfessionalDashboard();

    std::cout << "[OK] Comparison complete across all " << instances.size() << " registered schedulers." << std::endl;
}

void SimulatorUI::clearAllProcesses() {
    clearScreen();
    Utils::printHeader("Clear Processes");

    if (processes.empty()) {
        std::cout << "No processes to clear." << std::endl;
        return;
    }

    std::cout << "Are you sure you want to clear all processes? (Y/N): ";
    std::string response;
    std::getline(std::cin, response);

    if (response == "Y" || response == "y") {
        processes.clear();
        std::cout << "[OK] All processes cleared." << std::endl;
    } else {
        std::cout << "Cancelled." << std::endl;
    }
}

void SimulatorUI::promptClearAfterSimulation() {
    std::cout << "\nClear processes and start over? (Y/N, default: Y): ";
    std::string response;
    std::getline(std::cin, response);

    // Only skip clearing if user explicitly types 'N' or 'n'
    if (response == "N" || response == "n") {
        std::cout << "Keeping processes." << std::endl;
    } else {
        // Clear by default (Y, y, or just pressing Enter)
        processes.clear();
        std::cout << "[OK] Processes cleared. Ready for new simulation." << std::endl;
    }

    waitForUser();
}

void SimulatorUI::printWelcome() const {
    clearScreen();
    std::cout << "\n";
    std::cout << "==============================================================================\n";
    std::cout << "|                                                                            |\n";
    std::cout << "|              CPU SCHEDULING SIMULATOR - Interactive Version               |\n";
    std::cout << "|                                                                            |\n";
    std::cout << "|  Algorithms: FCFS | SJF | Priority Scheduling                            |\n";
    std::cout << "|  Features: Process Input | Simulation | Comparison | Metrics             |\n";
    std::cout << "|                                                                            |\n";
    std::cout << "==============================================================================\n";
    std::cout << "\n";
    waitForUser();
}

void SimulatorUI::printGoodbye() const {
    clearScreen();
    std::cout << "\n";
    std::cout << "==============================================================================\n";
    std::cout << "|                                                                            |\n";
    std::cout << "|                   Thank you for using CPU Scheduling Simulator!           |\n";
    std::cout << "|                         Goodbye! Bye!                                      |\n";
    std::cout << "|                                                                            |\n";
    std::cout << "==============================================================================\n";
    std::cout << "\n";
}

void SimulatorUI::waitForUser() const {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool SimulatorUI::isValidInteger(const std::string& input) const {
    if (input.empty()) return false;
    
    try {
        (void)std::stoi(input);
        return true;
    } catch (...) {
        return false;
    }
}

void SimulatorUI::clearScreen() const {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void SimulatorUI::runMultiCoreSimulation() {
    clearScreen();
    Utils::printHeader("Run Multi-Core Live Simulation");

    std::cout << "Enter number of CPU cores to simulate (1-8, default: 2): ";
    std::string coreInput;
    if (!std::getline(std::cin, coreInput)) return;

    int numCores = 2;
    if (coreInput.empty()) {
        numCores = 2;
    } else if (isValidInteger(coreInput)) {
        numCores = std::stoi(coreInput);
        if (numCores < 1 || numCores > 8) {
            std::cout << "⚠️ Cores must be between 1 and 8. Using default (2)." << std::endl;
            numCores = 2;
        }
    } else {
        std::cout << "⚠️ Invalid input. Using default (2)." << std::endl;
        numCores = 2;
    }

    std::string algoName = selectAlgorithm();
    if (algoName.empty()) {
        std::cout << "❌ Scheduling simulation cancelled." << std::endl;
        waitForUser();
        return;
    }

    auto scheduler = SchedulerRegistry::getInstance().createScheduler(algoName);
    if (!scheduler) {
        std::cout << "❌ Error: Could not instantiate scheduler for '" << algoName << "'." << std::endl;
        waitForUser();
        return;
    }

    CPURuntime runtime;
    std::vector<Process> tempProcesses = processes;

    runtime.run(tempProcesses, scheduler.get(), numCores, 
                contextSwitchEnabledSetting ? contextSwitchDelaySetting : 0, 
                contextSwitchRealTimeDelaySetting);

    // Calculate metrics and display results
    scheduler->calculateMetrics();
    scheduler->displayResults();

    std::cout << "\n[OK] Live Multi-Core Simulation completed for " << scheduler->getAlgorithmName() << std::endl;

    promptClearAfterSimulation();
}

void SimulatorUI::configureContextSwitchSettings() {
    clearScreen();
    Utils::printHeader("Context Switch Settings");
    
    std::cout << "Enable simulated context switching? (Y/N, default: Y): ";
    std::string csEnabledInput;
    std::getline(std::cin, csEnabledInput);
    if (csEnabledInput == "N" || csEnabledInput == "n") {
        contextSwitchEnabledSetting = false;
        contextSwitchDelaySetting = 0;
        contextSwitchRealTimeDelaySetting = 0;
        std::cout << "\n[OK] Context switching disabled." << std::endl;
        waitForUser();
        return;
    } else {
        contextSwitchEnabledSetting = true;
    }

    std::cout << "Enter Context Switch Delay (simulation units, default: 1): ";
    std::string csDelayInput;
    std::getline(std::cin, csDelayInput);
    if (csDelayInput.empty()) {
        contextSwitchDelaySetting = 1;
    } else if (isValidInteger(csDelayInput) && std::stoi(csDelayInput) >= 0) {
        contextSwitchDelaySetting = std::stoi(csDelayInput);
    } else {
        std::cout << "⚠️ Invalid input. Using default (1)." << std::endl;
        contextSwitchDelaySetting = 1;
    }

    std::cout << "Enter Real-Time step/sleep delay (ms, default: 200, 0 for instant): ";
    std::string csRealTimeInput;
    std::getline(std::cin, csRealTimeInput);
    if (csRealTimeInput.empty()) {
        contextSwitchRealTimeDelaySetting = 200;
    } else if (isValidInteger(csRealTimeInput) && std::stoi(csRealTimeInput) >= 0) {
        contextSwitchRealTimeDelaySetting = std::stoi(csRealTimeInput);
    } else {
        std::cout << "⚠️ Invalid input. Using default (200)." << std::endl;
        contextSwitchRealTimeDelaySetting = 200;
    }

    std::cout << "\n[OK] Context Switch Settings Updated:" << std::endl;
    std::cout << " - Context Switching: " << (contextSwitchEnabledSetting ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << " - Simulation Delay: " << contextSwitchDelaySetting << " units" << std::endl;
    std::cout << " - Real-Time Delay: " << contextSwitchRealTimeDelaySetting << " ms" << std::endl;
    waitForUser();
}
