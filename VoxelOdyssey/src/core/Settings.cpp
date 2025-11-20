#include "Settings.h"
#include "Camera.h"
#include <fstream>
#include <sstream>
#include <algorithm>

Settings::Settings() {
    initializeDefaultKeybinds();
}

Settings::~Settings() {
}

void Settings::initializeDefaultKeybinds() {
    m_keybinds[KeybindAction::MOVE_FORWARD] = GLFW_KEY_W;
    m_keybinds[KeybindAction::MOVE_BACKWARD] = GLFW_KEY_S;
    m_keybinds[KeybindAction::MOVE_LEFT] = GLFW_KEY_A;
    m_keybinds[KeybindAction::MOVE_RIGHT] = GLFW_KEY_D;
    m_keybinds[KeybindAction::MOVE_UP] = GLFW_KEY_SPACE;
    m_keybinds[KeybindAction::MOVE_DOWN] = GLFW_KEY_LEFT_SHIFT;
    m_keybinds[KeybindAction::TOGGLE_INVENTORY] = GLFW_KEY_E;
    m_keybinds[KeybindAction::TOGGLE_PAUSE] = GLFW_KEY_ESCAPE;
    m_keybinds[KeybindAction::TOGGLE_MOUSE_LOCK] = GLFW_KEY_ESCAPE;
    m_keybinds[KeybindAction::HOTBAR_1] = GLFW_KEY_1;
    m_keybinds[KeybindAction::HOTBAR_2] = GLFW_KEY_2;
    m_keybinds[KeybindAction::HOTBAR_3] = GLFW_KEY_3;
    m_keybinds[KeybindAction::HOTBAR_4] = GLFW_KEY_4;
    m_keybinds[KeybindAction::HOTBAR_5] = GLFW_KEY_5;
    m_keybinds[KeybindAction::HOTBAR_6] = GLFW_KEY_6;
    m_keybinds[KeybindAction::HOTBAR_7] = GLFW_KEY_7;
    m_keybinds[KeybindAction::HOTBAR_8] = GLFW_KEY_8;
    m_keybinds[KeybindAction::HOTBAR_9] = GLFW_KEY_9;
}

int Settings::getKeybind(KeybindAction action) const {
    auto it = m_keybinds.find(action);
    if (it != m_keybinds.end()) {
        return it->second;
    }
    return GLFW_KEY_UNKNOWN;
}

void Settings::setKeybind(KeybindAction action, int key) {
    m_keybinds[action] = key;
}

std::string Settings::getKeybindName(KeybindAction action) const {
    return keybindActionToString(action);
}

std::string Settings::getKeyName(int key) const {
    // Convert GLFW key code to readable name
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        return std::string(1, 'A' + (key - GLFW_KEY_A));
    }
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
        return std::string(1, '0' + (key - GLFW_KEY_0));
    }
    
    switch (key) {
        case GLFW_KEY_SPACE: return "Space";
        case GLFW_KEY_LEFT_SHIFT: return "Left Shift";
        case GLFW_KEY_RIGHT_SHIFT: return "Right Shift";
        case GLFW_KEY_LEFT_CONTROL: return "Left Ctrl";
        case GLFW_KEY_RIGHT_CONTROL: return "Right Ctrl";
        case GLFW_KEY_LEFT_ALT: return "Left Alt";
        case GLFW_KEY_RIGHT_ALT: return "Right Alt";
        case GLFW_KEY_ESCAPE: return "ESC";
        case GLFW_KEY_ENTER: return "Enter";
        case GLFW_KEY_TAB: return "Tab";
        case GLFW_KEY_BACKSPACE: return "Backspace";
        case GLFW_KEY_INSERT: return "Insert";
        case GLFW_KEY_DELETE: return "Delete";
        case GLFW_KEY_RIGHT: return "Right Arrow";
        case GLFW_KEY_LEFT: return "Left Arrow";
        case GLFW_KEY_DOWN: return "Down Arrow";
        case GLFW_KEY_UP: return "Up Arrow";
        case GLFW_KEY_PAGE_UP: return "Page Up";
        case GLFW_KEY_PAGE_DOWN: return "Page Down";
        case GLFW_KEY_HOME: return "Home";
        case GLFW_KEY_END: return "End";
        default: return "Unknown";
    }
}

std::string Settings::keybindActionToString(KeybindAction action) const {
    switch (action) {
        case KeybindAction::MOVE_FORWARD: return "Move Forward";
        case KeybindAction::MOVE_BACKWARD: return "Move Backward";
        case KeybindAction::MOVE_LEFT: return "Move Left";
        case KeybindAction::MOVE_RIGHT: return "Move Right";
        case KeybindAction::MOVE_UP: return "Move Up";
        case KeybindAction::MOVE_DOWN: return "Move Down";
        case KeybindAction::TOGGLE_INVENTORY: return "Toggle Inventory";
        case KeybindAction::TOGGLE_PAUSE: return "Toggle Pause";
        case KeybindAction::TOGGLE_MOUSE_LOCK: return "Toggle Mouse Lock";
        case KeybindAction::HOTBAR_1: return "Hotbar Slot 1";
        case KeybindAction::HOTBAR_2: return "Hotbar Slot 2";
        case KeybindAction::HOTBAR_3: return "Hotbar Slot 3";
        case KeybindAction::HOTBAR_4: return "Hotbar Slot 4";
        case KeybindAction::HOTBAR_5: return "Hotbar Slot 5";
        case KeybindAction::HOTBAR_6: return "Hotbar Slot 6";
        case KeybindAction::HOTBAR_7: return "Hotbar Slot 7";
        case KeybindAction::HOTBAR_8: return "Hotbar Slot 8";
        case KeybindAction::HOTBAR_9: return "Hotbar Slot 9";
        default: return "Unknown";
    }
}

KeybindAction Settings::stringToKeybindAction(const std::string& str) const {
    if (str == "Move Forward") return KeybindAction::MOVE_FORWARD;
    if (str == "Move Backward") return KeybindAction::MOVE_BACKWARD;
    if (str == "Move Left") return KeybindAction::MOVE_LEFT;
    if (str == "Move Right") return KeybindAction::MOVE_RIGHT;
    if (str == "Move Up") return KeybindAction::MOVE_UP;
    if (str == "Move Down") return KeybindAction::MOVE_DOWN;
    if (str == "Toggle Inventory") return KeybindAction::TOGGLE_INVENTORY;
    if (str == "Toggle Pause") return KeybindAction::TOGGLE_PAUSE;
    if (str == "Toggle Mouse Lock") return KeybindAction::TOGGLE_MOUSE_LOCK;
    if (str == "Hotbar Slot 1") return KeybindAction::HOTBAR_1;
    if (str == "Hotbar Slot 2") return KeybindAction::HOTBAR_2;
    if (str == "Hotbar Slot 3") return KeybindAction::HOTBAR_3;
    if (str == "Hotbar Slot 4") return KeybindAction::HOTBAR_4;
    if (str == "Hotbar Slot 5") return KeybindAction::HOTBAR_5;
    if (str == "Hotbar Slot 6") return KeybindAction::HOTBAR_6;
    if (str == "Hotbar Slot 7") return KeybindAction::HOTBAR_7;
    if (str == "Hotbar Slot 8") return KeybindAction::HOTBAR_8;
    if (str == "Hotbar Slot 9") return KeybindAction::HOTBAR_9;
    return KeybindAction::COUNT;
}

bool Settings::loadFromFile(const std::string& filename) {
    // Simple text-based format for now (could use JSON library later)
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse keybinds
            if (key.find("keybind_") == 0) {
                std::string actionStr = key.substr(8);
                KeybindAction action = stringToKeybindAction(actionStr);
                if (action != KeybindAction::COUNT) {
                    int keyCode = std::stoi(value);
                    setKeybind(action, keyCode);
                }
            }
            // Parse visual settings
            else if (key == "renderDistance") {
                m_visualSettings.renderDistance = std::stoi(value);
            }
            else if (key == "fov") {
                m_visualSettings.fov = std::stof(value);
            }
            else if (key == "mouseSensitivity") {
                m_visualSettings.mouseSensitivity = std::stof(value);
            }
            else if (key == "movementSpeed") {
                m_visualSettings.movementSpeed = std::stof(value);
            }
            else if (key == "enableFrustumCulling") {
                m_visualSettings.enableFrustumCulling = (value == "true" || value == "1");
            }
            else if (key == "enableAmbientOcclusion") {
                m_visualSettings.enableAmbientOcclusion = (value == "true" || value == "1");
            }
        }
    }
    
    file.close();
    return true;
}

bool Settings::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Voxel Odyssey Settings File\n";
    file << "# Keybinds\n";
    for (const auto& pair : m_keybinds) {
        file << "keybind_" << keybindActionToString(pair.first) << "=" << pair.second << "\n";
    }
    
    file << "\n# Visual Settings\n";
    file << "renderDistance=" << m_visualSettings.renderDistance << "\n";
    file << "fov=" << m_visualSettings.fov << "\n";
    file << "mouseSensitivity=" << m_visualSettings.mouseSensitivity << "\n";
    file << "movementSpeed=" << m_visualSettings.movementSpeed << "\n";
    file << "enableFrustumCulling=" << (m_visualSettings.enableFrustumCulling ? "true" : "false") << "\n";
    file << "enableAmbientOcclusion=" << (m_visualSettings.enableAmbientOcclusion ? "true" : "false") << "\n";
    
    file.close();
    return true;
}

void Settings::applySettings(Camera& camera, class World& world) const {
    camera.setMovementSpeed(m_visualSettings.movementSpeed);
    camera.setMouseSensitivity(m_visualSettings.mouseSensitivity);
    camera.setZoom(m_visualSettings.fov);
    // Note: Render distance would need to be set in World class
    // This is a placeholder - World would need a setRenderDistance method
    // For now, render distance is handled in World's constructor/update
}

