#pragma once

#include "Scheduler.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <mutex>

/**
 * @class SchedulerRegistry
 * @brief Singleton registry of available scheduling algorithms.
 * Allows core schedulers and plugins to self-register dynamically.
 */
class SchedulerRegistry {
public:
    using CreatorFunc = std::function<std::unique_ptr<Scheduler>()>;

    struct SchedulerInfo {
        std::string name;
        std::string description;
        CreatorFunc creator;
    };

    /**
     * @brief Retrieve singleton instance.
     */
    static SchedulerRegistry& getInstance();

    /**
     * @brief Register a scheduler factory function.
     * @param name Unique short identifier (e.g. "FCFS")
     * @param description Brief human-readable description
     * @param creator Callable factory to allocate dynamic instances
     */
    void registerScheduler(const std::string& name, const std::string& description, CreatorFunc creator);

    /**
     * @brief Instantiates a registered scheduler.
     * @param name Unique name of scheduler
     * @return unique_ptr to new Scheduler instance or nullptr if not registered
     */
    std::unique_ptr<Scheduler> createScheduler(const std::string& name);

    /**
     * @brief Fetch metadata for all currently registered schedulers.
     */
    std::vector<SchedulerInfo> getRegisteredSchedulers() const;

    /**
     * @brief Reset registry contents.
     */
    void clear();

private:
    SchedulerRegistry() = default;
    ~SchedulerRegistry() = default;
    SchedulerRegistry(const SchedulerRegistry&) = delete;
    SchedulerRegistry& operator=(const SchedulerRegistry&) = delete;

    std::map<std::string, SchedulerInfo> registry;
    mutable std::mutex registryMutex;
};
