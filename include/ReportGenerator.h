#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "Comparison.h"
#include "CPURuntime.h"
#include <string>

/**
 * @class ReportGenerator
 * @brief Generates scheduling analysis reports from comparison results and live sessions.
 * 
 * Exports professional analysis reports to text files for archival and sharing.
 */
class ReportGenerator {
public:
    /**
     * @brief Generate and export comparison analysis report to file.
     * @param comparison AlgorithmComparison object with results
     * @param filename Output filename
     * @return true if export successful, false otherwise
     */
    static bool generateReport(const AlgorithmComparison& comparison, const std::string& filename);

    /**
     * @brief Generate and export live simulation runtime execution report to file.
     * @param session The RuntimeSession data to export
     * @param filename Output filename
     * @return true if export successful, false otherwise
     */
    static bool generateRuntimeReport(const RuntimeSession& session, const std::string& filename);

    /**
     * @brief Get last error message from report generation.
     * @return Error message string
     */
    static std::string getLastError();

private:
    static std::string lastError;

    /**
     * @brief Generate report header.
     * @return Formatted header string
     */
    static std::string generateHeader();

    /**
     * @brief Generate metrics section.
     * @param comparison Comparison object with results
     * @return Formatted metrics table
     */
    static std::string generateMetricsSection(const AlgorithmComparison& comparison);

    /**
     * @brief Generate rankings section.
     * @param rankings Rankings from comparison
     * @return Formatted rankings table
     */
    static std::string generateRankingsSection(const std::vector<AlgorithmComparison::AlgorithmRank>& rankings);

    /**
     * @brief Generate recommendation section.
     * @param recommendation Recommendation pair
     * @return Formatted recommendation text
     */
    static std::string generateRecommendationSection(
        const std::pair<std::string, std::string>& recommendation);

    /**
     * @brief Generate report footer.
     * @return Formatted footer with timestamp
     */
    static std::string generateFooter();
};

#endif // REPORTGENERATOR_H
