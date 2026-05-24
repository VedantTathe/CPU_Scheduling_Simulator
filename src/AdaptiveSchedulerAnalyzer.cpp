#include "AdaptiveSchedulerAnalyzer.h"
#include "SchedulerRegistry.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// ANSI colors for clean dashboard logs
namespace ASA {
    constexpr const char* RESET   = "\033[0m";
    constexpr const char* BOLD    = "\033[1m";
    constexpr const char* GREEN   = "\033[32m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* CYAN    = "\033[36m";
    constexpr const char* RED     = "\033[31m";
}

// Helper to extract JSON string fields robustly, handling escaped characters and spacing variations
static std::string extractJSONStringField(const std::string& json, const std::string& fieldName) {
    size_t pos = json.find("\"" + fieldName + "\"");
    if (pos == std::string::npos) {
        return "";
    }
    pos = json.find(":", pos + fieldName.length() + 2);
    if (pos == std::string::npos) {
        return "";
    }
    pos = json.find("\"", pos + 1);
    if (pos == std::string::npos) {
        return "";
    }
    size_t start = pos + 1;
    std::string value = "";
    for (size_t i = start; i < json.length(); ++i) {
        if (json[i] == '\\') {
            if (i + 1 < json.length()) {
                char c = json[i + 1];
                if (c == 'n') value += '\n';
                else if (c == 't') value += '\t';
                else if (c == 'r') value += '\r';
                else value += c;
                i++;
            }
        } else if (json[i] == '"') {
            break; // End of string
        } else {
            value += json[i];
        }
    }
    return value;
}

std::string AdaptiveSchedulerAnalyzer::analyzeAndSelect(const std::vector<Process>& processes, std::string& explanationOut) {
    if (processes.empty()) {
        explanationOut = "No processes loaded. Cannot recommend any scheduler.";
        return "FCFS";
    }

    const char* apiKeyEnv = std::getenv("DEEPSEEK_API_KEY");
    if (apiKeyEnv && std::string(apiKeyEnv).length() > 5) {
        std::string apiKey = std::string(apiKeyEnv);
        std::cout << ASA::CYAN << "[AdaptiveAnalyzer] DEEPSEEK_API_KEY detected! Querying DeepSeek AI..." << ASA::RESET << std::endl;
        
        std::string recommendation = selectOnlineAI(processes, apiKey, explanationOut);
        if (!recommendation.empty() && isAlgorithmRegistered(recommendation)) {
            return recommendation;
        }
        std::cout << ASA::YELLOW << "[AdaptiveAnalyzer] AI selection failed or returned unregistered scheduler. Falling back to local offline Heuristic Engine..." << ASA::RESET << std::endl;
    }

    return selectOfflineHeuristic(processes, explanationOut);
}

std::string AdaptiveSchedulerAnalyzer::selectOfflineHeuristic(const std::vector<Process>& processes, std::string& explanationOut) {
    std::ostringstream ss;
    
    // Check if EDF is registered and we have tight real-time constraints/deadlines
    bool hasEDF = isAlgorithmRegistered("EDF");
    bool hasCustomDeadlines = false;
    bool hasRealTimeKeywords = false;
    
    for (const auto& p : processes) {
        // If deadline is custom (not the default arrival + burst + 10)
        if (p.deadline != p.arrivalTime + p.burstTime + 10) {
            hasCustomDeadlines = true;
        }
        // Check for real-time keywords in process names
        std::string nameLower = p.name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        if (nameLower.find("emergency") != std::string::npos ||
            nameLower.find("icu") != std::string::npos ||
            nameLower.find("sensor") != std::string::npos ||
            nameLower.find("critical") != std::string::npos ||
            nameLower.find("realtime") != std::string::npos ||
            nameLower.find("monitor") != std::string::npos) {
            hasRealTimeKeywords = true;
        }
    }

    if (hasEDF && (hasCustomDeadlines || hasRealTimeKeywords)) {
        ss << "Mathematical Heuristics analyzed process parameters:\n"
           << " - Detected real-time critical process requirements or customized deadlines.\n"
           << " - Earliest Deadline First (EDF) Preemptive scheduling is recommended to prevent deadline misses\n"
           << "   and guarantee optimal completion times for time-bounded workloads.";
        explanationOut = ss.str();
        return "EDF";
    }

    // Evaluate standard deviation of burst times
    double burstStdDev = calculateBurstStdDev(processes);
    
    // Evaluate priority variations
    bool hasPriorities = false;
    int firstPriority = processes[0].priority;
    for (const auto& p : processes) {
        if (p.priority != firstPriority) {
            hasPriorities = true;
            break;
        }
    }

    if (burstStdDev >= 3.0) {
        ss << "Mathematical Heuristics analyzed process parameters:\n"
           << " - Burst time Standard Deviation is extremely high (" << std::fixed << std::setprecision(2) << burstStdDev << ").\n"
           << " - Shortest Job First (SJF) is selected because it is mathematically proven to minimize\n"
           << "   average waiting time by executing shorter tasks first, mitigating convoy effects.";
        explanationOut = ss.str();
        return "SJF";
    }

    if (hasPriorities && isAlgorithmRegistered("Priority")) {
        ss << "Mathematical Heuristics analyzed process parameters:\n"
           << " - Workload has non-homogeneous process priorities (variance detected).\n"
           << " - Priority Scheduling is recommended to ensure critical, higher-priority system tasks\n"
           << "   receive preferential execution resource allocation.";
        explanationOut = ss.str();
        return "Priority";
    }

    // Default choice between RR and FCFS
    if (isAlgorithmRegistered("RR")) {
        ss << "Mathematical Heuristics analyzed process parameters:\n"
           << " - Homogeneous batch workload detected with low burst variation and identical priorities.\n"
           << " - Round Robin (RR) Scheduling is selected to provide fair, interactive, and time-sliced\n"
           << "   resource distribution among all running processes.";
        explanationOut = ss.str();
        return "RR";
    }

    ss << "Mathematical Heuristics analyzed process parameters:\n"
       << " - Standard homogeneous batch workload detected.\n"
       << " - First Come First Served (FCFS) Scheduling is recommended due to its minimal context switching\n"
       << "   overhead and simple implementation characteristics.";
    explanationOut = ss.str();
    return "FCFS";
}

std::string AdaptiveSchedulerAnalyzer::selectOnlineAI(const std::vector<Process>& processes, const std::string& apiKey, std::string& explanationOut) {
    // 1. Format the process descriptions into a clean payload
    std::ostringstream listStream;
    listStream << "[";
    for (size_t i = 0; i < processes.size(); ++i) {
        const auto& p = processes[i];
        listStream << "{\\\"pid\\\":" << p.pid 
                   << ",\\\"name\\\":\\\"" << p.name << "\\\""
                   << ",\\\"arrival\\\":" << p.arrivalTime 
                   << ",\\\"burst\\\":" << p.burstTime 
                   << ",\\\"priority\\\":" << p.priority 
                   << ",\\\"deadline\\\":" << p.deadline << "}";
        if (i + 1 < processes.size()) {
            listStream << ",";
        }
    }
    listStream << "]";

    // 2. Build the JSON structure and write to a temporary file
    std::string url = "https://api.deepseek.com/chat/completions";
    std::string tempFile = ".temp_adaptive_payload.json";
    
    std::ofstream outFile(tempFile);
    if (!outFile.is_open()) {
        std::cerr << "❌ [AdaptiveAnalyzer] Failed to create temp payload file!" << std::endl;
        return "";
    }

    outFile << "{"
            << "\"model\":\"deepseek-chat\","
            << "\"messages\":[{"
            << "\"role\":\"user\","
            << "\"content\":\"Select the absolute best scheduling algorithm (exactly one of: FCFS, SJF, Priority, RR, EDF) for this workload: " 
            << listStream.str() 
            << ". You must respond strictly with a valid raw JSON object matching this structure: {\\\"recommended_algorithm\\\": \\\"<ShortName>\\\", \\\"reason\\\": \\\"<One concise paragraph explaining reasoning>\\\"}. Do not include markdown codeblocks, backticks, or any other extra text.\""
            << "}],"
            << "\"temperature\":0.1"
            << "}";
    outFile.close();

    // 3. Build the curl system command using the temporary file payload
    std::string cmd = "curl -s -X POST \"" + url + "\" ";
    cmd += "-H \"Content-Type: application/json\" ";
    cmd += "-H \"Authorization: Bearer " + apiKey + "\" ";
    cmd += "-d @\"" + tempFile + "\"";

    // 4. Open pipeline
    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) {
        std::remove(tempFile.c_str());
        return "";
    }

    char buffer[1024];
    std::string response = "";
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        response += buffer;
    }
    pclose(fp);
    std::remove(tempFile.c_str());

    if (response.empty()) {
        return "";
    }

    // Extract content block robustly
    std::string innerContent = extractJSONStringField(response, "content");
    if (innerContent.empty()) {
        innerContent = extractJSONStringField(response, "text");
    }
    if (innerContent.empty()) {
        innerContent = response;
    }

    // Parse recommended algorithm and reason from the inner JSON payload robustly
    std::string algo = extractJSONStringField(innerContent, "recommended_algorithm");
    std::string reason = extractJSONStringField(innerContent, "reason");

    // Clean up spaces
    algo.erase(0, algo.find_first_not_of(" \t\r\n"));
    algo.erase(algo.find_last_not_of(" \t\r\n") + 1);

    if (algo.empty()) {
        return "";
    }

    if (reason.empty()) {
        reason = "DeepSeek AI successfully selected " + algo + " for the given process workload parameters.";
    }

    explanationOut = "DeepSeek AI Cognitive Workload Analysis:\n - Recommended Scheduler: " + algo + "\n - Reason: " + reason;
    return algo;
}

double AdaptiveSchedulerAnalyzer::calculateBurstStdDev(const std::vector<Process>& processes) const {
    if (processes.size() <= 1) return 0.0;

    double sum = 0.0;
    for (const auto& p : processes) {
        sum += p.burstTime;
    }
    double mean = sum / processes.size();

    double sqSum = 0.0;
    for (const auto& p : processes) {
        sqSum += std::pow(p.burstTime - mean, 2);
    }
    
    return std::sqrt(sqSum / processes.size());
}

bool AdaptiveSchedulerAnalyzer::isAlgorithmRegistered(const std::string& name) const {
    auto registered = SchedulerRegistry::getInstance().getRegisteredSchedulers();
    for (const auto& info : registered) {
        if (info.name == name) {
            return true;
        }
    }
    return false;
}
