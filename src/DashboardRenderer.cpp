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
                               const std::vector<Process>& processes,
                               int totalContextSwitches, int totalInterrupts) {
    clearScreen();

    // 1. Render Minimalist Header
    std::cout << DBColor::BOLD << DBColor::CYAN << "================================================================================\n" << DBColor::RESET;
    std::cout << DBColor::BOLD << DBColor::WHITE << " CPU SCHEDULING SYSTEM MONITOR\n" << DBColor::RESET;
    std::cout << DBColor::BOLD << DBColor::CYAN << "================================================================================\n" << DBColor::RESET;
    
    int activeCoreCount = 0;
    for (int i = 0; i < numCores; ++i) {
        if (!cores[i]->isIdle() || cores[i]->isSwitching()) {
            activeCoreCount++;
        }
    }

    std::cout << DBColor::BOLD << DBColor::WHITE << "  Time: " << DBColor::RESET << std::left << std::setw(12) << (std::to_string(currentTime) + " units")
              << DBColor::BOLD << DBColor::WHITE << "  Algorithm: " << DBColor::RESET << std::left << std::setw(30) << algoName
              << DBColor::BOLD << DBColor::WHITE << "  Cores Active: " << DBColor::RESET << activeCoreCount << " / " << numCores << "\n";
              
    std::cout << DBColor::BOLD << DBColor::WHITE << "  Telemetry: " << DBColor::RESET 
              << "Switches: " << std::left << std::setw(12) << totalContextSwitches
              << "Interrupts: " << std::left << std::setw(18) << totalInterrupts
              << "Processes: " << processes.size() << " total\n";
              
    std::cout << DBColor::GRAY << "--------------------------------------------------------------------------------\n" << DBColor::RESET;

    // 2. Render CPU Cores (Compact System View)
    std::cout << DBColor::BOLD << DBColor::CYAN << "  CPU CORE STATES\n" << DBColor::RESET;
    for (int i = 0; i < numCores; ++i) {
        const auto& core = cores[i];
        std::cout << "  Core " << core->getId() << " -> ";
        if (core->isSwitching()) {
            Process* p = core->getActiveProcess();
            std::cout << DBColor::YELLOW << "[SWITCHING]";
            if (p != nullptr) {
                std::cout << " (Loading P" << p->pid << " - " << p->name << ")";
            }
            std::cout << DBColor::RESET << "\n";
        } else if (core->isIdle() || core->getActiveProcess() == nullptr) {
            std::cout << DBColor::GRAY << "[IDLE]" << DBColor::RESET << "\n";
        } else {
            Process* p = core->getActiveProcess();
            std::cout << DBColor::GREEN << "[RUNNING]" << DBColor::RESET << " " 
                      << DBColor::BOLD << "P" << p->pid << " (" << p->name << ")" << DBColor::RESET
                      << " (Remaining: " << DBColor::CYAN << p->remainingTime << DBColor::RESET << "/" << p->burstTime 
                      << ", Priority: " << p->priority << ")\n";
        }
    }
    std::cout << DBColor::GRAY << "--------------------------------------------------------------------------------\n" << DBColor::RESET;

    // 3. Render Process States
    std::cout << DBColor::BOLD << DBColor::CYAN << "  PROCESS SCHEDULER STATES\n" << DBColor::RESET;
    
    // Running Row
    std::cout << "  " << DBColor::BOLD << DBColor::GREEN << "[RUNNING]" << DBColor::RESET << "   ";
    bool firstRunning = true;
    for (int i = 0; i < numCores; ++i) {
        const auto& core = cores[i];
        if (!core->isIdle() && core->getActiveProcess() != nullptr && !core->isSwitching()) {
            if (!firstRunning) std::cout << ", ";
            std::cout << "P" << core->getActiveProcess()->pid << " (Core " << core->getId() << ")";
            firstRunning = false;
        }
    }
    if (firstRunning) std::cout << DBColor::GRAY << "None (Idle)" << DBColor::RESET;
    std::cout << "\n";
    
    // Ready Row
    std::cout << "  " << DBColor::BOLD << DBColor::CYAN << "[READY]" << DBColor::RESET << "     ";
    bool firstReady = true;
    for (const auto& p : processes) {
        if (p.state == ProcessState::READY) {
            if (!firstReady) std::cout << ", ";
            std::cout << "P" << p.pid << " (Rem: " << p.remainingTime << ", Prio: " << p.priority << ")";
            firstReady = false;
        }
    }
    if (firstReady) std::cout << DBColor::GRAY << "None" << DBColor::RESET;
    std::cout << "\n";
    
    // Waiting Row
    std::cout << "  " << DBColor::BOLD << DBColor::YELLOW << "[WAITING]" << DBColor::RESET << "   ";
    bool firstWaiting = true;
    for (const auto& p : processes) {
        if (p.state == ProcessState::WAITING) {
            if (!firstWaiting) std::cout << ", ";
            std::cout << "P" << p.pid << " (Arrives: " << p.arrivalTime << ", Burst: " << p.burstTime << ")";
            firstWaiting = false;
        }
    }
    if (firstWaiting) std::cout << DBColor::GRAY << "None" << DBColor::RESET;
    std::cout << "\n";
    
    // Completed Row
    std::cout << "  " << DBColor::BOLD << DBColor::MAGENTA << "[COMPLETED]" << DBColor::RESET << " ";
    bool firstCompleted = true;
    for (const auto& p : processes) {
        if (p.state == ProcessState::COMPLETED) {
            if (!firstCompleted) std::cout << ", ";
            std::cout << "P" << p.pid << " (" << p.name << ")";
            firstCompleted = false;
        }
    }
    if (firstCompleted) std::cout << DBColor::GRAY << "None" << DBColor::RESET;
    std::cout << "\n";
    
    std::cout << DBColor::GRAY << "--------------------------------------------------------------------------------\n" << DBColor::RESET;

    // 4. Render Event Logs (Concise & Legible)
    std::cout << DBColor::BOLD << DBColor::CYAN << "  SYSTEM EVENT LOG\n" << DBColor::RESET;
    {
        std::lock_guard<std::mutex> lock(logMutex);
        for (size_t i = 0; i < maxEvents; ++i) {
            std::cout << "  ";
            if (i < eventLog.size()) {
                std::cout << eventLog[i];
            } else {
                std::cout << DBColor::GRAY << "---" << DBColor::RESET;
            }
            std::cout << "\n";
        }
    }
    std::cout << DBColor::BOLD << DBColor::CYAN << "================================================================================\n" << DBColor::RESET;
    std::cout << std::flush;
}
