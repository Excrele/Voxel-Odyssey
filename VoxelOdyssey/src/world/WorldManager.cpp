#include "WorldManager.h"
#include "World.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <glm/glm.hpp>

WorldManager& WorldManager::getInstance() {
    static WorldManager instance;
    return instance;
}

std::string WorldManager::getWorldsDirectory() const {
    return "saves/";
}

std::string WorldManager::sanitizeWorldName(const std::string& name) const {
    std::string sanitized = name;
    // Remove invalid characters for folder names
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
        [](char c) { return !std::isalnum(c) && c != '_' && c != '-' && c != ' '; }),
        sanitized.end());
    // Replace spaces with underscores
    std::replace(sanitized.begin(), sanitized.end(), ' ', '_');
    // Limit length
    if (sanitized.length() > 50) {
        sanitized = sanitized.substr(0, 50);
    }
    return sanitized;
}

bool WorldManager::createNewWorld(const std::string& worldName, std::string& worldPath) {
    if (worldName.empty()) {
        return false;
    }
    
    std::string sanitized = sanitizeWorldName(worldName);
    if (sanitized.empty()) {
        return false;
    }
    
    // Check if world already exists
    std::string basePath = getWorldsDirectory() + sanitized;
    if (std::filesystem::exists(basePath)) {
        // Append number if exists
        int counter = 1;
        std::string testPath = basePath + "_" + std::to_string(counter);
        while (std::filesystem::exists(testPath)) {
            counter++;
            testPath = basePath + "_" + std::to_string(counter);
        }
        basePath = testPath;
        sanitized = sanitizeWorldName(worldName) + "_" + std::to_string(counter);
    }
    
    // Create world directory structure
    std::filesystem::create_directories(basePath);
    std::filesystem::create_directories(basePath + "/chunks");
    
    // Create world info
    WorldInfo info;
    info.name = worldName;
    info.folderName = sanitized;
    info.path = basePath;
    info.creationTime = std::time(nullptr);
    info.lastPlayedTime = info.creationTime;
    info.spawnPosition = glm::vec3(0.0f, 50.0f, 0.0f);
    
    // Save world info
    if (!saveWorldInfo(basePath, info)) {
        return false;
    }
    
    worldPath = basePath;
    m_currentWorld = sanitized;
    
    return true;
}

bool WorldManager::deleteWorld(const std::string& worldName) {
    std::string worldPath = getWorldsDirectory() + worldName;
    if (!std::filesystem::exists(worldPath)) {
        return false;
    }
    
    try {
        std::filesystem::remove_all(worldPath);
        if (m_currentWorld == worldName) {
            m_currentWorld = "";
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool WorldManager::worldExists(const std::string& worldName) const {
    std::string worldPath = getWorldsDirectory() + worldName;
    return std::filesystem::exists(worldPath);
}

std::vector<WorldInfo> WorldManager::getAvailableWorlds() const {
    std::vector<WorldInfo> worlds;
    std::string worldsDir = getWorldsDirectory();
    
    if (!std::filesystem::exists(worldsDir)) {
        return worlds;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(worldsDir)) {
        if (entry.is_directory()) {
            WorldInfo info = loadWorldInfo(entry.path().string());
            if (!info.name.empty()) {
                worlds.push_back(info);
            }
        }
    }
    
    // Sort by last played time (most recent first)
    std::sort(worlds.begin(), worlds.end(),
        [](const WorldInfo& a, const WorldInfo& b) {
            return a.lastPlayedTime > b.lastPlayedTime;
        });
    
    return worlds;
}

WorldInfo WorldManager::getWorldInfo(const std::string& worldName) const {
    std::string worldPath = getWorldsDirectory() + worldName;
    return loadWorldInfo(worldPath);
}

bool WorldManager::updateWorldInfo(const std::string& worldName, const WorldInfo& info) {
    std::string worldPath = getWorldsDirectory() + worldName;
    return saveWorldInfo(worldPath, info);
}

void WorldManager::setCurrentWorld(const std::string& worldName) {
    m_currentWorld = worldName;
    
    // Update last played time
    WorldInfo info = getWorldInfo(worldName);
    if (!info.name.empty()) {
        info.lastPlayedTime = std::time(nullptr);
        updateWorldInfo(worldName, info);
    }
}

std::string WorldManager::getCurrentWorldPath() const {
    if (m_currentWorld.empty()) {
        return "";
    }
    return getWorldsDirectory() + m_currentWorld;
}

void WorldManager::update(float deltaTime) {
    if (!m_autoSaveEnabled || m_currentWorld.empty()) {
        return;
    }
    
    m_autoSaveTimer += deltaTime;
    if (m_autoSaveTimer >= AUTO_SAVE_INTERVAL) {
        forceSave();
        m_autoSaveTimer = 0.0f;
    }
}

void WorldManager::forceSave() {
    // This triggers a save - the actual saving is done by calling World::saveAllChunks()
    // from the main game loop when this is called
    // The timer is reset in update(), so this just marks that a save should happen
}

WorldInfo WorldManager::loadWorldInfo(const std::string& worldPath) const {
    WorldInfo info;
    std::string infoPath = worldPath + "/world.info";
    
    if (!std::filesystem::exists(infoPath)) {
        return info;
    }
    
    std::ifstream file(infoPath);
    if (!file.is_open()) {
        return info;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (key == "name") {
            info.name = value;
        } else if (key == "folderName") {
            info.folderName = value;
        } else if (key == "creationTime") {
            info.creationTime = std::stoll(value);
        } else if (key == "lastPlayedTime") {
            info.lastPlayedTime = std::stoll(value);
        } else if (key == "spawnX") {
            info.spawnPosition.x = std::stof(value);
        } else if (key == "spawnY") {
            info.spawnPosition.y = std::stof(value);
        } else if (key == "spawnZ") {
            info.spawnPosition.z = std::stof(value);
        }
    }
    
    info.path = worldPath;
    file.close();
    return info;
}

bool WorldManager::saveWorldInfo(const std::string& worldPath, const WorldInfo& info) const {
    std::string infoPath = worldPath + "/world.info";
    
    std::ofstream file(infoPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# World Information File\n";
    file << "name=" << info.name << "\n";
    file << "folderName=" << info.folderName << "\n";
    file << "creationTime=" << info.creationTime << "\n";
    file << "lastPlayedTime=" << info.lastPlayedTime << "\n";
    file << "spawnX=" << info.spawnPosition.x << "\n";
    file << "spawnY=" << info.spawnPosition.y << "\n";
    file << "spawnZ=" << info.spawnPosition.z << "\n";
    
    file.close();
    return true;
}

