#include "FileHandler.h"
#include <iostream>
#include <algorithm>

// Initialize static error storage
std::string FileHandler::lastError = "";

bool FileHandler::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string FileHandler::getLastError() {
    return lastError;
}

std::vector<Process> FileHandler::loadProcessesFromFile(const std::string& filename) {
    std::vector<Process> processes;
    lastError = "";

    // Check if file exists
    std::ifstream file(filename);
    if (!file.is_open()) {
        lastError = "Error: Cannot open file '" + filename + "'. File not found or permission denied.";
        throw std::runtime_error(lastError);
    }

    std::string line;
    int processCount = 0;
    int lineNumber = 0;

    // Read number of processes (skip comments and empty lines)
    bool foundCount = false;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip leading whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        iss >> processCount;
        if (iss.fail() || processCount <= 0) {
            lastError = "Error on line " + std::to_string(lineNumber) + 
                       ": First non-comment line must be a positive integer (process count).";
            throw std::runtime_error(lastError);
        }
        foundCount = true;
        break;
    }

    if (!foundCount) {
        lastError = "Error: File is empty or contains only comments. Expected process count.";
        throw std::runtime_error(lastError);
    }

    // Read process definitions
    int processesRead = 0;
    while (std::getline(file, line) && processesRead < processCount) {
        lineNumber++;

        // Skip empty lines and comments
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty() || line[0] == '#') {
            continue;
        }

        Process process;
        if (!parseProcessLine(line, lineNumber, process)) {
            file.close();
            throw std::runtime_error(lastError);
        }

        processes.push_back(process);
        processesRead++;
    }

    file.close();

    // Verify we read the expected number of processes
    if (processesRead != processCount) {
        lastError = "Error: Expected " + std::to_string(processCount) + 
                   " processes, but only found " + std::to_string(processesRead) + ".";
        throw std::runtime_error(lastError);
    }

    // Validate all processes
    if (!validateProcesses(processes)) {
        throw std::runtime_error(lastError);
    }

    return processes;
}

bool FileHandler::parseProcessLine(const std::string& line, int lineNumber, Process& process) {
    std::istringstream iss(line);
    int pid, arrival, burst, priority;

    // Attempt to parse all four required fields
    iss >> pid >> arrival >> burst >> priority;

    // Check if parsing succeeded
    if (iss.fail()) {
        lastError = "Error on line " + std::to_string(lineNumber) + 
                   ": Expected format: PID Arrival Burst Priority (integers).\n" +
                   "Got: " + line;
        return false;
    }

    // Check for extra tokens on the line
    std::string extra;
    if (iss >> extra) {
        lastError = "Error on line " + std::to_string(lineNumber) + 
                   ": Too many fields. Expected exactly 4 fields (PID Arrival Burst Priority).";
        return false;
    }

    // Validate individual values
    if (pid <= 0) {
        lastError = "Error on line " + std::to_string(lineNumber) + 
                   ": Process ID must be positive, got " + std::to_string(pid) + ".";
        return false;
    }

    if (arrival < 0) {
        lastError = "Error on line " + std::to_string(lineNumber) + 
                   ": Arrival time cannot be negative, got " + std::to_string(arrival) + ".";
        return false;
    }

    if (burst <= 0) {
        lastError = "Error on line " + std::to_string(lineNumber) + 
                   ": Burst time must be positive, got " + std::to_string(burst) + ".";
        return false;
    }

    if (priority <= 0) {
        lastError = "Error on line " + std::to_string(lineNumber) + 
                   ": Priority must be positive, got " + std::to_string(priority) + ".";
        return false;
    }

    // Create and return the process
    process = Process(pid, arrival, burst, priority);
    return true;
}

bool FileHandler::validateProcesses(const std::vector<Process>& processes) {
    if (processes.empty()) {
        lastError = "Error: No valid processes loaded from file.";
        return false;
    }

    // Check for duplicate PIDs
    if (!checkForDuplicatePIDs(processes)) {
        return false;
    }

    return true;
}

bool FileHandler::checkForDuplicatePIDs(const std::vector<Process>& processes) {
    for (size_t i = 0; i < processes.size(); ++i) {
        for (size_t j = i + 1; j < processes.size(); ++j) {
            if (processes[i].pid == processes[j].pid) {
                lastError = "Error: Duplicate Process ID found: " + std::to_string(processes[i].pid) + 
                           ". Each process must have a unique ID.";
                return false;
            }
        }
    }
    return true;
}
