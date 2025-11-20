#pragma once
#include "renderer/Shader.h"
#include <glm/glm.hpp>
#include <string>

enum class MainMenuState {
    MAIN,
    SINGLEPLAYER,
    NEW_WORLD,
    SELECT_WORLD,
    SETTINGS,
    QUIT
};

class MainMenu {
public:
    static void init();
    static void cleanup();
    
    static void render(Shader& shader, int windowWidth, int windowHeight);
    static void update(float deltaTime);
    
    static bool isOpen() { return s_isOpen; }
    static void setOpen(bool open) { s_isOpen = open; }
    
    static MainMenuState getState() { return s_state; }
    static void setState(MainMenuState state) { s_state = state; }
    
    // World selection
    static int getSelectedWorldIndex() { return s_selectedWorldIndex; }
    static void setSelectedWorldIndex(int index) { s_selectedWorldIndex = index; }
    
    // New world
    static std::string getNewWorldName() { return s_newWorldName; }
    static void setNewWorldName(const std::string& name) { s_newWorldName = name; }
    static bool isCreatingWorld() { return s_creatingWorld; }
    static void setCreatingWorld(bool creating) { s_creatingWorld = creating; }
    
private:
    static bool s_initialized;
    static bool s_isOpen;
    static MainMenuState s_state;
    static int s_selectedWorldIndex;
    static std::string s_newWorldName;
    static bool s_creatingWorld;
    static unsigned int s_quadVAO, s_quadVBO;
    
    static void setupQuadVAO();
    static void renderMainScreen(Shader& shader, int windowWidth, int windowHeight);
    static void renderSingleplayerScreen(Shader& shader, int windowWidth, int windowHeight);
    static void renderNewWorldScreen(Shader& shader, int windowWidth, int windowHeight);
    static void renderSelectWorldScreen(Shader& shader, int windowWidth, int windowHeight);
    static void renderButton(Shader& shader, float x, float y, float width, float height,
                            const glm::vec3& color, const glm::mat4& projection);
    static bool isMouseOverButton(float mouseX, float mouseY, float btnX, float btnY,
                                 float btnWidth, float btnHeight, int windowWidth, int windowHeight);
};

