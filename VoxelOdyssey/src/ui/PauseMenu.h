#pragma once
#include "core/Settings.h"
#include "renderer/Shader.h"
#include <glm/glm.hpp>

enum class PauseMenuState {
    MAIN,
    SETTINGS,
    KEYBINDS,
    VISUAL
};

class PauseMenu {
public:
    static void init();
    static void cleanup();
    
    static void render(Shader& shader, Settings& settings, int windowWidth, int windowHeight);
    static void update(Settings& settings, float deltaTime);
    
    static bool isOpen() { return s_isOpen; }
    static void setOpen(bool open) { s_isOpen = open; }
    static void toggle() { s_isOpen = !s_isOpen; }
    
    static PauseMenuState getState() { return s_state; }
    static void setState(PauseMenuState state) { s_state = state; }
    
    // Keybind editing
    static KeybindAction getEditingKeybind() { return s_editingKeybind; }
    static void setEditingKeybind(KeybindAction action) { s_editingKeybind = action; }
    static bool isEditingKeybind() { return s_editingKeybind != KeybindAction::COUNT; }
    static void cancelKeybindEdit() { s_editingKeybind = KeybindAction::COUNT; }
    
private:
    static bool s_initialized;
    static bool s_isOpen;
    static PauseMenuState s_state;
    static KeybindAction s_editingKeybind;
    static unsigned int s_quadVAO, s_quadVBO;
    
    static void setupQuadVAO();
    static void renderMainMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight);
    static void renderSettingsMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight);
    static void renderKeybindsMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight);
    static void renderVisualMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight);
    static void renderButton(Shader& shader, float x, float y, float width, float height, 
                            const glm::vec3& color, const glm::mat4& projection);
    static void renderText(Shader& shader, const std::string& text, float x, float y, 
                          float scale, const glm::mat4& projection);
    static bool isMouseOverButton(float mouseX, float mouseY, float btnX, float btnY, 
                                 float btnWidth, float btnHeight, int windowWidth, int windowHeight);
};

