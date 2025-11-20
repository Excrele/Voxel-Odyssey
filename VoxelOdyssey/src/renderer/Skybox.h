#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    void init();
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    
private:
    unsigned int m_VAO, m_VBO;
    unsigned int m_textureID;
    bool m_initialized;
    
    void loadSkybox();
    void generateProceduralSkybox();
};

