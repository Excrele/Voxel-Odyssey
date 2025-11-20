#include "MainMenu.h"
#include "world/WorldManager.h"
#include "core/Input.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

bool MainMenu::s_initialized = false;
bool MainMenu::s_isOpen = true;
MainMenuState MainMenu::s_state = MainMenuState::MAIN;
int MainMenu::s_selectedWorldIndex = 0;
std::string MainMenu::s_newWorldName = "";
bool MainMenu::s_creatingWorld = false;
unsigned int MainMenu::s_quadVAO = 0;
unsigned int MainMenu::s_quadVBO = 0;

void MainMenu::setupQuadVAO() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    unsigned int EBO;
    glGenVertexArrays(1, &s_quadVAO);
    glGenBuffers(1, &s_quadVBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(s_quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glDeleteBuffers(1, &EBO);
}

void MainMenu::init() {
    if (s_initialized) return;
    setupQuadVAO();
    s_initialized = true;
}

void MainMenu::cleanup() {
    if (!s_initialized) return;
    
    glDeleteVertexArrays(1, &s_quadVAO);
    glDeleteBuffers(1, &s_quadVBO);
    
    s_quadVAO = 0;
    s_quadVBO = 0;
    s_initialized = false;
}

void MainMenu::update(float deltaTime) {
    if (!s_isOpen) return;
    
    // Handle keyboard input for menu navigation
    // ESC goes back
    if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE)) {
        if (s_state == MainMenuState::SINGLEPLAYER) {
            s_state = MainMenuState::MAIN;
        } else if (s_state == MainMenuState::NEW_WORLD || s_state == MainMenuState::SELECT_WORLD) {
            s_state = MainMenuState::SINGLEPLAYER;
        }
    }
    
    // Handle world name input (simple text input)
    if (s_state == MainMenuState::NEW_WORLD) {
        // Simple text input - in production would use proper text input system
        // For now, allow typing letters/numbers
        for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++) {
            if (Input::isKeyJustPressed(key)) {
                s_newWorldName += (char)('a' + (key - GLFW_KEY_A));
            }
        }
        for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
            if (Input::isKeyJustPressed(key)) {
                s_newWorldName += (char)('0' + (key - GLFW_KEY_0));
            }
        }
        if (Input::isKeyJustPressed(GLFW_KEY_SPACE)) {
            s_newWorldName += " ";
        }
        if (Input::isKeyJustPressed(GLFW_KEY_BACKSPACE) && !s_newWorldName.empty()) {
            s_newWorldName.pop_back();
        }
        if (Input::isKeyJustPressed(GLFW_KEY_ENTER) && !s_newWorldName.empty()) {
            s_creatingWorld = true;
        }
    }
    
    // Handle world selection navigation
    if (s_state == MainMenuState::SELECT_WORLD) {
        auto worlds = WorldManager::getInstance().getAvailableWorlds();
        if (Input::isKeyJustPressed(GLFW_KEY_UP) && s_selectedWorldIndex > 0) {
            s_selectedWorldIndex--;
        }
        if (Input::isKeyJustPressed(GLFW_KEY_DOWN) && s_selectedWorldIndex < (int)worlds.size() - 1) {
            s_selectedWorldIndex++;
        }
    }
}

void MainMenu::render(Shader& shader, int windowWidth, int windowHeight) {
    if (!s_isOpen) return;
    if (!s_initialized) init();
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    shader.use();
    glDisable(GL_DEPTH_TEST);
    
    // Render background (dark overlay)
    shader.setVec3("color", glm::vec3(0.1f, 0.1f, 0.15f));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(windowWidth, windowHeight, 1.0f));
    shader.setMat4("model", model);
    shader.setMat4("view", glm::mat4(1.0f));
    shader.setMat4("projection", projection);
    
    glBindVertexArray(s_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Render menu based on state
    switch (s_state) {
        case MainMenuState::MAIN:
            renderMainScreen(shader, windowWidth, windowHeight);
            break;
        case MainMenuState::SINGLEPLAYER:
            renderSingleplayerScreen(shader, windowWidth, windowHeight);
            break;
        case MainMenuState::NEW_WORLD:
            renderNewWorldScreen(shader, windowWidth, windowHeight);
            break;
        case MainMenuState::SELECT_WORLD:
            renderSelectWorldScreen(shader, windowWidth, windowHeight);
            break;
        default:
            break;
    }
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void MainMenu::renderButton(Shader& shader, float x, float y, float width, float height,
                           const glm::vec3& color, const glm::mat4& projection) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    shader.setMat4("model", model);
    shader.setMat4("view", glm::mat4(1.0f));
    shader.setMat4("projection", projection);
    shader.setVec3("color", color);
    
    glBindVertexArray(s_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool MainMenu::isMouseOverButton(float mouseX, float mouseY, float btnX, float btnY,
                                float btnWidth, float btnHeight, int windowWidth, int windowHeight) {
    float glY = windowHeight - mouseY;
    return mouseX >= btnX && mouseX <= btnX + btnWidth &&
           glY >= btnY && glY <= btnY + btnHeight;
}

void MainMenu::renderMainScreen(Shader& shader, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float btnWidth = 300.0f;
    float btnHeight = 60.0f;
    float spacing = 80.0f;
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    // Title (placeholder - would render text)
    // renderText("Voxel Odyssey", centerX - 200, 100, 3.0f);
    
    // Singleplayer button
    renderButton(shader, centerX - btnWidth/2, centerY - spacing, btnWidth, btnHeight,
                glm::vec3(0.2f, 0.4f, 0.6f), projection);
    
    // Settings button
    renderButton(shader, centerX - btnWidth/2, centerY, btnWidth, btnHeight,
                glm::vec3(0.3f, 0.3f, 0.3f), projection);
    
    // Quit button
    renderButton(shader, centerX - btnWidth/2, centerY + spacing, btnWidth, btnHeight,
                glm::vec3(0.6f, 0.2f, 0.2f), projection);
}

void MainMenu::renderSingleplayerScreen(Shader& shader, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float btnWidth = 300.0f;
    float btnHeight = 60.0f;
    float spacing = 80.0f;
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    // Create New World button
    renderButton(shader, centerX - btnWidth/2, centerY - spacing, btnWidth, btnHeight,
                glm::vec3(0.2f, 0.6f, 0.2f), projection);
    
    // Select World button
    renderButton(shader, centerX - btnWidth/2, centerY, btnWidth, btnHeight,
                glm::vec3(0.2f, 0.4f, 0.6f), projection);
    
    // Back button
    renderButton(shader, centerX - btnWidth/2, centerY + spacing, btnWidth, btnHeight,
                glm::vec3(0.3f, 0.3f, 0.3f), projection);
}

void MainMenu::renderNewWorldScreen(Shader& shader, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float btnWidth = 400.0f;
    float btnHeight = 50.0f;
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    // World name input box (placeholder)
    renderButton(shader, centerX - btnWidth/2, centerY - 100, btnWidth, btnHeight,
                glm::vec3(0.2f, 0.2f, 0.2f), projection);
    
    // Create World button
    renderButton(shader, centerX - btnWidth/2, centerY, btnWidth, btnHeight,
                glm::vec3(0.2f, 0.6f, 0.2f), projection);
    
    // Back button
    renderButton(shader, centerX - btnWidth/2, centerY + 80, btnWidth, btnHeight,
                glm::vec3(0.3f, 0.3f, 0.3f), projection);
}

void MainMenu::renderSelectWorldScreen(Shader& shader, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float startY = 150.0f;
    float btnWidth = 600.0f;
    float btnHeight = 60.0f;
    float spacing = 70.0f;
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    // Get available worlds
    auto worlds = WorldManager::getInstance().getAvailableWorlds();
    
    // Render world list
    for (size_t i = 0; i < worlds.size() && i < 5; i++) {
        float y = startY + i * spacing;
        glm::vec3 color = (i == s_selectedWorldIndex) ? glm::vec3(0.3f, 0.5f, 0.7f) : glm::vec3(0.2f, 0.3f, 0.4f);
        renderButton(shader, centerX - btnWidth/2, y, btnWidth, btnHeight, color, projection);
    }
    
    // Play Selected World button
    if (!worlds.empty() && s_selectedWorldIndex >= 0 && s_selectedWorldIndex < (int)worlds.size()) {
        renderButton(shader, centerX - btnWidth/2, startY + 5 * spacing, btnWidth, btnHeight,
                    glm::vec3(0.2f, 0.6f, 0.2f), projection);
    }
    
    // Back button
    renderButton(shader, 50.0f, windowHeight - 80.0f, 150.0f, 50.0f,
                glm::vec3(0.3f, 0.3f, 0.3f), projection);
}

