#include "CPURuntime.h"
#include "ContextSwitch.h"
#include "Utils.h"
#include "DashboardRenderer.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <sstream>

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
        } else if (algoName.find("Round Robin") != std::string::npos || algoName.find("RR") != std::string::npos) {
            // Round Robin: FIFO queue order (first element in eligible is already the oldest in readyQueue)
            // We just return eligible[0] which is already at selected.
            return selected;
        } else if (algoName.find("EDF") != std::string::npos) {
            // EDF: Earliest absolute deadline first, then arrival time, then lower PID
            if (p->deadline < selected->deadline) {
                selected = p;
            } else if (p->deadline == selected->deadline && p->arrivalTime < selected->arrivalTime) {
                selected = p;
            } else if (p->deadline == selected->deadline && p->arrivalTime == selected->arrivalTime && p->pid < selected->pid) {
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
    
    // Initialize lastSession
    lastSession = RuntimeSession();
    lastSession.algorithmName = scheduler->getAlgorithmName();
    lastSession.numCores = numCores;
    lastSession.switchDelay = switchDelay;
    lastSession.realTimeDelayMs = realTimeDelayMs;

    // 1. Initialize simulation parameters
    int tickIntervalMs = 200; // 200ms per simulation time unit/tick
    int currentTime = 0;
    
    // Clear and prepare cores
    cores.clear();
    previousProcesses.clear();
    std::vector<int> coreQuantumElapsed(numCores, 0);
    std::vector<Process*> readyQueue;

    // Advanced analytics structures
    int totalContextSwitches = 0;
    int totalInterrupts = 0;
    std::vector<int> coreActiveTicks(numCores, 0);
    // Maps process ID to (core ID -> ticks spent executing)
    std::vector<std::vector<int>> processCoreTicks(simProcesses.size() + 100, std::vector<int>(numCores + 1, 0));

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
    
    // Helper to strip ANSI escape codes for file logging
    auto stripAnsiCodes = [](const std::string& str) {
        std::string result;
        bool inEscape = false;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\033') {
                inEscape = true;
            } else if (inEscape) {
                if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')) {
                    inEscape = false;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    };

    // Helper to log events to both live screen and session structure
    auto logEvent = [&](int time, const std::string& eventText) {
        renderer.addEvent(time, eventText);
        std::ostringstream timeOss;
        timeOss << "[Time " << std::setw(3) << std::setfill(' ') << time << "] " << stripAnsiCodes(eventText);
        lastSession.fullEventLog.push_back(timeOss.str());
    };

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
        totalInterrupts++;
        logEvent(currentTime, "\033[1m\033[35m[TIMER INTERRUPT]\033[0m Scheduler Invoked");

        // Process arrived notifications
        for (auto& p : simProcesses) {
            if (p.state == ProcessState::WAITING && p.arrivalTime <= currentTime) {
                p.state = ProcessState::READY;
                scheduler->recordTransition(currentTime, p.pid, ProcessState::WAITING, ProcessState::READY);
                logEvent(currentTime, "Process \033[1m\033[36mP" + std::to_string(p.pid) + "\033[0m Arrived: WAITING -> READY");
                readyQueue.push_back(&p);
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
                    
                    logEvent(currentTime, "Process \033[1m\033[32mP" + std::to_string(completedProcess->pid) + "\033[0m Completed on Core " + std::to_string(core->getId()));
                    scheduler->recordTransition(currentTime, completedProcess->pid, ProcessState::RUNNING, ProcessState::COMPLETED);
                    core->releaseProcess();
                }
                coreQuantumElapsed[i] = 0; // Reset quantum
            }
        }

        // Check for preemption if preemptive scheduler (quantum > 0)
        int quantum = scheduler->getQuantum();
        if (quantum > 0) {
            for (int i = 0; i < numCores; ++i) {
                auto& core = cores[i];
                if (!core->isIdle() && !core->isSwitching()) {
                    Process* p = core->getActiveProcess();
                    if (p != nullptr && p->remainingTime > 0) {
                        coreQuantumElapsed[i]++;
                        if (coreQuantumElapsed[i] >= quantum) {
                            // Preempt process!
                            p->state = ProcessState::READY;
                            scheduler->recordTransition(currentTime, p->pid, ProcessState::RUNNING, ProcessState::READY);
                            logEvent(currentTime, "Process \033[1m\033[33mP" + std::to_string(p->pid) + "\033[0m Preempted (Quantum Expired) on Core " + std::to_string(core->getId()));
                            readyQueue.push_back(p);
                            core->releaseProcess();
                            coreQuantumElapsed[i] = 0;
                        }
                    }
                }
            }
        }

        // Check for preemptive EDF scheduling
        if (scheduler->getAlgorithmName().find("EDF") != std::string::npos) {
            // Sort readyQueue by deadline (earliest first) to find processes requesting CPU
            std::vector<Process*> sortedReady = readyQueue;
            std::sort(sortedReady.begin(), sortedReady.end(), [](const Process* a, const Process* b) {
                if (a->deadline != b->deadline) return a->deadline < b->deadline;
                return a->pid < b->pid;
            });

            for (auto* readyP : sortedReady) {
                // Find a core running a process with a deadline LATER than readyP->deadline
                int targetCoreIdx = -1;
                int latestDeadlineOnCores = -1;

                for (int i = 0; i < numCores; ++i) {
                    auto& core = cores[i];
                    if (!core->isIdle() && !core->isSwitching()) {
                        Process* runningP = core->getActiveProcess();
                        if (runningP != nullptr && runningP->deadline > readyP->deadline) {
                            if (runningP->deadline > latestDeadlineOnCores) {
                                latestDeadlineOnCores = runningP->deadline;
                                targetCoreIdx = i;
                            }
                        }
                    }
                }

                // If a core runs a process with a later deadline, preempt it!
                if (targetCoreIdx != -1) {
                    auto& core = cores[targetCoreIdx];
                    Process* p = core->getActiveProcess();

                    p->state = ProcessState::READY;
                    scheduler->recordTransition(currentTime, p->pid, ProcessState::RUNNING, ProcessState::READY);
                    logEvent(currentTime, "Process \033[1m\033[33mP" + std::to_string(p->pid) + "\033[0m Preempted by earlier deadline P" + std::to_string(readyP->pid) + " on Core " + std::to_string(core->getId()));
                    
                    readyQueue.push_back(p);
                    core->releaseProcess();
                    coreQuantumElapsed[targetCoreIdx] = 0;
                }
            }
        }

        // Schedule / dispatch READY processes onto IDLE cores
        for (int i = 0; i < numCores; ++i) {
            auto& core = cores[i];
            if (core->isIdle() && !core->isSwitching()) {
                Process* nextP = selectNextProcess(readyQueue, scheduler->getAlgorithmName());
                if (nextP != nullptr) {
                    // Remove from readyQueue
                    auto it = std::find(readyQueue.begin(), readyQueue.end(), nextP);
                    if (it != readyQueue.end()) {
                        readyQueue.erase(it);
                    }

                    // Mark core as context switching
                    core->setSwitching(true);
                    totalContextSwitches++;
                    
                    // Assign temporary process so renderer knows what is switching in
                    core->assignProcess(nextP, tickIntervalMs);

                    // Phase 1: Save outgoing context
                    logEvent(currentTime, "\033[1m\033[36m[CONTEXT SWITCH]\033[0m Core " + std::to_string(core->getId()) + ": Saving outgoing context...");
                    renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses, totalContextSwitches, totalInterrupts);
                    
                    if (realTimeDelayMs > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(realTimeDelayMs / 2));
                    }
                    
                    // Phase 2: Load incoming context
                    logEvent(currentTime, "\033[1m\033[36m[CONTEXT SWITCH]\033[0m Core " + std::to_string(core->getId()) + ": Loading context for P" + std::to_string(nextP->pid) + "...");
                    renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses, totalContextSwitches, totalInterrupts);
                    
                    if (realTimeDelayMs > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(realTimeDelayMs / 2));
                    }

                    // Increment the simulation clock by the context switch overhead
                    currentTime += switchDelay;
                    
                    // Context Switch Complete
                    core->setSwitching(false);
                    logEvent(currentTime, "Context Switch Complete on Core " + std::to_string(core->getId()));

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
                    coreQuantumElapsed[i] = 0; // Reset quantum elapsed for the new process

                    logEvent(currentTime, "Core " + std::to_string(core->getId()) + ": Dispatching Process \033[1m\033[32mP" + std::to_string(nextP->pid) + "\033[0m");
                    renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses, totalContextSwitches, totalInterrupts);
                }
            }
        }

        // Render current tick state
        renderer.render(currentTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses, totalContextSwitches, totalInterrupts);

        // Sleep for one tick cycle and advance clock
        timer.sleepForTick();
        
        // Track Core and Process active ticks during this tick cycle
        for (int i = 0; i < numCores; ++i) {
            auto& core = cores[i];
            if (!core->isIdle() && !core->isSwitching()) {
                coreActiveTicks[i]++;
                Process* p = core->getActiveProcess();
                if (p != nullptr && p->pid < static_cast<int>(processCoreTicks.size())) {
                    processCoreTicks[p->pid][core->getId()]++;
                }
            }
        }

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
    logEvent(totalSimulationTime, ">>> SIMULATION COMPLETE (Total Time: " + std::to_string(totalSimulationTime) + " units) <<<");
    renderer.render(totalSimulationTime, scheduler->getAlgorithmName(), numCores, cores, simProcesses, totalContextSwitches, totalInterrupts);
    
    // Render the beautiful visual statistics analytics summary box
    std::cout << "\n\033[1m\033[36m╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                         ADVANCED RUNTIME EXECUTION ANALYTICS                        ║\n";
    std::cout << "╠═════════════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ " << std::left << std::setw(30) << "Total Timer Interrupts:" << std::left << std::setw(52) << totalInterrupts << "║\n";
    std::cout << "║ " << std::left << std::setw(30) << "Total Context Switches:" << std::left << std::setw(52) << totalContextSwitches << "║\n";
    std::cout << "║ " << std::left << std::setw(30) << "Average Scheduler Overhead:" << std::left << std::setw(52) << (std::to_string(totalContextSwitches * switchDelay) + " time units") << "║\n";
    std::cout << "╠═════════════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ CORE UTILIZATION STATISTICS                                                         ║\n";
    
    for (int i = 0; i < numCores; ++i) {
        double util = 0.0;
        if (totalSimulationTime > 0) {
            util = (double)coreActiveTicks[i] / totalSimulationTime * 100.0;
            if (util > 100.0) util = 100.0;
        }
        int filled = (int)(util / 5); // 20 segments
        std::ostringstream barStream;
        barStream << "Core " << (i + 1) << ": [";
        for (int k = 0; k < 20; ++k) {
            if (k < filled) barStream << "█";
            else barStream << "░";
        }
        barStream << "] " << std::fixed << std::setprecision(1) << util << "%";
        std::cout << "║  " << std::left << std::setw(81) << barStream.str() << "║\n";
    }
    
    std::cout << "╠═════════════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ PROCESS CORE DISPATCH HISTORY (Ticks spent per Core)                                ║\n";
    
    for (const auto& p : simProcesses) {
        std::ostringstream histStream;
        histStream << "Process P" << p.pid << " (" << p.name << ") : ";
        bool firstCore = true;
        for (int c = 1; c <= numCores; ++c) {
            int ticks = 0;
            if (p.pid < static_cast<int>(processCoreTicks.size())) {
                ticks = processCoreTicks[p.pid][c];
            }
            if (ticks > 0) {
                if (!firstCore) histStream << ", ";
                histStream << "Core " << c << " (" << ticks << " ticks)";
                firstCore = false;
            }
        }
        if (firstCore) {
            histStream << "No execution ticks logged.";
        }
        std::cout << "║  " << std::left << std::setw(81) << histStream.str() << "║\n";
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n";
    std::cout << std::endl;

    // Populate lastSession remaining metrics
    lastSession.totalSimulationTime = totalSimulationTime;
    lastSession.totalContextSwitches = totalContextSwitches;
    lastSession.totalInterrupts = totalInterrupts;

    for (int i = 0; i < numCores; ++i) {
        double util = 0.0;
        if (totalSimulationTime > 0) {
            util = (double)coreActiveTicks[i] / totalSimulationTime * 100.0;
            if (util > 100.0) util = 100.0;
        }
        lastSession.coreUtilizations.push_back(util);
    }

    for (const auto& p : simProcesses) {
        std::ostringstream histStream;
        histStream << "Process P" << p.pid << " (" << p.name << ") : ";
        bool firstCore = true;
        for (int c = 1; c <= numCores; ++c) {
            int ticks = 0;
            if (p.pid < static_cast<int>(processCoreTicks.size())) {
                ticks = processCoreTicks[p.pid][c];
            }
            if (ticks > 0) {
                if (!firstCore) histStream << ", ";
                histStream << "Core " << c << " (" << ticks << " ticks)";
                firstCore = false;
            }
        }
        if (firstCore) {
            histStream << "No execution ticks logged.";
        }
        lastSession.processDispatchHistory.push_back(histStream.str());
    }
}
