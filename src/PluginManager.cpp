#include "PluginManager.h"
#include <iostream>
#include <filesystem>

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fs = std::filesystem;

typedef void (*RegisterPluginFunc)(SchedulerRegistry&);

PluginManager::~PluginManager() {
    unloadAll();
}

void PluginManager::scanPlugins(const std::string& folderPath) {
    std::cout << "[PluginManager] Scanning plugins directory '" << folderPath << "'..." << std::endl;
    
    try {
        if (!fs::exists(folderPath)) {
            fs::create_directories(folderPath);
            std::cout << "[PluginManager] Created empty plugins folder at '" << folderPath << "'" << std::endl;
            return;
        }

        int countDiscovered = 0;
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::string pathStr = entry.path().string();
                
                bool shouldLoad = false;
#ifdef _WIN32
                if (ext == ".dll") shouldLoad = true;
#else
                if (ext == ".so" || ext == ".dylib") shouldLoad = true;
#endif
                
                if (shouldLoad) {
                    std::cout << "[PluginManager] Discovered plugin module: " << entry.path().filename().string() << std::endl;
                    if (loadPlugin(pathStr)) {
                        countDiscovered++;
                    }
                }
            }
        }
        std::cout << "[PluginManager] Scanning finished. Discovered and loaded " << countDiscovered << " external plugin(s)." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[PluginManager] Error scanning directory: " << e.what() << std::endl;
    }
}

bool PluginManager::loadPlugin(const std::string& path) {
#ifdef _WIN32
    HMODULE handle = LoadLibraryA(path.c_str());
    if (!handle) {
        std::cerr << "❌ [PluginManager] Failed to load DLL from '" << path << "'. Error: " << GetLastError() << std::endl;
        return false;
    }
    
    // Find the register_plugin function
    RegisterPluginFunc registerFunc = (RegisterPluginFunc)GetProcAddress(handle, "register_plugin");
    if (!registerFunc) {
        std::cerr << "❌ [PluginManager] Plugin DLL '" << path << "' is missing the 'register_plugin' entry symbol!" << std::endl;
        FreeLibrary(handle);
        return false;
    }
#else
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        std::cerr << "❌ [PluginManager] Failed to load SO/DYLIB from '" << path << "'. Error: " << dlerror() << std::endl;
        return false;
    }
    
    // Find the register_plugin function
    RegisterPluginFunc registerFunc = (RegisterPluginFunc)dlsym(handle, "register_plugin");
    if (!registerFunc) {
        std::cerr << "❌ [PluginManager] Plugin SO/DYLIB '" << path << "' is missing the 'register_plugin' entry symbol!" << std::endl;
        dlclose(handle);
        return false;
    }
#endif

    // Invoke entry point to let plugin register its schedulers
    try {
        registerFunc(SchedulerRegistry::getInstance());
        loadedHandles.push_back(handle);
        std::cout << "🚀 [PluginManager] Plugin successfully loaded and registered!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ [PluginManager] Plugin threw exception during registration: " << e.what() << std::endl;
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        return false;
    }
}

void PluginManager::unloadAll() {
    for (auto handle : loadedHandles) {
        if (handle) {
#ifdef _WIN32
            FreeLibrary(handle);
#else
            dlclose(handle);
#endif
        }
    }
    loadedHandles.clear();
}
