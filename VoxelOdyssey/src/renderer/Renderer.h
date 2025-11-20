#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class Renderer {
private:
    static unsigned int s_cubeVAO;
    static unsigned int s_cubeVBO;
    static bool s_initialized;
    static void initCubeVAO();
    
public:
    Renderer();
    void drawCube(Shader& shader, glm::mat4 model, unsigned int texture);
    static void cleanup();
};

