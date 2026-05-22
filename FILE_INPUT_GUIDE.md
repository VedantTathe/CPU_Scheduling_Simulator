# File Input Support - Usage Guide

## Quick Start

The simulator now supports loading processes from text files in addition to manual input.

### Sample Data Files

Pre-built sample datasets are available in the `data/` directory:

- **data/sample1.txt** - Basic 4-process test case
- **data/sample2.txt** - Extended 5-process test case  
- **data/priority_test.txt** - 6-process dataset designed to showcase Priority Scheduling

### Using File Input

1. Run the simulator: `.\build\bin\Release\simulator.exe`

2. From the main menu, select option **2 - Load Processes from File**

3. Enter the filename:
   ```
   Enter filename (or full path): data/sample1.txt
   ```

4. The simulator will:
   - Validate the file format
   - Load all processes
   - Display a preview of loaded processes
   - Return to the main menu

### File Format

```
4
1 0 7 3
2 2 4 1
3 4 1 2
4 5 4 4
```

**First line**: Total number of processes  
**Following lines**: `PID ArrivalTime BurstTime Priority`

- **PID**: Unique Process Identifier (must be positive, must be unique)
- **ArrivalTime**: When process arrives (non-negative integer)
- **BurstTime**: CPU time needed (positive integer)
- **Priority**: Priority level (1=highest, positive integers only)

### Error Handling

The FileHandler provides detailed error messages for:
- Missing files
- Invalid numeric values
- Incorrect number of fields
- Negative or zero values (where not allowed)
- Duplicate Process IDs

Example error:
```
❌ Error on line 3: Burst time must be positive, got -5.
```

### Batch Workflow

**Professional Use Case**:

1. Create test dataset file
2. Load with option 2
3. Run comparison of all 3 algorithms
4. Analyze results
5. Repeat with different test cases

This enables **rapid testing** of scheduling algorithms across multiple datasets!

### Creating Your Own Dataset

Create a `.txt` file following the format above:

```
# my_processes.txt
5
101 0 8 2
102 1 4 1
103 3 6 3
104 5 2 2
105 7 5 1
```

Then load it in the simulator:
```
Enter filename (or full path): my_processes.txt
```
