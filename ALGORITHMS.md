# CPU Scheduling Algorithms - Logic Cheat Sheet

This document is a quick, high-level summary of exactly how the core C++ logic works for each algorithm in this project. Use this for a quick review before your interview!

---

### 1. First Come First Serve (FCFS)
**Core Concept:** The process that arrives first executes first, regardless of burst time.
**C++ Implementation:**
1. **Sort** the `vector<Process>` by `arrival_time`.
2. Iterate through the array using a basic `for` loop.
3. If `current_time` is less than the next arrival time, advance `current_time` (CPU is IDLE).
4. Add the process's `burst_time` to `current_time`.
5. Calculate metrics: `Completion Time = current_time`, `Turnaround = Completion - Arrival`, `Waiting = Turnaround - Burst`.

---

### 2. Shortest Job First (SJF) - Non-Preemptive
**Core Concept:** At any given second, look at all available processes and execute the one with the smallest burst time.
**C++ Implementation:**
1. Use a `while(completed != total_processes)` loop.
2. Inside, use a `for` loop to scan **all** processes to find the one that:
   - Has already arrived (`arrival_time <= current_time`).
   - Is not finished yet (`is_completed == false`).
   - Has the absolute smallest `burst_time`.
3. Execute that specific process entirely (add its `burst_time` to `current_time`).
4. Mark it as completed (`completed++`) and calculate its metrics.
5. If no process was found in the loop, increment `current_time` by 1 (CPU is IDLE).

---

### 3. Priority Scheduling - Non-Preemptive
**Core Concept:** Exactly the same logic as SJF, but we look for the smallest Priority number instead of Burst Time.
**C++ Implementation:**
1. Use a `while(completed != total_processes)` loop.
2. Scan to find the available, uncompleted process with the **smallest `priority` value**.
3. If there is a tie, pick the one that arrived first.
4. Execute it entirely, mark as completed, and update `current_time` and metrics.

---

### 4. Round Robin (RR) - Preemptive
**Core Concept:** Processes take turns executing for a fixed "Time Quantum". If they don't finish, they go to the back of the line.
**C++ Implementation:**
1. **Sort** the processes by `arrival_time`.
2. Push the first arrived process into a `std::queue<int>`.
3. Use a `while(completed != total_processes)` loop.
4. Pop the front process from the queue.
5. Execute it for either the `quantum` limit OR its `remaining_time` (whichever is smaller).
6. **CRITICAL STEP:** Before putting the paused process back in the queue, scan for any *brand new* processes that arrived during the execution window and push them into the queue first!
7. If the current process has `remaining_time > 0`, push it to the back of the queue. If it is `0`, mark it as completed and calculate metrics.
