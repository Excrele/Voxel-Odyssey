#include "PauseMenu.h"
#include "core/Input.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iomanip>

bool PauseMenu::s_initialized = false;
bool PauseMenu::s_isOpen = false;
PauseMenuState PauseMenu::s_state = PauseMenuState::MAIN;
KeybindAction PauseMenu::s_editingKeybind = KeybindAction::COUNT;
unsigned int PauseMenu::s_quadVAO = 0;
unsigned int PauseMenu::s_quadVBO = 0;

void PauseMenu::setupQuadVAO() {
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

void PauseMenu::init() {
    if (s_initialized) return;
    setupQuadVAO();
    s_initialized = true;
}

void PauseMenu::cleanup() {
    if (!s_initialized) return;
    
    glDeleteVertexArrays(1, &s_quadVAO);
    glDeleteBuffers(1, &s_quadVBO);
    
    s_quadVAO = 0;
    s_quadVBO = 0;
    s_initialized = false;
}

void PauseMenu::update(Settings& settings, float deltaTime) {
    if (!s_isOpen) return;
    
    // Handle mouse clicks for menu navigation
    glm::vec2 mousePos = Input::getMousePosition();
    int windowWidth = 1280; // Would get from window
    int windowHeight = 720;
    
    // Convert mouse position (screen coordinates)
    float mouseX = mousePos.x;
    float mouseY = mousePos.y;
    
    // Handle keybind editing
    if (isEditingKeybind()) {
        // Check for any key press
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
            if (Input::isKeyJustPressed(key)) {
                settings.setKeybind(s_editingKeybind, key);
                s_editingKeybind = KeybindAction::COUNT;
                break;
            }
        }
    }
}

void PauseMenu::render(Shader& shader, Settings& settings, int windowWidth, int windowHeight) {
    if (!s_isOpen) return;
    if (!s_initialized) init();
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    shader.use();
    glDisable(GL_DEPTH_TEST);
    
    // Render semi-transparent background overlay
    shader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(windowWidth, windowHeight, 1.0f));
    shader.setMat4("model", model);
    shader.setMat4("view", glm::mat4(1.0f));
    shader.setMat4("projection", projection);
    
    glBindVertexArray(s_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Render menu based on state
    switch (s_state) {
        case PauseMenuState::MAIN:
            renderMainMenu(shader, settings, windowWidth, windowHeight);
            break;
        case PauseMenuState::SETTINGS:
            renderSettingsMenu(shader, settings, windowWidth, windowHeight);
            break;
        case PauseMenuState::KEYBINDS:
            renderKeybindsMenu(shader, settings, windowWidth, windowHeight);
            break;
        case PauseMenuState::VISUAL:
            renderVisualMenu(shader, settings, windowWidth, windowHeight);
            break;
    }
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void PauseMenu::renderButton(Shader& shader, float x, float y, float width, float height,
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

void PauseMenu::renderText(Shader& shader, const std::string& text, float x, float y,
                          float scale, const glm::mat4& projection) {
    // Simple text rendering placeholder
    // In a real implementation, you'd use a text rendering library
    // For now, we'll just render a small quad as a placeholder
    // This would be replaced with actual text rendering
}

bool PauseMenu::isMouseOverButton(float mouseX, float mouseY, float btnX, float btnY,
                                  float btnWidth, float btnHeight, int windowWidth, int windowHeight) {
    // Convert screen coordinates (mouseY is from top, OpenGL is from bottom)
    float glY = windowHeight - mouseY;
    return mouseX >= btnX && mouseX <= btnX + btnWidth &&
           glY >= btnY && glY <= btnY + btnHeight;
}

void PauseMenu::renderMainMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float btnWidth = 300.0f;
    float btnHeight = 50.0f;
    float spacing = 60.0f;
    
    // Title
    // renderText(shader, "PAUSED", centerX - 100, centerY - 200, 2.0f, projection);
    
    // Resume button
    glm::vec3 btnColor(0.3f, 0.3f, 0.3f);
    renderButton(shader, centerX - btnWidth/2, centerY - spacing*2, btnWidth, btnHeight, btnColor, 
                glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f));
    
    // Settings button
    renderButton(shader, centerX - btnWidth/2, centerY - spacing, btnWidth, btnHeight, btnColor,
                glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f));
    
    // Keybinds button
    renderButton(shader, centerX - btnWidth/2, centerY, btnWidth, btnHeight, btnColor,
                glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f));
    
    // Visual Settings button
    renderButton(shader, centerX - btnWidth/2, centerY + spacing, btnWidth, btnHeight, btnColor,
                glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f));
}

void PauseMenu::renderSettingsMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight) {
    // Settings submenu - would show various settings
    renderMainMenu(shader, settings, windowWidth, windowHeight); // Placeholder
}

void PauseMenu::renderKeybindsMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float startY = 100.0f;
    float btnWidth = 200.0f;
    float btnHeight = 40.0f;
    float spacing = 50.0f;
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    // Render each keybind
    int index = 0;
    for (int i = 0; i < (int)KeybindAction::COUNT; i++) {
        KeybindAction action = (KeybindAction)i;
        float y = startY + index * spacing;
        
        // Action name (left side)
        glm::vec3 textColor(1.0f, 1.0f, 1.0f);
        shader.setVec3("color", textColor);
        // renderText would go here
        
        // Key name (right side) - editable
        float keyX = centerX + 150.0f;
        glm::vec3 keyColor = (s_editingKeybind == action) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.5f, 0.5f, 0.5f);
        renderButton(shader, keyX, y, btnWidth, btnHeight, keyColor, projection);
        
        index++;
    }
    
    // Back button
    float backY = windowHeight - 80.0f;
    renderButton(shader, 50.0f, backY, 150.0f, 40.0f, glm::vec3(0.3f, 0.3f, 0.3f), projection);
}

void PauseMenu::renderVisualMenu(Shader& shader, Settings& settings, int windowWidth, int windowHeight) {
    float centerX = windowWidth / 2.0f;
    float startY = 100.0f;
    float spacing = 60.0f;
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    VisualSettings& visual = settings.getVisualSettings();
    
    // Render Distance
    // renderText(shader, "Render Distance: " + std::to_string(visual.renderDistance), centerX - 200, startY, 1.0f, projection);
    
    // FOV
    // renderText(shader, "FOV: " + std::to_string((int)visual.fov), centerX - 200, startY + spacing, 1.0f, projection);
    
    // Mouse Sensitivity
    // renderText(shader, "Mouse Sensitivity: " + std::to_string(visual.mouseSensitivity), centerX - 200, startY + spacing*2, 1.0f, projection);
    
    // Movement Speed
    // renderText(shader, "Movement Speed: " + std::to_string(visual.movementSpeed), centerX - 200, startY + spacing*3, 1.0f, projection);
    
    // Toggle options
    glm::vec3 toggleColor = visual.enableFrustumCulling ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
    renderButton(shader, centerX + 100, startY + spacing*4, 50.0f, 30.0f, toggleColor, projection);
    
    // Back button
    float backY = windowHeight - 80.0f;
    renderButton(shader, 50.0f, backY, 150.0f, 40.0f, glm::vec3(0.3f, 0.3f, 0.3f), projection);
}

