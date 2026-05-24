#include "ContextSwitch.h"
#include "Utils.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

void ContextSwitch::performSwitch(int currentTime, const Process* outgoing, const Process* incoming, int realTimeDelayMs) {
    // Print context switch block header
    std::cout << ConsoleColor::BOLD << ConsoleColor::CYAN 
              << "[Time " << std::setw(3) << currentTime << "] [CONTEXT SWITCH]" 
              << ConsoleColor::RESET << std::endl;

    // 1. Save outgoing context
    std::cout << "  |- Saving Context: ";
    if (outgoing != nullptr) {
        std::cout << ConsoleColor::YELLOW << "Process P" << outgoing->pid << ConsoleColor::RESET << "..." << std::endl;
    } else {
        std::cout << ConsoleColor::YELLOW << "[None - CPU was Idle]" << ConsoleColor::RESET << std::endl;
    }

    // Optional delay for saving context
    if (realTimeDelayMs > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(realTimeDelayMs / 2));
    }

    // 2. Load/restore incoming context
    std::cout << "  |- Loading Context: ";
    if (incoming != nullptr) {
        std::cout << ConsoleColor::GREEN << "Process P" << incoming->pid << ConsoleColor::RESET << "..." << std::endl;
    } else {
        std::cout << ConsoleColor::GREEN << "[None - CPU Idle]" << ConsoleColor::RESET << std::endl;
    }

    // Optional delay for loading context
    if (realTimeDelayMs > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(realTimeDelayMs / 2));
    }

    // 3. Complete context switch
    std::cout << "  +- " << ConsoleColor::BOLD << ConsoleColor::CYAN 
              << "Context Switch Complete" << ConsoleColor::RESET << std::endl;
}
