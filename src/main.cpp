#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib> // For system()
#include "Process.h"
#include "Scheduler.h"
#include "FCFS.h"
#include "SJF.h"
#include "PriorityScheduler.h"
#include "RoundRobin.h"
#include "SRTF.h"

using namespace std;

// Helper function to clear the screen (Cross-platform)
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Helper function to pause before clearing the screen
void pauseScreen() {
    cout << "\nPress Enter to return to the main menu...";
    cin.ignore(); // clear previous newline
    if (cin.peek() == '\n') {
        cin.get();
    } else {
        string dummy;
        getline(cin, dummy);
    }
}

vector<Process> loadFromFile(string filename) {
    vector<Process> processes;
    ifstream file(filename);
    if (!file) {
        cout << "Could not open file: " << filename << "\n";
        return processes;
    }
    int n;
    file >> n;
    for (int i = 0; i < n; i++) {
        string id;
        int at, bt, prio;
        file >> id >> at >> bt >> prio;
        
        Process p(id, at, bt, prio);
        processes.push_back(p);
    }
    file.close();
    return processes;
}

void printProcesses(const vector<Process>& processes) {
    cout << "\n=== Loaded Processes ===\n";
    if (processes.empty()) {
        cout << "No processes loaded.\n";
        return;
    }
    cout << "ID\tArrival\tBurst\tPriority\n";
    cout << "---------------------------------\n";
    for (const auto& p : processes) {
        cout << p.getId() << "\t" 
             << p.getArrivalTime() << "\t" 
             << p.getBurstTime() << "\t" 
             << p.getPriority() << "\n";
    }
    cout << "---------------------------------\n";
}


int main() {
    clearScreen();
    vector<Process> processes;
    int choice;
    
    cout << "=== CPU Scheduling Simulator (Modular OOP) ===\n";
    cout << "1. Load processes from input.txt\n";
    cout << "2. Enter processes manually\n";
    cout << "Select option: ";
    cin >> choice;
    
    if (cin.fail()) {
        cin.clear();
        string dummy;
        getline(cin, dummy);
        cout << "Invalid input! Defaulting to Load from input.txt\n";
        choice = 1;
    }
    if (choice == 1) {
        processes = loadFromFile("input.txt");
        if (processes.empty()) return 1;
        cout << "Successfully loaded " << processes.size() << " processes from input.txt\n";
    } else {
        int n;
        cout << "Enter number of processes: ";
        cin >> n;
        for (int i = 0; i < n; i++) {
            string id;
            int at, bt, prio;
            
            cout << "\nProcess " << i + 1 << " ID: ";
            cin >> id;
            cout << "Arrival Time: ";
            cin >> at;
            cout << "Burst Time: ";
            cin >> bt;
            cout << "Priority (lower number = higher priority): ";
            cin >> prio;
            
            Process p(id, at, bt, prio);
            processes.push_back(p);
        }
    }
    
    pauseScreen();
    
    int cs_time = 0;
    clearScreen();
    cout << "Enter Context Switch Overhead Time (default 0): ";
    cin >> cs_time;
    if (cin.fail()) {
        cin.clear();
        string dummy;
        getline(cin, dummy);
        cs_time = 0;
    }

    while (true) {
        clearScreen();
        cout << "\n=== Main Menu ===\n";
        cout << "1. View Loaded Processes\n";
        cout << "2. FCFS\n";
        cout << "3. SJF (Non-Preemptive)\n";
        cout << "4. Priority Scheduling\n";
        cout << "5. Round Robin\n";
        cout << "6. SRTF (Preemptive SJF)\n";
        cout << "7. Exit\n";
        cout << "Select Algorithm: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            string dummy;
            getline(cin, dummy);
            cout << "Invalid input! Please enter a number.\n";
            pauseScreen();
            continue;
        }
        
        // Demonstrating POLYMORPHISM: Base class pointer holds derived class object
        Scheduler* scheduler = nullptr;
        
        if (choice == 1) {
            clearScreen();
            printProcesses(processes);
            pauseScreen();
            continue;
        } else if (choice == 2) {
            scheduler = new FCFS(processes, cs_time);
        } else if (choice == 3) {
            scheduler = new SJF(processes, cs_time);
        } else if (choice == 4) {
            scheduler = new PriorityScheduler(processes, cs_time);
        } else if (choice == 5) {
            int quantum;
            cout << "Enter Time Quantum: ";
            cin >> quantum;
            scheduler = new RoundRobin(processes, quantum, cs_time);
        } else if (choice == 6) {
            scheduler = new SRTF(processes, cs_time);
        } else if (choice == 7) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice! Try again.\n";
            pauseScreen();
            continue;
        }

        if (scheduler) {
            clearScreen(); 
            // Polymorphism in action! Calls the overridden run() method dynamically
            scheduler->run();
            
            // Abstraction in action! We don't care how these are printed, just that they are.
            scheduler->printGanttChart();
            scheduler->printMetrics();
            
            delete scheduler; // Prevent memory leak
            
            pauseScreen(); 
        }
    }
    
    return 0;
}
