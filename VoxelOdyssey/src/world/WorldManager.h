#pragma once
#include <string>
#include <vector>
#include <ctime>

struct WorldInfo {
    std::string name;
    std::string folderName;
    std::string path;
    time_t creationTime;
    time_t lastPlayedTime;
    glm::vec3 spawnPosition;
    
    WorldInfo() : creationTime(0), lastPlayedTime(0), spawnPosition(0, 50, 0) {}
};

class WorldManager {
public:
    static WorldManager& getInstance();
    
    // World management
    bool createNewWorld(const std::string& worldName, std::string& worldPath);
    bool deleteWorld(const std::string& worldName);
    bool worldExists(const std::string& worldName) const;
    
    // World info
    std::vector<WorldInfo> getAvailableWorlds() const;
    WorldInfo getWorldInfo(const std::string& worldName) const;
    bool updateWorldInfo(const std::string& worldName, const WorldInfo& info);
    
    // Current world
    void setCurrentWorld(const std::string& worldName);
    std::string getCurrentWorld() const { return m_currentWorld; }
    std::string getCurrentWorldPath() const;
    
    // Auto-save
    void update(float deltaTime);
    void forceSave();
    void setAutoSaveEnabled(bool enabled) { m_autoSaveEnabled = enabled; }
    bool isAutoSaveEnabled() const { return m_autoSaveEnabled; }
    
private:
    WorldManager() : m_currentWorld(""), m_autoSaveEnabled(true), m_autoSaveTimer(0.0f) {}
    ~WorldManager() = default;
    WorldManager(const WorldManager&) = delete;
    WorldManager& operator=(const WorldManager&) = delete;
    
    std::string m_currentWorld;
    bool m_autoSaveEnabled;
    float m_autoSaveTimer;
    static constexpr float AUTO_SAVE_INTERVAL = 60.0f; // 1 minute
    
    std::string getWorldsDirectory() const;
    std::string sanitizeWorldName(const std::string& name) const;
    WorldInfo loadWorldInfo(const std::string& worldPath) const;
    bool saveWorldInfo(const std::string& worldPath, const WorldInfo& info) const;
};

