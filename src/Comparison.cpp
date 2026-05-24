#include "Comparison.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <regex>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

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


void AlgorithmComparison::addScheduler(Scheduler* scheduler, const std::vector<Process>& processes) {
    schedulers.push_back({scheduler, processes});
}

void AlgorithmComparison::runAllSchedulers() {
    results.clear();

    for (auto& info : schedulers) {
        // Clear any previous state
        info.scheduler->clearProcesses();

        // Add all processes
        for (const auto& p : info.processes) {
            info.scheduler->addProcess(p);
        }

        // Run the scheduler
        info.scheduler->run();
        info.scheduler->calculateMetrics();

        // Store results
        AlgorithmResult result;
        result.algorithmName = info.scheduler->getAlgorithmName();
        result.metrics = info.scheduler->getMetrics();
        result.totalExecutionTime = info.scheduler->getProcessCount() > 0 
            ? info.processes[0].completionTime 
            : 0;

        // Get actual total execution time from processes
        int maxCompletion = 0;
        for (const auto& p : info.processes) {
            maxCompletion = std::max(maxCompletion, p.completionTime);
        }
        result.totalExecutionTime = maxCompletion;

        results.push_back(result);
    }
}

void AlgorithmComparison::displayComparison() const {
    Utils::printHeader("Algorithm Comparison Results");
    printComparisonTable();
}

const AlgorithmComparison::AlgorithmResult* AlgorithmComparison::getResult(const std::string& algorithmName) const {
    for (const auto& result : results) {
        if (result.algorithmName == algorithmName) {
            return &result;
        }
    }
    return nullptr;
}

void AlgorithmComparison::clear() {
    schedulers.clear();
    results.clear();
}

void AlgorithmComparison::printComparisonHeader() const {
    std::cout << std::left
              << std::setw(35) << "Algorithm"
              << std::setw(20) << "Avg Wait Time"
              << std::setw(20) << "Avg Turnaround"
              << std::setw(18) << "Avg Response"
              << std::setw(15) << "Total Time"
              << std::endl;
    Utils::printDivider(100, '-');
}

void AlgorithmComparison::printComparisonTable() const {
    if (results.empty()) {
        std::cout << "No results to display. Run schedulers first." << std::endl;
        return;
    }

    printComparisonHeader();

    std::cout << std::fixed << std::setprecision(2);

    for (const auto& result : results) {
        std::cout << std::left
                  << std::setw(35) << result.algorithmName
                  << std::setw(20) << result.metrics.averageWaitingTime
                  << std::setw(20) << result.metrics.averageTurnaroundTime
                  << std::setw(18) << result.metrics.averageResponseTime
                  << std::setw(15) << result.totalExecutionTime
                  << std::endl;
    }

    std::cout << "\n";

    // Display analysis
    std::cout << "Analysis:" << std::endl;
    if (results.size() >= 2) {
        double fcfsWait = results[0].metrics.averageWaitingTime;
        double sjfWait = results[1].metrics.averageWaitingTime;

        if (sjfWait < fcfsWait) {
            std::cout << "  [+] SJF has " << std::fixed << std::setprecision(2) 
                      << ((fcfsWait - sjfWait) / fcfsWait * 100.0) 
                      << "% lower average waiting time than FCFS" << std::endl;
        }

        double fcfsTurnaround = results[0].metrics.averageTurnaroundTime;
        double sjfTurnaround = results[1].metrics.averageTurnaroundTime;

        if (sjfTurnaround < fcfsTurnaround) {
            std::cout << "  [+] SJF has " << std::fixed << std::setprecision(2) 
                      << ((fcfsTurnaround - sjfTurnaround) / fcfsTurnaround * 100.0) 
                      << "% lower average turnaround time than FCFS" << std::endl;
        }
    }
}

bool AlgorithmComparison::isLowerBetter(const std::string& metric) const {
    // For scheduling metrics, lower waiting/turnaround time is better
    return true;
}

// ============================================================================
// PROFESSIONAL DASHBOARD IMPLEMENTATION
// ============================================================================

void AlgorithmComparison::displayProfessionalDashboard() const {
    if (results.empty()) {
        std::cout << "No results to display. Run schedulers first." << std::endl;
        return;
    }

    printDashboardHeader();
    std::cout << std::endl;

    // Print metrics table
    printMetricsTable();
    std::cout << std::endl;

    // Print rankings section
    printRankingsSection();
    std::cout << std::endl;

    // Print recommendation section
    printRecommendationSection();
}

std::vector<AlgorithmComparison::AlgorithmRank> AlgorithmComparison::getRankings() const {
    return calculateRankings();
}

std::pair<std::string, std::string> AlgorithmComparison::getRecommendation() const {
    return generateRecommendation();
}

void AlgorithmComparison::printDashboardHeader() const {
    Utils::printHeader("PROFESSIONAL ANALYSIS DASHBOARD");
}

void AlgorithmComparison::printMetricsTable() const {
    Utils::printHeader("Performance Metrics Comparison");

    // Print table header
    std::cout << std::left
              << std::setw(25) << "Metric"
              << std::setw(20) << "FCFS"
              << std::setw(20) << "SJF"
              << std::setw(20) << "Priority"
              << std::setw(15) << "Best"
              << std::endl;
    Utils::printDivider(100, '-');

    std::cout << std::fixed << std::setprecision(2);

    // Metrics to display
    std::vector<std::pair<std::string, double (Metrics::*)>> metrics = {
        {"Avg Waiting Time", &Metrics::averageWaitingTime},
        {"Avg Turnaround Time", &Metrics::averageTurnaroundTime},
        {"Avg Response Time", &Metrics::averageResponseTime},
        {"CPU Utilization %", &Metrics::cpuUtilization},
        {"Throughput", &Metrics::throughput}
    };

    for (const auto& metric : metrics) {
        std::cout << std::left << std::setw(25) << metric.first;

        // Print values for each algorithm
        std::string bestAlgo = getBestAlgorithmForMetric(metric.first);

        for (const auto& result : results) {
            double value = result.metrics.*metric.second;
            std::cout << std::setw(20) << formatMetricValue(value, 2);
        }

        // Print best algorithm for this metric
        std::cout << std::setw(15) << ("(" + bestAlgo + ")");
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void AlgorithmComparison::printRankingsSection() const {
    Utils::printHeader("Algorithm Rankings");

    auto rankings = calculateRankings();

    std::cout << std::left
              << std::setw(5) << "Rank"
              << std::setw(20) << "Algorithm"
              << std::setw(15) << "Score"
              << std::setw(20) << "Metrics Won"
              << std::endl;
    Utils::printDivider(60, '-');

    int rank = 1;
    for (const auto& result : rankings) {
        std::cout << std::left
                  << std::setw(5) << (std::to_string(rank) + ".")
                  << std::setw(20) << result.algorithmName
                  << std::setw(15) << result.overallScore
                  << std::setw(20) << result.totalMetricsWon
                  << std::endl;
        rank++;
    }

    std::cout << std::endl;
}

void AlgorithmComparison::printRecommendationSection() const {
    Utils::printHeader("Recommendation");

    auto [bestAlgo, explanation] = generateRecommendation();

    std::cout << "[BEST] Best Overall Algorithm: " << bestAlgo << std::endl;
    std::cout << std::endl;
    std::cout << "Reason:" << std::endl;
    std::cout << explanation;
    std::cout << std::endl;
}

std::vector<AlgorithmComparison::AlgorithmRank> AlgorithmComparison::calculateRankings() const {
    std::vector<AlgorithmRank> rankings;

    for (const auto& result : results) {
        AlgorithmRank rank;
        rank.algorithmName = result.algorithmName;
        rank.overallScore = 0;
        rank.totalMetricsWon = 0;

        rankings.push_back(rank);
    }

    // Evaluate each metric
    std::vector<std::string> metrics = {
        "Avg Waiting Time",
        "Avg Turnaround Time",
        "Avg Response Time",
        "CPU Utilization %",
        "Throughput"
    };

    for (const auto& metric : metrics) {
        // Helper lambda to get metric value
        auto getMetricValue = [&](const AlgorithmResult& result) -> double {
            if (metric == "Avg Waiting Time") return result.metrics.averageWaitingTime;
            if (metric == "Avg Turnaround Time") return result.metrics.averageTurnaroundTime;
            if (metric == "Avg Response Time") return result.metrics.averageResponseTime;
            if (metric == "CPU Utilization %") return result.metrics.cpuUtilization;
            if (metric == "Throughput") return result.metrics.throughput;
            return 0.0;
        };

        bool lowerIsBetter = (metric != "Throughput");
        double bestValue = getMetricValue(results[0]);

        // Find the best value across all algorithms
        for (const auto& result : results) {
            double currentValue = getMetricValue(result);
            if (lowerIsBetter) {
                if (currentValue < bestValue) bestValue = currentValue;
            } else {
                if (currentValue > bestValue) bestValue = currentValue;
            }
        }

        // Award points to ALL algorithms that tie for best
        for (auto& rank : rankings) {
            // Find corresponding result
            for (const auto& result : results) {
                if (result.algorithmName == rank.algorithmName) {
                    double currentValue = getMetricValue(result);
                    
                    // Use small epsilon for floating point comparison
                    const double epsilon = 1e-6;
                    bool isBest = std::abs(currentValue - bestValue) < epsilon;
                    
                    if (isBest) {
                        rank.overallScore++;
                        rank.totalMetricsWon++;
                        rank.metricRanks[metric] = 1;
                    } else {
                        rank.metricRanks[metric] = 2;
                    }
                    break;
                }
            }
        }
    }

    // Sort by overall score (descending)
    std::sort(rankings.begin(), rankings.end(),
              [](const AlgorithmRank& a, const AlgorithmRank& b) {
                  return a.overallScore > b.overallScore;
              });

    return rankings;
}

std::pair<std::string, std::string> AlgorithmComparison::generateRecommendation() const {
    auto rankings = calculateRankings();

    if (rankings.empty()) {
        return {"None", "No algorithms to recommend.\n"};
    }

    // Get best algorithm
    const auto& best = rankings[0];

    // Build explanation
    std::string explanation;
    explanation += "  - Best performance in " + std::to_string(best.totalMetricsWon) + " metrics\n";
    explanation += "  - Superior algorithm for this workload\n";

    // Add specific strengths
    if (best.algorithmName == "First Come First Served (FCFS)") {
        explanation += "  - Simple, predictable scheduling\n";
        explanation += "  - Good for sequential, long-running tasks\n";
    } else if (best.algorithmName == "Shortest Job First (SJF)") {
        explanation += "  - Minimizes waiting time\n";
        explanation += "  - Excellent for varying burst times\n";
        explanation += "  - Best overall fairness and responsiveness\n";
    } else if (best.algorithmName == "Priority Scheduling") {
        explanation += "  - Respects priority requirements\n";
        explanation += "  - Best for mixed-priority workloads\n";
    }

    return {best.algorithmName, explanation};
}

std::string AlgorithmComparison::getBestAlgorithmForMetric(const std::string& metric) const {
    if (results.empty()) {
        return "N/A";
    }

    bool lowerIsBetter = true;  // For all scheduling metrics except throughput

    if (metric == "Throughput") {
        lowerIsBetter = false;  // Higher throughput is better
    }

    std::string bestAlgo = results[0].algorithmName;
    double bestValue = 0.0;

    // Helper lambda to get metric value
    auto getMetricValue = [&](const AlgorithmResult& result) -> double {
        if (metric == "Avg Waiting Time") return result.metrics.averageWaitingTime;
        if (metric == "Avg Turnaround Time") return result.metrics.averageTurnaroundTime;
        if (metric == "Avg Response Time") return result.metrics.averageResponseTime;
        if (metric == "CPU Utilization %") return result.metrics.cpuUtilization;
        if (metric == "Throughput") return result.metrics.throughput;
        return 0.0;
    };

    bestValue = getMetricValue(results[0]);

    for (size_t i = 1; i < results.size(); ++i) {
        double currentValue = getMetricValue(results[i]);

        bool isBetter = false;
        if (lowerIsBetter && currentValue < bestValue) {
            isBetter = true;
        } else if (!lowerIsBetter && currentValue > bestValue) {
            isBetter = true;
        }

        if (isBetter) {
            bestValue = currentValue;
            bestAlgo = results[i].algorithmName;
        }
    }

    return bestAlgo;
}

std::string AlgorithmComparison::formatMetricValue(double value, int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

std::string AlgorithmComparison::getAIExplanation() const {
    const char* apiKeyEnv = std::getenv("DEEPSEEK_API_KEY");
    if (apiKeyEnv && std::string(apiKeyEnv).length() > 5) {
        std::string apiKey = std::string(apiKeyEnv);
        std::cout << "\n\033[36m[AI Explanation] DEEPSEEK_API_KEY detected! Querying DeepSeek AI for deep analysis...\033[0m" << std::endl;
        
        // Build prompt with actual results data
        std::ostringstream promptStream;
        promptStream << "Analyze the following CPU Scheduling comparison results:\n\n";
        for (const auto& r : results) {
            promptStream << "Algorithm: " << r.algorithmName << "\n"
                         << " - Avg Waiting Time: " << r.metrics.averageWaitingTime << " units\n"
                         << " - Avg Turnaround Time: " << r.metrics.averageTurnaroundTime << " units\n"
                         << " - Avg Response Time: " << r.metrics.averageResponseTime << " units\n"
                         << " - CPU Utilization: " << r.metrics.cpuUtilization << "%\n"
                         << " - Throughput: " << r.metrics.throughput << " processes/unit\n\n";
        }
        promptStream << "Explain: 1. Which algorithm performed best and why. 2. The tradeoffs of waiting times, response times, and throughput between these algorithms. Keep it concise, professional, and educational (limit to 3-4 short paragraphs). Do not use markdown wrappers like ``` or other HTML formatting.";
        
        std::string apiResponse = queryDeepSeekAIExplanation(promptStream.str(), apiKey);
        if (!apiResponse.empty()) {
            std::string explanation = extractJSONStringField(apiResponse, "content");
            if (explanation.empty()) {
                explanation = extractJSONStringField(apiResponse, "text");
            }
            if (!explanation.empty()) {
                return "\033[32m🚀 [DeepSeek AI Deep Analysis]\033[0m\n" + explanation;
            }
        }
        std::cout << "\033[33m⚠️ [AI Explanation] DeepSeek API query failed or returned invalid JSON. Falling back to offline rule-based analysis...\033[0m" << std::endl;
    }

    return generateOfflineAIExplanation();
}

std::string AlgorithmComparison::queryDeepSeekAIExplanation(const std::string& prompt, const std::string& apiKey) const {
    // 1. Prepare prompt for JSON inclusion by escaping backslashes, double quotes, and newlines
    std::string safePrompt = prompt;
    safePrompt = std::regex_replace(safePrompt, std::regex("\\\\"), "\\\\");
    safePrompt = std::regex_replace(safePrompt, std::regex("\""), "\\\"");
    safePrompt = std::regex_replace(safePrompt, std::regex("\n"), "\\n");

    // 2. Build the JSON structure and write to a temporary file
    std::string url = "https://api.deepseek.com/chat/completions";
    std::string tempFile = ".temp_comp_payload.json";
    
    std::ofstream outFile(tempFile);
    if (!outFile.is_open()) {
        std::cerr << "❌ [Comparison] Failed to create temp payload file!" << std::endl;
        return "";
    }

    outFile << "{"
            << "\"model\":\"deepseek-chat\","
            << "\"messages\":[{"
            << "\"role\":\"user\","
            << "\"content\":\"" << safePrompt << "\""
            << "}],"
            << "\"temperature\":0.2"
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
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        result += buffer;
    }
    pclose(fp);
    std::remove(tempFile.c_str());
    return result;
}

std::string AlgorithmComparison::generateOfflineAIExplanation() const {
    auto rankings = calculateRankings();
    if (rankings.empty()) return "  No algorithms evaluated for explanation.\n";

    std::ostringstream oss;
    oss << "\n\033[1m\033[36m🎓 [Offline Smart Analysis Engine] Educational Insight & Trade-offs\033[0m\n\n";

    // Describe Winner
    std::string bestAlgo = rankings[0].algorithmName;
    oss << "🏆 \033[1m" << bestAlgo << "\033[0m achieved the highest rank in this evaluation session.\n\n";

    // Performance details
    std::string bestWait = getBestAlgorithmForMetric("Avg Waiting Time");
    std::string bestTurn = getBestAlgorithmForMetric("Avg Turnaround Time");
    std::string bestResp = getBestAlgorithmForMetric("Avg Response Time");
    std::string bestThru = getBestAlgorithmForMetric("Throughput");

    oss << " - \033[1mWaiting Time\033[0m: \033[32m" << bestWait << "\033[0m won this metric. ";
    if (bestWait.find("SJF") != std::string::npos) {
        oss << "Shortest Job First minimizes wait times by executing shorter jobs first, preventing the queue from piling up.\n";
    } else {
        oss << "This algorithm achieved the fastest scheduling queue throughput.\n";
    }

    oss << " - \033[1mResponse Time\033[0m: \033[32m" << bestResp << "\033[0m won this metric. ";
    if (bestResp.find("Round Robin") != std::string::npos) {
        oss << "Round Robin provides excellent interactive responsiveness due to preemptive time-slicing, giving every process immediate CPU time.\n";
    } else {
        oss << "This algorithm quickly initiated process execution.\n";
    }

    oss << " - \033[1mThroughput\033[0m: \033[32m" << bestThru << "\033[0m won this metric. ";
    oss << "A high throughput indicates efficient process graduation, which maximizes scheduling speed.\n\n";

    // System Trade-offs
    oss << "\033[1mSystem Trade-offs Analysis:\033[0m\n";
    oss << " 1. \033[1mFCFS vs SJF\033[0m: FCFS is completely fair in terms of arrival sequence but is prone to the \"convoy effect\" where a long process blocks subsequent shorter tasks. SJF avoids this but requires knowing burst times in advance, which can cause starvation for longer jobs.\n";
    oss << " 2. \033[1mRound Robin (Preemptive)\033[0m: Provides fair and immediate CPU access (low response times) which is ideal for interactive operating systems. However, it incurs significant context switch overhead (CPU core save/restore phases) which increases average turnaround time.\n";
    oss << " 3. \033[1mPriority Scheduling\033[0m: Allocates resources based on critical process needs (e.g. system services vs user apps). However, without priority aging, low-priority tasks can remain in the READY state indefinitely (starvation).\n";

    return oss.str();
}
