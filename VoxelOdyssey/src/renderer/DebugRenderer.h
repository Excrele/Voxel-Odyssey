#pragma once
#include <glm/glm.hpp>

class Shader;

class DebugRenderer {
public:
    static void init();
    static void cleanup();
    static void renderBlockOutline(Shader& shader, const glm::ivec3& blockPos, const glm::mat4& view, const glm::mat4& projection);
    
private:
    static unsigned int s_VAO, s_VBO;
    static bool s_initialized;
};

