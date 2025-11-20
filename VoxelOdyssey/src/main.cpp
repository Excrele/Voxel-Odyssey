#include "core/window.h"
#include <glad/glad.h>
#include <iostream>

int main() {
    try {
        Window window (1280,720, "Voxel Odyssey");

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Glad init failed");
        }

        std::cout << "OpenGL Version: " <<glGetString(GL_VERSION) <<std::endl;

        //basic clear loop
        while (!window.shouldClose()) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // creates a teal colored background
            glClear(GL_COLOR_BUFFER_BIT);

            static unsigned int texture = 0;  // Load once
              if (!texture) {
            // stb_image load code here (int width, height, nrChannels; unsigned char* data = stbi_load("assets/textures/block_atlas.png", &width, &height, &nrChannels, 0);
                             glGenTextures(1, &texture);
                             glBindTexture(GL_TEXTURE_2D, texture);
                             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                             stbi_image_free(data);
                            }

Renderer renderer;
Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));  // Move it
renderer.drawCube(shader, model, texture);

            window.swapBufferes();
            window.pollEvents();
        }
    } catch (const std::exception& e) {
        std:cerr << "Error: " << e.what() << std::endl;
        return-1;
    }
    return 0;
}