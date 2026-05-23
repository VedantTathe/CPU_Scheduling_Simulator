#include "CPURuntime.h"
#include "ContextSwitch.h"
#include "Utils.h"
#include "DashboardRenderer.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

CPURuntime::CPURuntime() : totalSimulationTime(0) {}

CPURuntime::~CPURuntime() {
    // Ensure all cores are stopped on destruction
    for (auto& core : cores) {
        if (core) {
            core->stop();
        }
    }
}

Process* CPURuntime::selectNextProcess(const std::vector<Process*>& eligible, const std::string& algoName) {
    if (eligible.empty()) return nullptr;
    
    Process* selected = eligible[0];
    for (size_t i = 1; i < eligible.size(); ++i) {
        Process* p = eligible[i];
        if (algoName.find("FCFS") != std::string::npos) {
            // FCFS: Earliest arrival time first, then lower PID
            if (p->arrivalTime < selected->arrivalTime) {
                selected = p;
            } else if (p->arrivalTime == selected->arrivalTime && p->pid < selected->pid) {
                selected = p;
            }
        } else if (algoName.find("SJF") != std::string::npos) {
            // SJF: Smallest remaining burst time, then arrival time, then lower PID
            if (p->remainingTime < selected->remainingTime) {
                selected = p;
            } else if (p->remainingTime == selected->remainingTime && p->arrivalTime < selected->arrivalTime) {
                selected = p;
            } else if (p->remainingTime == selected->remainingTime && p->arrivalTime == selected->arrivalTime && p->pid < selected->pid) {
                selected = p;
            }
        } else if (algoName.find("Priority") != std::string::npos) {
            // Priority: Smallest priority number (highest priority), then arrival time, then lower PID
            if (p->priority < selected->priority) {
                selected = p;
            } else if (p->priority == selected->priority && p->arrivalTime < selected->arrivalTime) {
                selected = p;
            } else if (p->priority == selected->priority && p->arrivalTime == selected->arrivalTime && p->pid < selected->pid) {
                selected = p;
            }
        }
    }
    return selected;
}

void CPURuntime::run(std::vector<Process>& simProcesses, Scheduler* scheduler, int numCores, int switchDelay, int realTimeDelayMs) {
    if (simProcesses.empty()) {
        std::cerr << "Error: No processes to run!" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(printMutex);
    
    // 1. Initialize simulation parameters
    int tickIntervalMs = 200; // 200ms per simulation time unit/tick
    int currentTime = 0;
    
    // Clear and prepare cores
    cores.clear();
    previousProcesses.clear();
    for (int i = 0; i < numCores; ++i) {
        cores.push_back(std::make_unique<CPUCore>(i + 1));
        previousProcesses.push_back(nullptr);
    }

    // Set process initial states (WAITING initially, will arrive at arrivalTime)
    for (auto& p : simProcesses) {
        p.reset();
        p.state = ProcessState::WAITING;
    }

    // Initialize Dashboard Renderer
    DashboardRenderer renderer;
    renderer.initialize();
    
    // Clear screen first to avoid remnants
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    // Start all core threads
    for (auto& core : cores) {
        core->start();
    }

    // 2. Main Simulation loop
    bool allCompleted = false;
    while (!allCompleted) {
        // Log Timer Interrupt Event at start of tick
        renderer.addEvent(currentTime, "\033[1m\033[35m[TIMER INTERRUPT]\033[0m Scheduler Invoked");

        // Process arrived notifications
        for (auto& p : simProcesses) {
            if (p.state == ProcessState::WAITING && p.arrivalTime <= currentTime) {
                p.state = ProcessState::READY;
                scheduler->recordTransition(currentTime, p.pid, ProcessState::WAITING, ProcessState::READY);
                renderer.addEvent(currentTime, "Process \033[1m\033[36mP" + std::to_string(p.pid) + "\033[0m Arrived: WAITING -> READY");
            }
        }

        // Check for processes completed on the last tick
        for (int i = 0; i < numCores; ++i) {
            auto& core = cores[i];
            if (core->checkAndClearFinished()) {
                Process* completedProcess = core->getActiveProcess();
                if (completedProcess != nullptr) {
                    completedProcess->completionTime = currentTime;
                    completedProcess->turnaroundTime = completedProcess->completionTime - completedProcess->arrivalTime;
                    completedProcess->waitingTime = completedProcess->turnaroundTime - completedProcess->burstTime;
                    if (completedProcess->waitingTime < 0) completedProcess->waitingTime = 0;
                    
                    renderer.addEvent(currentTime, "Process \033[1m\033[32mP" + std::to_string(completedProcess->pid) + "\033[0m Completed on Core " + std::to_string(core->getId()));
                    scheduler->recordTransition(currentTime, completedProcess->pid, ProcessState::RUNNING, ProcessState::COMPLETED);
                    core->releaseProcess();
                }
            }
        }

        // Schedule / dispatch READY processes onto IDLE cores
        for (int i = 0; i < numCores; ++i) {
            auto& core = cores[i];
            if (core->isIdle() && !core->isSwitching()) {
                // Find all eligible processes in READY state that have arrived
                std::vector<Process*> eligible;
                for (auto& p : simProcesses) {
                    if (p.state == ProcessState::READY && p.arrivalTime <= currentTime) {
                        eligible.push_back(&p);
                    }
                }

                Process* nextP = selectNextProcess(eligible, scheduler->getAlgorithmName());
                if (nextP != nullptr) {
                    // Mark core as context switching
                    core->setSwitching(true);
                    
                    // Assign temporary process so renderer knows what is switching in
                    core->assignProcess(nextP, tickIntervalMs);

                    // Phase 1: Save outgoing context
                    renderer.addEvent(currentTime, "\033[1m\033[36m[CONTEXT SWITCH]\033[0m Core " + std::to_string(core->getId()) + ": Saving outgoing context...");
                    renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses);
                    
                    if (realTimeDelayMs > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(realTimeDelayMs / 2));
                    }
                    
                    // Phase 2: Load incoming context
                    renderer.addEvent(currentTime, "\033[1m\033[36m[CONTEXT SWITCH]\033[0m Core " + std::to_string(core->getId()) + ": Loading context for P" + std::to_string(nextP->pid) + "...");
                    renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses);
                    
                    if (realTimeDelayMs > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(realTimeDelayMs / 2));
                    }

                    // Increment the simulation clock by the context switch overhead
                    currentTime += switchDelay;
                    
                    // Context Switch Complete
                    core->setSwitching(false);
                    renderer.addEvent(currentTime, "Context Switch Complete on Core " + std::to_string(core->getId()));

                    // Set Process Response Time if it is the first time running
                    if (nextP->responseTime < 0) {
                        nextP->responseTime = currentTime - nextP->arrivalTime;
                        if (nextP->responseTime < 0) nextP->responseTime = 0;
                    }

                    // Change state to RUNNING
                    scheduler->recordTransition(currentTime, nextP->pid, ProcessState::READY, ProcessState::RUNNING);
                    nextP->state = ProcessState::RUNNING;

                    // Assign to core
                    core->assignProcess(nextP, tickIntervalMs);
                    previousProcesses[i] = nextP;

                    renderer.addEvent(currentTime, "Core " + std::to_string(core->getId()) + ": Dispatching Process \033[1m\033[32mP" + std::to_string(nextP->pid) + "\033[0m");
                    renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses);
                }
            }
        }

        // Render current tick state
        renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses);

        // Sleep for one tick cycle and advance clock
        timer.sleepForTick();
        currentTime++;

        // Check if all processes are completed
        allCompleted = true;
        for (const auto& p : simProcesses) {
            if (!p.isCompleted()) {
                allCompleted = false;
                break;
            }
        }
    }

    // Stop and join all core worker threads
    for (auto& core : cores) {
        core->stop();
    }

    totalSimulationTime = currentTime - 1; // Last tick was empty after final completion
    scheduler->totalExecutionTime = totalSimulationTime;
    
    // Copy the updated processes back into the scheduler so it can display Gantt/Table/Metrics
    scheduler->processes = simProcesses;

    // Draw one final render with complete status
    renderer.addEvent(totalSimulationTime, ">>> SIMULATION COMPLETE (Total Time: " + std::to_string(totalSimulationTime) + " units) <<<");
    renderer.render(totalSimulationTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses);
    
    std::cout << std::endl;
}
