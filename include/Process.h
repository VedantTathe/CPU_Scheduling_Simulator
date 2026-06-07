#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using namespace std;

// Demonstrating ENCAPSULATION: Data is private, accessed via public getters/setters
class Process {
private:
    string id;
    int arrival_time;
    int burst_time;
    int priority;
    
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int remaining_time; 

public:
    // Constructor
    Process(string i = "", int a = 0, int b = 0, int p = 0) 
        : id(i), arrival_time(a), burst_time(b), priority(p), 
          completion_time(0), turnaround_time(0), waiting_time(0), remaining_time(b) {}

    // Getters
    string getId() const { return id; }
    int getArrivalTime() const { return arrival_time; }
    int getBurstTime() const { return burst_time; }
    int getPriority() const { return priority; }
    int getCompletionTime() const { return completion_time; }
    int getTurnaroundTime() const { return turnaround_time; }
    int getWaitingTime() const { return waiting_time; }
    int getRemainingTime() const { return remaining_time; }

    // Setters
    void setId(string i) { id = i; }
    void setArrivalTime(int a) { arrival_time = a; }
    void setBurstTime(int b) { burst_time = b; remaining_time = b; }
    void setPriority(int p) { priority = p; }
    void setCompletionTime(int c) { completion_time = c; }
    void setTurnaroundTime(int t) { turnaround_time = t; }
    void setWaitingTime(int w) { waiting_time = w; }
    void setRemainingTime(int r) { remaining_time = r; }
};

#endif
