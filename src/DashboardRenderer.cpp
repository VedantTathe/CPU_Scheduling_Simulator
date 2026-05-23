#include "DashboardRenderer.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

// ANSI color configurations
namespace DBColor {
    constexpr const char* RESET       = "\033[0m";
    constexpr const char* BOLD        = "\033[1m";
    constexpr const char* GREEN       = "\033[32m";
    constexpr const char* YELLOW      = "\033[33m";
    constexpr const char* RED         = "\033[31m";
    constexpr const char* BLUE        = "\033[34m";
    constexpr const char* MAGENTA     = "\033[35m";
    constexpr const char* CYAN        = "\033[36m";
    constexpr const char* WHITE       = "\033[37m";
    constexpr const char* GRAY        = "\033[90m";
}

DashboardRenderer::DashboardRenderer() {}

void DashboardRenderer::initialize() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void DashboardRenderer::addEvent(int time, const std::string& eventText) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ostringstream oss;
    oss << DBColor::GRAY << "[Time " << std::setw(3) << std::setfill(' ') << time << "] " << DBColor::RESET << eventText;
    eventLog.push_back(oss.str());
    if (eventLog.size() > maxEvents) {
        eventLog.erase(eventLog.begin());
    }
}

void DashboardRenderer::clearScreen() const {
    // Moves cursor to top-left home position
    std::cout << "\033[H" << std::flush;
}

void DashboardRenderer::render(int currentTime, const std::string& algoName, int numCores,
                               const std::vector<std::unique_ptr<CPUCore>>& cores,
                               const std::vector<Process>& processes) {
    clearScreen();

    // 1. Render Header
    std::cout << DBColor::CYAN << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n" << DBColor::RESET;
    std::cout << DBColor::CYAN << "║" << DBColor::BOLD << DBColor::WHITE << "                            OS CPU RUNTIME DASHBOARD                                 " << DBColor::CYAN << "║\n" << DBColor::RESET;
    std::cout << DBColor::CYAN << "╠═════════════════════════════════════════════════════════════════════════════════════╣\n" << DBColor::RESET;
    
    std::cout << DBColor::CYAN << "║ " 
              << DBColor::BOLD << DBColor::WHITE << "Simulation Time:" << DBColor::RESET << " " << std::left << std::setw(15) << (std::to_string(currentTime) + " units")
              << DBColor::BOLD << DBColor::WHITE << "Algorithm:" << DBColor::RESET << " " << std::left << std::setw(42) << algoName 
              << DBColor::CYAN << "║\n";

    int activeCoreCount = 0;
    for (int i = 0; i < numCores; ++i) {
        if (!cores[i]->isIdle() || cores[i]->isSwitching()) {
            activeCoreCount++;
        }
    }
    
    std::ostringstream coreText, procText;
    coreText << activeCoreCount << " / " << numCores;
    procText << processes.size() << " total";

    std::cout << DBColor::CYAN << "║ " 
              << DBColor::BOLD << DBColor::WHITE << "Active CPU Cores:" << DBColor::RESET << " " << std::left << std::setw(14) << coreText.str()
              << DBColor::BOLD << DBColor::WHITE << "Processes:" << DBColor::RESET << " " << std::left << std::setw(42) << procText.str()
              << DBColor::CYAN << "║\n";
              
    std::cout << DBColor::CYAN << "╚═════════════════════════════════════════════════════════════════════════════════════╝\n" << DBColor::RESET;

    // 2. Render CPU Cores
    std::cout << DBColor::BOLD << DBColor::WHITE << "┌── CPU CORES ────────────────────────────────────────────────────────────────────────┐\n" << DBColor::RESET;
    for (int i = 0; i < numCores; ++i) {
        const auto& core = cores[i];
        std::cout << "│  " << DBColor::BOLD << "Core " << core->getId() << ":" << DBColor::RESET << " ";
        if (core->isSwitching()) {
            // Context Switching State
            std::cout << DBColor::RED << "[▒░▒░▒░▒░▒░▒░▒░▒░▒░▒░  --%]" << DBColor::RESET << " ";
            std::cout << DBColor::BOLD << DBColor::YELLOW << "SWITCHING" << DBColor::RESET;
            Process* p = core->getActiveProcess();
            if (p != nullptr) {
                std::cout << " -> Loading P" << p->pid;
            } else {
                std::cout << " -> Releasing Core";
            }
            std::cout << "\n│          " << DBColor::GRAY << "(Context Save/Restore in progress...)" << DBColor::RESET << "\n";
        } else if (core->isIdle() || core->getActiveProcess() == nullptr) {
            // Idle State
            std::cout << DBColor::GRAY << "[░░░░░░░░░░░░░░░░░░░░   0%]" << DBColor::RESET << " ";
            std::cout << DBColor::BOLD << DBColor::YELLOW << "IDLE" << DBColor::RESET << "\n│\n";
        } else {
            // Running State
            Process* p = core->getActiveProcess();
            double progress = 1.0;
            if (p->burstTime > 0) {
                progress = (double)(p->burstTime - p->remainingTime) / p->burstTime;
                if (progress < 0.0) progress = 0.0;
                if (progress > 1.0) progress = 1.0;
            }
            int percent = (int)(progress * 100);
            int filled = (int)(progress * 20);
            
            std::cout << DBColor::GREEN << "[";
            for (int k = 0; k < 20; ++k) {
                if (k < filled) std::cout << "█";
                else std::cout << "░";
            }
            std::cout << " " << std::setw(3) << percent << "%]" << DBColor::RESET << " ";
            std::cout << DBColor::BOLD << DBColor::GREEN << "RUNNING" << DBColor::RESET << " -> " 
                      << DBColor::BOLD << "Process P" << p->pid << " (" << p->name << ")" << DBColor::RESET 
                      << " (Burst Left: " << DBColor::CYAN << p->remainingTime << DBColor::RESET << " / " << p->burstTime << ")\n";
            std::cout << "│          " << DBColor::GRAY << "(Priority: " << p->priority << ", Arrived: " << p->arrivalTime << ")" << DBColor::RESET << "\n";
        }
    }
    std::cout << DBColor::BOLD << DBColor::WHITE << "└─────────────────────────────────────────────────────────────────────────────────────┘\n" << DBColor::RESET;

    // 3. Render Process States
    std::cout << DBColor::BOLD << DBColor::WHITE << "┌── PROCESS STATES ───────────────────────────────────────────────────────────────────┐\n" << DBColor::RESET;
    
    // Running Row
    std::cout << "│  " << DBColor::BOLD << DBColor::GREEN << "RUNNING" << DBColor::RESET << "   : ";
    bool firstRunning = true;
    for (int i = 0; i < numCores; ++i) {
        const auto& core = cores[i];
        if (!core->isIdle() && core->getActiveProcess() != nullptr && !core->isSwitching()) {
            if (!firstRunning) std::cout << ", ";
            std::cout << DBColor::GREEN << "P" << core->getActiveProcess()->pid << DBColor::RESET << " (Core " << core->getId() << ")";
            firstRunning = false;
        }
    }
    if (firstRunning) std::cout << DBColor::GRAY << "None (CPU Idle)" << DBColor::RESET;
    std::cout << "\n";
    
    // Ready Row
    std::cout << "│  " << DBColor::BOLD << DBColor::CYAN << "READY" << DBColor::RESET << "     : ";
    bool firstReady = true;
    for (const auto& p : processes) {
        if (p.state == ProcessState::READY) {
            if (!firstReady) std::cout << ", ";
            std::cout << DBColor::CYAN << "P" << p.pid << DBColor::RESET << " (" << p.name << ", Rem: " << p.remainingTime << ", Prio: " << p.priority << ")";
            firstReady = false;
        }
    }
    if (firstReady) std::cout << DBColor::GRAY << "None" << DBColor::RESET;
    std::cout << "\n";
    
    // Waiting Row (Not Arrived Yet)
    std::cout << "│  " << DBColor::BOLD << DBColor::YELLOW << "WAITING" << DBColor::RESET << "   : ";
    bool firstWaiting = true;
    for (const auto& p : processes) {
        if (p.state == ProcessState::WAITING) {
            if (!firstWaiting) std::cout << ", ";
            std::cout << DBColor::YELLOW << "P" << p.pid << DBColor::RESET << " (" << p.name << ", Arrives: " << p.arrivalTime << ", Burst: " << p.burstTime << ")";
            firstWaiting = false;
        }
    }
    if (firstWaiting) std::cout << DBColor::GRAY << "None" << DBColor::RESET;
    std::cout << "\n";
    
    // Completed Row
    std::cout << "│  " << DBColor::BOLD << DBColor::MAGENTA << "COMPLETED" << DBColor::RESET << " : ";
    bool firstCompleted = true;
    for (const auto& p : processes) {
        if (p.state == ProcessState::COMPLETED) {
            if (!firstCompleted) std::cout << ", ";
            std::cout << DBColor::MAGENTA << "P" << p.pid << DBColor::RESET << " (" << p.name << ")";
            firstCompleted = false;
        }
    }
    if (firstCompleted) std::cout << DBColor::GRAY << "None" << DBColor::RESET;
    std::cout << "\n";
    
    std::cout << DBColor::BOLD << DBColor::WHITE << "└─────────────────────────────────────────────────────────────────────────────────────┘\n" << DBColor::RESET;

    // 4. Render Event Logs
    std::cout << DBColor::BOLD << DBColor::WHITE << "┌── SYSTEM LOG / INTERRUPTS ──────────────────────────────────────────────────────────┐\n" << DBColor::RESET;
    {
        std::lock_guard<std::mutex> lock(logMutex);
        for (size_t i = 0; i < maxEvents; ++i) {
            std::cout << "│  ";
            if (i < eventLog.size()) {
                std::cout << eventLog[i];
            } else {
                std::cout << DBColor::GRAY << "---" << DBColor::RESET;
            }
            std::cout << "\n";
        }
    }
    std::cout << DBColor::BOLD << DBColor::WHITE << "└─────────────────────────────────────────────────────────────────────────────────────┘\n" << DBColor::RESET;
    std::cout << std::flush;
}
