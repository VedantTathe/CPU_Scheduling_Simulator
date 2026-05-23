#include "SchedulerRegistry.h"

SchedulerRegistry& SchedulerRegistry::getInstance() {
    static SchedulerRegistry instance;
    return instance;
}

void SchedulerRegistry::registerScheduler(const std::string& name, const std::string& description, CreatorFunc creator) {
    std::lock_guard<std::mutex> lock(registryMutex);
    registry[name] = {name, description, creator};
}

std::unique_ptr<Scheduler> SchedulerRegistry::createScheduler(const std::string& name) {
    std::lock_guard<std::mutex> lock(registryMutex);
    auto it = registry.find(name);
    if (it != registry.end()) {
        return it->second.creator();
    }
    return nullptr;
}

std::vector<SchedulerRegistry::SchedulerInfo> SchedulerRegistry::getRegisteredSchedulers() const {
    std::lock_guard<std::mutex> lock(registryMutex);
    std::vector<SchedulerInfo> list;
    for (const auto& pair : registry) {
        list.push_back(pair.second);
    }
    return list;
}

void SchedulerRegistry::clear() {
    std::lock_guard<std::mutex> lock(registryMutex);
    registry.clear();
}
