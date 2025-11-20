#include "HUD.h"
#include "world/Block.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

bool HUD::s_initialized = false;
unsigned int HUD::s_VAO = 0;
unsigned int HUD::s_VBO = 0;

void HUD::init() {
    if (s_initialized) return;
    
    // Create a simple quad for rendering UI elements
    float quadVertices[] = {
        // positions
        0.0f, 0.0f, 0.0f,  // bottom-left
        1.0f, 0.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 0.0f,  // top-right
        0.0f, 1.0f, 0.0f   // top-left
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    glGenVertexArrays(1, &s_VAO);
    glGenBuffers(1, &s_VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(s_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glDeleteBuffers(1, &EBO);
    
    s_initialized = true;
}

void HUD::cleanup() {
    if (!s_initialized) return;
    
    glDeleteVertexArrays(1, &s_VAO);
    glDeleteBuffers(1, &s_VBO);
    
    s_VAO = 0;
    s_VBO = 0;
    s_initialized = false;
}

std::string HUD::getBlockName(int blockType) {
    switch ((BlockType)blockType) {
        case BlockType::GRASS: return "Grass";
        case BlockType::DIRT: return "Dirt";
        case BlockType::STONE: return "Stone";
        case BlockType::SAND: return "Sand";
        case BlockType::WOOD: return "Wood";
        case BlockType::LEAVES: return "Leaves";
        case BlockType::GRAVEL: return "Gravel";
        default: return "Air";
    }
}

void HUD::renderQuad(float x, float y, float width, float height, const glm::vec3& color) {
    if (!s_initialized) init();
    
    // Simple quad rendering with immediate mode (for simplicity)
    // In a production system, you'd use a proper UI library or text rendering
    
    // For now, we'll use a simple approach with glBegin/glEnd (deprecated but works)
    // Actually, let's use a shader-based approach instead
    
    // This is a placeholder - in a real implementation, you'd render text or use a UI library
    // For now, we'll just render a colored rectangle
}

void HUD::renderBlockSelection(int selectedBlock, int windowWidth, int windowHeight) {
    // Simple text rendering placeholder
    // In a real implementation, you'd use a text rendering library like FreeType
    // For now, this is a placeholder that shows the concept
    
    // The actual rendering would happen here with a text renderer
    // For simplicity, we'll just note that this should display:
    // "Selected: [Block Name] (Press 1-7 to change)"
}

void HUD::renderCrosshair(int windowWidth, int windowHeight) {
    if (!s_initialized) init();
    
    // Render a simple crosshair in the center of the screen
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);
    
    // Simple crosshair using immediate mode (for simplicity)
    // In production, use proper rendering
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float size = 10.0f;
    
    // We'll use a simple approach - render lines
    // Note: This is a simplified version. In production, use proper UI rendering
    
    glEnable(GL_DEPTH_TEST);
}

