#pragma once
#include <string>
#include <map>
#include <GLFW/glfw3.h>

// Keybind action types
enum class KeybindAction {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    TOGGLE_INVENTORY,
    TOGGLE_PAUSE,
    TOGGLE_MOUSE_LOCK,
    HOTBAR_1,
    HOTBAR_2,
    HOTBAR_3,
    HOTBAR_4,
    HOTBAR_5,
    HOTBAR_6,
    HOTBAR_7,
    HOTBAR_8,
    HOTBAR_9,
    COUNT
};

// Visual quality settings
struct VisualSettings {
    int renderDistance = 4;        // Chunk render distance
    bool enableFrustumCulling = true;
    bool enableAmbientOcclusion = true;
    float fov = 45.0f;             // Field of view
    float mouseSensitivity = 0.1f;
    float movementSpeed = 10.0f;
};

class Settings {
public:
    Settings();
    ~Settings();
    
    // Keybind management
    int getKeybind(KeybindAction action) const;
    void setKeybind(KeybindAction action, int key);
    std::string getKeybindName(KeybindAction action) const;
    std::string getKeyName(int key) const;
    
    // Visual settings
    VisualSettings& getVisualSettings() { return m_visualSettings; }
    const VisualSettings& getVisualSettings() const { return m_visualSettings; }
    void setVisualSettings(const VisualSettings& settings) { m_visualSettings = settings; }
    
    // Save/Load
    bool loadFromFile(const std::string& filename = "settings.json");
    bool saveToFile(const std::string& filename = "settings.json") const;
    
    // Apply settings to game systems
    void applySettings(class Camera& camera, class World& world) const;
    
private:
    std::map<KeybindAction, int> m_keybinds;
    VisualSettings m_visualSettings;
    
    void initializeDefaultKeybinds();
    std::string keybindActionToString(KeybindAction action) const;
    KeybindAction stringToKeybindAction(const std::string& str) const;
};

