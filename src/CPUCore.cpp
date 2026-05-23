#include "CPUCore.h"
#include <chrono>

CPUCore::CPUCore(int id)
    : coreId(id),
      running(false),
      activeProcess(nullptr),
      tickIntervalMs(200),
      processJustFinished(false),
      contextSwitching(false) {}

CPUCore::~CPUCore() {
    stop();
}

bool CPUCore::isIdle() const {
    std::lock_guard<std::mutex> lock(coreMutex);
    return activeProcess == nullptr;
}

bool CPUCore::assignProcess(Process* process, int tickInterval) {
    std::lock_guard<std::mutex> lock(coreMutex);
    if (activeProcess != nullptr) {
        return false; // Core is currently busy
    }
    activeProcess = process;
    tickIntervalMs.store(tickInterval);
    processJustFinished.store(false);
    return true;
}

Process* CPUCore::getActiveProcess() {
    std::lock_guard<std::mutex> lock(coreMutex);
    return activeProcess;
}

void CPUCore::releaseProcess() {
    std::lock_guard<std::mutex> lock(coreMutex);
    activeProcess = nullptr;
    processJustFinished.store(false);
}

bool CPUCore::checkAndClearFinished() {
    return processJustFinished.exchange(false);
}

void CPUCore::start() {
    if (running) return;
    running = true;
    workerThread = std::thread(&CPUCore::coreLoop, this);
}

void CPUCore::stop() {
    if (!running) return;
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void CPUCore::coreLoop() {
    while (running) {
        // Sleep representing 1 tick of simulated CPU clock cycle
        std::this_thread::sleep_for(std::chrono::milliseconds(tickIntervalMs.load()));
        
        if (!running) break;

        std::lock_guard<std::mutex> lock(coreMutex);
        if (activeProcess != nullptr && !activeProcess->isCompleted()) {
            // Execute process burst for 1 tick cycle
            if (activeProcess->remainingTime > 0) {
                activeProcess->remainingTime--;
            }
            
            // Check if process has finished executing
            if (activeProcess->remainingTime <= 0) {
                activeProcess->state = ProcessState::COMPLETED;
                processJustFinished.store(true);
            }
        }
    }
}

bool CPUCore::isSwitching() const {
    return contextSwitching.load();
}

void CPUCore::setSwitching(bool switching) {
    contextSwitching.store(switching);
}
