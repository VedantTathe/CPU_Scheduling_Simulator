#pragma once

/**
 * @struct Metrics
 * @brief Stores performance metrics for scheduling algorithms.
 * 
 * Contains calculated performance indicators that measure the efficiency
 * and effectiveness of a scheduling algorithm.
 */
struct Metrics {
    double averageWaitingTime;      // Average time processes spend waiting
    double averageTurnaroundTime;   // Average time from arrival to completion
    double averageResponseTime;     // Average time to first CPU allocation
    double cpuUtilization;          // Percentage of time CPU is executing processes
    double throughput;              // Number of processes completed per unit time

    /**
     * @brief Default constructor.
     * Initializes all metrics to 0.0
     */
    Metrics()
        : averageWaitingTime(0.0), averageTurnaroundTime(0.0),
          averageResponseTime(0.0), cpuUtilization(0.0), throughput(0.0) {}

    /**
     * @brief Reset all metrics to 0.
     */
    void reset() {
        averageWaitingTime = 0.0;
        averageTurnaroundTime = 0.0;
        averageResponseTime = 0.0;
        cpuUtilization = 0.0;
        throughput = 0.0;
    }

    /**
     * @brief Check if any metrics have been calculated.
     * @return true if any metric is non-zero
     */
    bool hasData() const {
        return averageWaitingTime > 0.0 || averageTurnaroundTime > 0.0 
               || cpuUtilization > 0.0;
    }
};
