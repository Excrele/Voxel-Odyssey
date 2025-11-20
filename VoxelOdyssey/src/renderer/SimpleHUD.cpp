#include "SimpleHUD.h"
#include "world/Block.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

bool SimpleHUD::s_initialized = false;
unsigned int SimpleHUD::s_lineVAO = 0;
unsigned int SimpleHUD::s_lineVBO = 0;
unsigned int SimpleHUD::s_quadVAO = 0;
unsigned int SimpleHUD::s_quadVBO = 0;

void SimpleHUD::setupLineVAO() {
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    
    glGenVertexArrays(1, &s_lineVAO);
    glGenBuffers(1, &s_lineVBO);
    
    glBindVertexArray(s_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void SimpleHUD::setupQuadVAO() {
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

void SimpleHUD::init() {
    if (s_initialized) return;
    
    setupLineVAO();
    setupQuadVAO();
    
    s_initialized = true;
}

void SimpleHUD::cleanup() {
    if (!s_initialized) return;
    
    glDeleteVertexArrays(1, &s_lineVAO);
    glDeleteBuffers(1, &s_lineVBO);
    glDeleteVertexArrays(1, &s_quadVAO);
    glDeleteBuffers(1, &s_quadVBO);
    
    s_initialized = false;
}

void SimpleHUD::renderCrosshair(Shader& shader, int windowWidth, int windowHeight) {
    if (!s_initialized) init();
    
    // Use orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float size = 15.0f;
    float thickness = 2.0f;
    
    shader.use();
    // Set color uniform if it exists (debug shader has color uniform)
    shader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // White
    
    glDisable(GL_DEPTH_TEST);
    glLineWidth(thickness);
    
    // Horizontal line
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(centerX - size, centerY, 0.0f));
    model = glm::scale(model, glm::vec3(size * 2, thickness, 1.0f));
    
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    glBindVertexArray(s_lineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    
    // Vertical line
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(centerX, centerY - size, 0.0f));
    model = glm::scale(model, glm::vec3(thickness, size * 2, 1.0f));
    
    shader.setMat4("model", model);
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void SimpleHUD::renderBlockIndicator(Shader& shader, int selectedBlock, int windowWidth, int windowHeight) {
    if (!s_initialized) init();
    
    // Render a simple colored box in the bottom-right corner showing selected block
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    float boxSize = 60.0f;
    float margin = 20.0f;
    float x = windowWidth - boxSize - margin;
    float y = windowHeight - boxSize - margin;
    
    // Get color based on block type
    glm::vec3 color(0.5f, 0.5f, 0.5f);
    switch ((BlockType)selectedBlock) {
        case BlockType::GRASS: color = glm::vec3(0.2f, 0.8f, 0.2f); break;
        case BlockType::DIRT: color = glm::vec3(0.6f, 0.4f, 0.2f); break;
        case BlockType::STONE: color = glm::vec3(0.5f, 0.5f, 0.5f); break;
        case BlockType::SAND: color = glm::vec3(0.9f, 0.8f, 0.6f); break;
        case BlockType::WOOD: color = glm::vec3(0.5f, 0.3f, 0.1f); break;
        case BlockType::LEAVES: color = glm::vec3(0.1f, 0.6f, 0.1f); break;
        case BlockType::GRAVEL: color = glm::vec3(0.4f, 0.4f, 0.4f); break;
        default: color = glm::vec3(0.5f, 0.5f, 0.5f); break;
    }
    
    shader.use();
    shader.setVec3("color", color);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(boxSize, boxSize, 1.0f));
    
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    glDisable(GL_DEPTH_TEST);
    
    glBindVertexArray(s_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glEnable(GL_DEPTH_TEST);
}

void SimpleHUD::renderBar(Shader& shader, float value, float maxValue, float x, float y,
                          float width, float height, const glm::vec3& fillColor,
                          const glm::vec3& bgColor, int windowWidth, int windowHeight) {
    if (!s_initialized) init();
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    shader.use();
    glDisable(GL_DEPTH_TEST);
    
    // Render background
    shader.setVec3("color", bgColor);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    glBindVertexArray(s_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Render fill
    float fillWidth = width * (value / maxValue);
    if (fillWidth > 0.0f) {
        shader.setVec3("color", fillColor);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(fillWidth, height, 1.0f));
        
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void SimpleHUD::renderHealthBar(Shader& shader, float health, int windowWidth, int windowHeight) {
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float margin = 20.0f;
    float x = margin;
    float y = windowHeight - margin - barHeight - 60.0f; // Above hunger bar
    
    glm::vec3 fillColor;
    if (health > 60.0f) {
        fillColor = glm::vec3(0.2f, 0.8f, 0.2f); // Green
    } else if (health > 30.0f) {
        fillColor = glm::vec3(0.8f, 0.8f, 0.2f); // Yellow
    } else {
        fillColor = glm::vec3(0.8f, 0.2f, 0.2f); // Red
    }
    
    glm::vec3 bgColor(0.2f, 0.2f, 0.2f); // Dark gray background
    
    renderBar(shader, health, 100.0f, x, y, barWidth, barHeight, fillColor, bgColor, windowWidth, windowHeight);
}

void SimpleHUD::renderHungerBar(Shader& shader, float hunger, int windowWidth, int windowHeight) {
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float margin = 20.0f;
    float x = margin;
    float y = windowHeight - margin - barHeight; // Bottom of screen
    
    glm::vec3 fillColor;
    if (hunger > 60.0f) {
        fillColor = glm::vec3(0.8f, 0.6f, 0.2f); // Orange/brown
    } else if (hunger > 30.0f) {
        fillColor = glm::vec3(0.6f, 0.4f, 0.2f); // Darker orange
    } else {
        fillColor = glm::vec3(0.4f, 0.2f, 0.1f); // Dark brown
    }
    
    glm::vec3 bgColor(0.2f, 0.2f, 0.2f); // Dark gray background
    
    renderBar(shader, hunger, 100.0f, x, y, barWidth, barHeight, fillColor, bgColor, windowWidth, windowHeight);
}

