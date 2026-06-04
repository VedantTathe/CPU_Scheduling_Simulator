@echo off
echo Compiling CPU Scheduling Simulator...

g++ main.cpp Scheduler.cpp FCFS.cpp SJF.cpp PriorityScheduler.cpp RoundRobin.cpp -o simulator.exe

if %errorlevel% neq 0 (
    echo.
    echo Compilation FAILED! Please check the errors above.
    pause
    exit /b %errorlevel%
)

echo Compilation SUCCESSFUL!
echo You can run the simulator by typing:
echo .\simulator.exe
echo ========================================================
echo.
