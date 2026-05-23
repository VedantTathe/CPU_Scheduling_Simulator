#pragma once

#include "SchedulerRegistry.h"
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
using PluginHandle = HMODULE;
#else
using PluginHandle = void*;
#endif

/**
 * @class PluginManager
 * @brief Manages platform-independent dynamic loading of external scheduler modules.
 */
class PluginManager {
private:
    std::vector<PluginHandle> loadedHandles;

public:
    PluginManager() = default;
    ~PluginManager();

    /**
     * @brief Scans a directory and dynamically loads matching dynamic library plugins.
     * @param folderPath Location of the plugins directory
     */
    void scanPlugins(const std::string& folderPath);

    /**
     * @brief Load a specific dynamic library.
     * @param path Full filepath to the plugin file
     * @return True if loaded and register_plugin succeeded
     */
    bool loadPlugin(const std::string& path);

    /**
     * @brief Unloads all plugins and releases platform resources.
     */
    void unloadAll();
};
