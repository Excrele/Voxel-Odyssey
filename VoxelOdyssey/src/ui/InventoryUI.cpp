#include "InventoryUI.h"
#include "world/Block.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iomanip>

bool InventoryUI::s_initialized = false;
unsigned int InventoryUI::s_quadVAO = 0;
unsigned int InventoryUI::s_quadVBO = 0;
unsigned int InventoryUI::s_lineVAO = 0;
unsigned int InventoryUI::s_lineVBO = 0;

void InventoryUI::setupQuadVAO() {
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

void InventoryUI::setupLineVAO() {
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

void InventoryUI::init() {
    if (s_initialized) return;
    
    setupQuadVAO();
    setupLineVAO();
    
    s_initialized = true;
}

void InventoryUI::cleanup() {
    if (!s_initialized) return;
    
    glDeleteVertexArrays(1, &s_quadVAO);
    glDeleteBuffers(1, &s_quadVBO);
    glDeleteVertexArrays(1, &s_lineVAO);
    glDeleteBuffers(1, &s_lineVBO);
    
    s_initialized = false;
}

glm::vec3 InventoryUI::getBlockColor(BlockType type) {
    switch (type) {
        case BlockType::GRASS: return glm::vec3(0.2f, 0.8f, 0.2f);
        case BlockType::DIRT: return glm::vec3(0.6f, 0.4f, 0.2f);
        case BlockType::STONE: return glm::vec3(0.5f, 0.5f, 0.5f);
        case BlockType::SAND: return glm::vec3(0.9f, 0.8f, 0.6f);
        case BlockType::WOOD: return glm::vec3(0.5f, 0.3f, 0.1f);
        case BlockType::LEAVES: return glm::vec3(0.1f, 0.6f, 0.1f);
        case BlockType::GRAVEL: return glm::vec3(0.4f, 0.4f, 0.4f);
        default: return glm::vec3(0.3f, 0.3f, 0.3f);
    }
}

void InventoryUI::renderTexturedQuad(Shader& shader, float x, float y, float width, float height,
                                     const glm::vec3& color, const glm::mat4& projection) {
    if (!s_initialized) init();
    
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("color", color);
    
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(s_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void InventoryUI::renderText(Shader& shader, const std::string& text, float x, float y,
                             float scale, const glm::mat4& projection) {
    // Placeholder for text rendering
    // In a full implementation, you'd use FreeType or similar
    // For now, we'll skip text rendering and just show colored boxes
}

void InventoryUI::renderHotbar(Shader& shader, const Inventory& inventory, int windowWidth, int windowHeight) {
    if (!s_initialized) init();
    
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    const int HOTBAR_SIZE = 9;
    const float slotSize = 50.0f;
    const float slotSpacing = 5.0f;
    const float hotbarWidth = HOTBAR_SIZE * slotSize + (HOTBAR_SIZE - 1) * slotSpacing;
    const float hotbarHeight = slotSize + 20.0f; // Extra space for selection indicator
    const float hotbarX = (windowWidth - hotbarWidth) / 2.0f;
    const float hotbarY = windowHeight - hotbarHeight - 20.0f;
    
    int selectedSlot = inventory.getSelectedHotbarSlot();
    
    // Render hotbar background
    renderTexturedQuad(shader, hotbarX - 5.0f, hotbarY - 5.0f, 
                      hotbarWidth + 10.0f, hotbarHeight + 10.0f,
                      glm::vec3(0.2f, 0.2f, 0.2f), projection);
    
    // Render each slot
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        float slotX = hotbarX + i * (slotSize + slotSpacing);
        float slotY = hotbarY;
        
        BlockType blockType = inventory.getHotbarSlot(i);
        
        // Render slot background
        glm::vec3 bgColor = (i == selectedSlot) ? glm::vec3(0.8f, 0.8f, 0.8f) : glm::vec3(0.4f, 0.4f, 0.4f);
        renderTexturedQuad(shader, slotX, slotY, slotSize, slotSize, bgColor, projection);
        
        // Render block if not air
        if (blockType != BlockType::AIR) {
            glm::vec3 blockColor = getBlockColor(blockType);
            float margin = 5.0f;
            renderTexturedQuad(shader, slotX + margin, slotY + margin, 
                              slotSize - 2 * margin, slotSize - 2 * margin,
                              blockColor, projection);
            
            // Render quantity if available
            int count = inventory.getBlockCount(blockType);
            if (count > 0) {
                // Would render text here - for now just show colored box
            }
        }
        
        // Render selection indicator (border)
        if (i == selectedSlot) {
            // Render border around selected slot
            float borderWidth = 3.0f;
            renderTexturedQuad(shader, slotX - borderWidth, slotY - borderWidth,
                              slotSize + 2 * borderWidth, borderWidth,
                              glm::vec3(1.0f, 1.0f, 0.0f), projection); // Top
            renderTexturedQuad(shader, slotX - borderWidth, slotY + slotSize,
                              slotSize + 2 * borderWidth, borderWidth,
                              glm::vec3(1.0f, 1.0f, 0.0f), projection); // Bottom
            renderTexturedQuad(shader, slotX - borderWidth, slotY,
                              borderWidth, slotSize,
                              glm::vec3(1.0f, 1.0f, 0.0f), projection); // Left
            renderTexturedQuad(shader, slotX + slotSize, slotY,
                              borderWidth, slotSize,
                              glm::vec3(1.0f, 1.0f, 0.0f), projection); // Right
        }
    }
}

void InventoryUI::renderInventory(Shader& shader, const Inventory& inventory, 
                                  int windowWidth, int windowHeight, bool isOpen) {
    if (!isOpen) return;
    if (!s_initialized) init();
    
    // Render full inventory UI (when 'E' is pressed)
    // For now, just show a simple overlay
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    
    // Render inventory background
    float invWidth = windowWidth * 0.6f;
    float invHeight = windowHeight * 0.7f;
    float invX = (windowWidth - invWidth) / 2.0f;
    float invY = (windowHeight - invHeight) / 2.0f;
    
    renderTexturedQuad(shader, invX, invY, invWidth, invHeight,
                      glm::vec3(0.1f, 0.1f, 0.1f), projection);
    
    // Render all blocks in inventory
    // This would show a grid of all available blocks
}

