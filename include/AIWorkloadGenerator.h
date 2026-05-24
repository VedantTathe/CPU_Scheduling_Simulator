#pragma once

#include "CPUProcess.h"
#include <vector>
#include <string>

/**
 * @class AIWorkloadGenerator
 * @brief Component to generate realistic process datasets from natural language prompts.
 * Dynamically queries DeepSeek API if DEEPSEEK_API_KEY is available, or defaults to a highly polished
 * offline rule-based generator.
 */
class AIWorkloadGenerator {
public:
    AIWorkloadGenerator() = default;
    ~AIWorkloadGenerator() = default;

    /**
     * @brief Generates a realistic process dataset based on a natural language prompt.
     * @param prompt Natural language scenario (e.g. "gaming workload")
     * @return Vector of populated Process objects
     */
    std::vector<Process> generateWorkload(const std::string& prompt);

private:
    /**
     * @brief Generates a highly customized process dataset offline using intelligent keyword parsing.
     */
    std::vector<Process> generateOfflineWorkload(const std::string& prompt);

    /**
     * @brief Query DeepSeek AI API via system curl processes (fully dependency-free).
     */
    std::string callDeepSeekAPI(const std::string& prompt, const std::string& apiKey);

    /**
     * @brief Parses JSON list outputs into internal Process representation.
     */
    std::vector<Process> parseJSONResponse(const std::string& jsonStr);
};
