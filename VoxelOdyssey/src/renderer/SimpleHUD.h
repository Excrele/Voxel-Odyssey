#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class PlayerStats;

class SimpleHUD {
public:
    static void init();
    static void cleanup();
    static void renderCrosshair(Shader& shader, int windowWidth, int windowHeight);
    static void renderBlockIndicator(Shader& shader, int selectedBlock, int windowWidth, int windowHeight);
    static void renderHealthBar(Shader& shader, float health, int windowWidth, int windowHeight);
    static void renderHungerBar(Shader& shader, float hunger, int windowWidth, int windowHeight);
    
private:
    static bool s_initialized;
    static unsigned int s_lineVAO, s_lineVBO;
    static unsigned int s_quadVAO, s_quadVBO;
    
    static void setupLineVAO();
    static void setupQuadVAO();
    static void renderBar(Shader& shader, float value, float maxValue, float x, float y, 
                         float width, float height, const glm::vec3& fillColor, 
                         const glm::vec3& bgColor, int windowWidth, int windowHeight);
};

