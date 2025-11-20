#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window {
    public:
           Window(int widtch, int height, const char* title);
           ~Window();
           bool shouldClose() const;
           void swapBuffers();
           void pollEvents();
           GLFWwindow* getHandle() const { return m_window; }
           glm::vec2 getSize() const;

    private:
        GLFWwindow* m_window;
        int m_width, m_height;

};
