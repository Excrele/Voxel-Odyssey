#pragma once
#include "world/Inventory.h"
#include "renderer/Shader.h"
#include <glm/glm.hpp>

class InventoryUI {
public:
    static void init();
    static void cleanup();
    static void renderHotbar(Shader& shader, const Inventory& inventory, int windowWidth, int windowHeight);
    static void renderInventory(Shader& shader, const Inventory& inventory, int windowWidth, int windowHeight, bool isOpen);
    
private:
    static bool s_initialized;
    static unsigned int s_quadVAO, s_quadVBO;
    static unsigned int s_lineVAO, s_lineVBO;
    
    static void setupQuadVAO();
    static void setupLineVAO();
    static glm::vec3 getBlockColor(BlockType type);
    static void renderTexturedQuad(Shader& shader, float x, float y, float width, float height, 
                                   const glm::vec3& color, const glm::mat4& projection);
    static void renderText(Shader& shader, const std::string& text, float x, float y, 
                          float scale, const glm::mat4& projection);
};

