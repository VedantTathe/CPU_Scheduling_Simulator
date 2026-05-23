#include "CPUProcess.h"

Process::Process()
    : pid(0), name("P0"), arrivalTime(0), burstTime(0), priority(0),
      remainingTime(0), completionTime(0), waitingTime(0),
      turnaroundTime(0), responseTime(-1), state(ProcessState::WAITING) {}

Process::Process(int id, int arrival, int burst, int prio)
    : pid(id), name("P" + std::to_string(id)), arrivalTime(arrival), burstTime(burst), priority(prio),
      remainingTime(burst), completionTime(0), waitingTime(0),
      turnaroundTime(0), responseTime(-1), state(ProcessState::WAITING) {}

void Process::reset() {
    remainingTime = burstTime;
    completionTime = 0;
    waitingTime = 0;
    turnaroundTime = 0;
    responseTime = -1;  // -1 indicates not yet started
    state = ProcessState::WAITING;
}

void Process::display() const {
    std::cout << "PID: " << pid 
              << " | Arrival: " << arrivalTime 
              << " | Burst: " << burstTime 
              << " | Priority: " << priority << std::endl;
}

bool Process::isCompleted() const {
    return remainingTime == 0;
}
