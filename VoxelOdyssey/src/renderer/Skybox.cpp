#include "Skybox.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

Skybox::Skybox() : m_VAO(0), m_VBO(0), m_textureID(0), m_initialized(false) {
}

Skybox::~Skybox() {
    if (m_initialized) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteTextures(1, &m_textureID);
    }
}

void Skybox::generateProceduralSkybox() {
    // Generate a simple gradient skybox procedurally
    // Top = light blue, bottom = darker blue (horizon effect)
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            
            // Gradient from light blue (top) to darker blue (bottom)
            float t = (float)y / height;
            float r = 0.4f + 0.2f * (1.0f - t); // Red component
            float g = 0.6f + 0.2f * (1.0f - t); // Green component
            float b = 0.9f + 0.1f * (1.0f - t); // Blue component
            
            data[idx] = (unsigned char)(r * 255);
            data[idx + 1] = (unsigned char)(g * 255);
            data[idx + 2] = (unsigned char)(b * 255);
        }
    }
    
    // Create 6 faces of the cube map (all same for simplicity)
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
                     width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::loadSkybox() {
    // For now, generate procedurally
    // Later can load from 6 images
    generateProceduralSkybox();
}

void Skybox::init() {
    if (m_initialized) return;
    
    // Skybox vertices (large cube)
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    loadSkybox();
    
    m_initialized = true;
}

void Skybox::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    if (!m_initialized) init();
    
    // Remove translation from view matrix (skybox should follow camera rotation only)
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    
    glDepthFunc(GL_LEQUAL); // Change depth function so skybox passes depth test at max depth
    
    shader.use();
    shader.setMat4("view", skyboxView);
    shader.setMat4("projection", projection);
    
    // Bind skybox texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    shader.setInt("skybox", 0);
    
    // Render skybox
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glDepthFunc(GL_LESS); // Reset depth function
}

