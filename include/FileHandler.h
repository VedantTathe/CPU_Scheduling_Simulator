#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Process.h"

/**
 * @class FileHandler
 * @brief Handles reading and parsing process data from external files
 * 
 * Supports file format:
 * - First line: total number of processes (integer)
 * - Following lines: PID Arrival Burst Priority (space-separated integers)
 * 
 * Example:
 * 4
 * 1 0 7 3
 * 2 2 4 1
 * 3 4 1 2
 * 4 5 4 4
 */
class FileHandler {
public:
    /**
     * @brief Load processes from a text file
     * @param filename Path to the input file
     * @return Vector of Process objects parsed from file
     * @throws std::runtime_error if file cannot be opened or contains invalid data
     */
    static std::vector<Process> loadProcessesFromFile(const std::string& filename);

    /**
     * @brief Check if a file exists and is readable
     * @param filename Path to check
     * @return true if file exists and can be opened, false otherwise
     */
    static bool fileExists(const std::string& filename);

    /**
     * @brief Get descriptive error message from last operation
     * @return Error message string
     */
    static std::string getLastError();

private:
    // Static error storage for providing detailed error messages
    static std::string lastError;

    /**
     * @brief Validate parsed process data
     * @param processes Vector of processes to validate
     * @return true if all validations pass, false otherwise
     */
    static bool validateProcesses(const std::vector<Process>& processes);

    /**
     * @brief Check for duplicate process IDs
     * @param processes Vector of processes to check
     * @return true if no duplicates, false if duplicates found
     */
    static bool checkForDuplicatePIDs(const std::vector<Process>& processes);

    /**
     * @brief Parse a single process line
     * @param line Raw line from file
     * @param lineNumber Line number for error reporting
     * @param process Output parameter: parsed Process object
     * @return true if parsing successful, false otherwise
     */
    static bool parseProcessLine(const std::string& line, int lineNumber, Process& process);
};

#endif // FILEHANDLER_H
