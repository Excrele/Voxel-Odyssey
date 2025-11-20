#include "core/window.h"
#include <glad/glad.h>
#include <iostream>

int main() {
    // ... Window setup

    // Texture load (once)
    unsigned int texture = 0;
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // GL convention
    unsigned char* data = stbi_load("assets/textures/atlas.png", &width, &height, &channels, 0);
    if (data) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    World world;
    Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    // Cam setup (fixed for now)
    glm::vec3 camPos(8, 10, 8);  // Eye level over chunk
    glm::mat4 view = glm::lookAt(camPos, camPos + glm::vec3(0,-1,0), glm::vec3(0,1,0));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 1000.0f);

    while (!window.shouldClose()) {
        float time = (float)glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update
        world.update(camPos);

        // Render
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", proj);
        shader.setVec3("viewPos", camPos);
        shader.setVec3("lightPos", camPos + glm::vec3(10,10,10));  // Sun-ish

        world.render(shader, texture);

        window.swapBuffers();
        window.pollEvents();
    }

    // Cleanup texture
    glDeleteTextures(1, &texture);
    return 0;
}