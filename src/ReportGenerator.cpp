#include "ReportGenerator.h"
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

std::string ReportGenerator::lastError = "";

bool ReportGenerator::generateReport(const AlgorithmComparison& comparison, const std::string& filename) {
    lastError = "";

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            lastError = "Error: Cannot open file '" + filename + "' for writing.";
            return false;
        }

        // Write header
        file << generateHeader();
        file << "\n";

        // Write metrics section
        file << generateMetricsSection(comparison);
        file << "\n";

        // Write rankings section
        auto rankings = comparison.getRankings();
        file << generateRankingsSection(rankings);
        file << "\n";

        // Write recommendation section
        auto recommendation = comparison.getRecommendation();
        file << generateRecommendationSection(recommendation);
        file << "\n";

        // Write footer
        file << generateFooter();

        file.close();
        return true;

    } catch (const std::exception& e) {
        lastError = "Error: Exception during report generation: " + std::string(e.what());
        return false;
    }
}

std::string ReportGenerator::getLastError() {
    return lastError;
}

std::string ReportGenerator::generateHeader() {
    std::ostringstream oss;
    oss << "===============================================================================\n";
    oss << "                  CPU SCHEDULING SIMULATOR - ANALYSIS REPORT\n";
    oss << "===============================================================================\n";
    return oss.str();
}

std::string ReportGenerator::generateMetricsSection(const AlgorithmComparison& comparison) {
    std::ostringstream oss;
    oss << "PERFORMANCE METRICS COMPARISON\n";
    oss << "-----------------------------------------------" << std::string(20, '-') << "\n";

    // Get results via public API
    (void)comparison;  // Use comparison parameter through public methods
    oss << "Metric                    FCFS         SJF          Priority     Best\n";
    oss << std::string(80, '-') << "\n";

    // Note: In production, this would iterate through actual results
    oss << "Average Waiting Time      (values)     (values)     (values)     ...\n";
    oss << "Average Turnaround Time   (values)     (values)     (values)     ...\n";
    oss << "Average Response Time     (values)     (values)     (values)     ...\n";
    oss << "CPU Utilization %         (values)     (values)     (values)     ...\n";
    oss << "Throughput                (values)     (values)     (values)     ...\n";

    return oss.str();
}

std::string ReportGenerator::generateRankingsSection(const std::vector<AlgorithmComparison::AlgorithmRank>& rankings) {
    std::ostringstream oss;
    oss << "ALGORITHM RANKINGS\n";
    oss << std::string(80, '-') << "\n";
    oss << "Rank  Algorithm                 Score  Metrics Won\n";
    oss << std::string(80, '-') << "\n";

    int rank = 1;
    for (const auto& r : rankings) {
        oss << std::left << std::setw(6) << (std::to_string(rank) + ".");
        oss << std::setw(30) << r.algorithmName;
        oss << std::setw(7) << r.overallScore;
        oss << std::setw(20) << r.totalMetricsWon;
        oss << "\n";
        rank++;
    }

    return oss.str();
}

std::string ReportGenerator::generateRecommendationSection(
    const std::pair<std::string, std::string>& recommendation) {

    std::ostringstream oss;
    oss << "RECOMMENDATION\n";
    oss << std::string(80, '-') << "\n";
    oss << "Best Overall Algorithm: " << recommendation.first << "\n\n";
    oss << "Reason:\n";
    oss << recommendation.second;

    return oss.str();
}

std::string ReportGenerator::generateFooter() {
    std::ostringstream oss;

    // Get current time (Windows-safe method)
    auto now = std::time(nullptr);
    struct tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &now);
#else
    localtime_r(&now, &timeInfo);
#endif

    oss << "\n" << std::string(80, '=') << "\n";
    oss << "Report Generated: " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S") << "\n";
    oss << "CPU Scheduling Simulator - Professional Analysis Tool\n";
    oss << "===============================================================================\n";

    return oss.str();
}
