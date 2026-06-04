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

int main() {
    clearScreen();
    vector<Process> processes;
    int choice;
    
    cout << "=== CPU Scheduling Simulator (Modular OOP) ===\n";
    cout << "1. Load processes from input.txt\n";
    cout << "2. Enter processes manually\n";
    cout << "Select option: ";
    cin >> choice;
    
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

    while (true) {
        clearScreen();
        cout << "\n=== Main Menu ===\n";
        cout << "1. FCFS\n";
        cout << "2. SJF (Non-Preemptive)\n";
        cout << "3. Priority Scheduling\n";
        cout << "4. Round Robin\n";
        cout << "5. Exit\n";
        cout << "Select Algorithm: ";
        cin >> choice;
        
        // Demonstrating POLYMORPHISM: Base class pointer holds derived class object
        Scheduler* scheduler = nullptr;
        
        if (choice == 1) {
            scheduler = new FCFS(processes);
        } else if (choice == 2) {
            scheduler = new SJF(processes);
        } else if (choice == 3) {
            scheduler = new PriorityScheduler(processes);
        } else if (choice == 4) {
            int quantum;
            cout << "Enter Time Quantum: ";
            cin >> quantum;
            scheduler = new RoundRobin(processes, quantum);
        } else if (choice == 5) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice! Try again.\n";
            pauseScreen();
            continue;
        }

        if (scheduler) {
            clearScreen(); // Clear screen to show fresh results
            // Polymorphism in action! Calls the overridden run() method dynamically
            scheduler->run();
            
            // Abstraction in action! We don't care how these are printed, just that they are.
            scheduler->printGanttChart();
            scheduler->printMetrics();
            
            delete scheduler; // Prevent memory leak
            
            pauseScreen(); // Let user read the results before looping back to the menu
        }
    }
    
    return 0;
}
