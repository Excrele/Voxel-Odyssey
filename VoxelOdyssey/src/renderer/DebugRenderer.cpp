#include "DebugRenderer.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

unsigned int DebugRenderer::s_VAO = 0;
unsigned int DebugRenderer::s_VBO = 0;
bool DebugRenderer::s_initialized = false;

void DebugRenderer::init() {
    if (s_initialized) return;
    
    // Create wireframe box vertices (12 edges of a cube)
    float vertices[] = {
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        // Top face
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
        // Vertical edges
        -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f
    };
    
    glGenVertexArrays(1, &s_VAO);
    glGenBuffers(1, &s_VBO);
    
    glBindVertexArray(s_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    s_initialized = true;
}

void DebugRenderer::cleanup() {
    if (!s_initialized) return;
    
    glDeleteVertexArrays(1, &s_VAO);
    glDeleteBuffers(1, &s_VBO);
    
    s_VAO = 0;
    s_VBO = 0;
    s_initialized = false;
}

void DebugRenderer::renderBlockOutline(Shader& shader, const glm::ivec3& blockPos, 
                                       const glm::mat4& view, const glm::mat4& projection) {
    if (!s_initialized) {
        init();
    }
    
    // Create model matrix (translate to block position)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(blockPos.x + 0.5f, blockPos.y + 0.5f, blockPos.z + 0.5f));
    
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f)); // Yellow outline
    
    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    
    glBindVertexArray(s_VAO);
    glDrawArrays(GL_LINES, 0, 24); // 12 edges * 2 vertices each
    glBindVertexArray(0);
    
    // Disable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

