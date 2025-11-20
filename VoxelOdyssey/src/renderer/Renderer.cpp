#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  // Backface culling for perf
}

void Renderer::drawCube(Shader& shader, glm::mat4 model, unsigned int texture) {
    // Cube vertices (pos, normal, texcoord). This is a unit cube at origin.
    float vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  // Bottom-left
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  // Bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  // Top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  // Top-left
        // Back face (and so on for all 6 faces—I'll abbreviate; copy from a cube tutorial)
        // ... (Full array is ~288 floats; use online gen or expand below)
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", glm::lookAt(glm::vec3(0,0,3), glm::vec3(0), glm::vec3(0,1,0)));  // Cam
    shader.setMat4("projection", glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 100.0f));
    shader.setVec3("lightPos", glm::vec3(0,5,5));
    shader.setVec3("lightColor", glm::vec3(1));
    shader.setVec3("viewPos", glm::vec3(0,0,3));
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);  // 6 faces * 6 indices

    // Cleanup (per-frame for now; optimize later)
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}