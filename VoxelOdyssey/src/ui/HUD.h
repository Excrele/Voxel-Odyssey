#pragma once
#include <string>
#include <glm/glm.hpp>

class HUD {
public:
    static void init();
    static void cleanup();
    static void renderBlockSelection(int selectedBlock, int windowWidth, int windowHeight);
    static void renderCrosshair(int windowWidth, int windowHeight);
    
private:
    static bool s_initialized;
    static unsigned int s_VAO, s_VBO;
    
    static void renderQuad(float x, float y, float width, float height, const glm::vec3& color);
    static std::string getBlockName(int blockType);
};

