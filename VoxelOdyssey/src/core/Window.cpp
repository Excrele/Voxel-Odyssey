#include "Window.h"
#include <stdexcept>
#include <glad/glad.h>

Window::Window(int width, int height, const char* title) : m_width(width), m_height(height) {
    if (!glfwInit()) throw std::runtime_error("GLFW init failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) { glfwTerminate(); throw std::runtime_error("Window creation failed");}
    glfwMakeContextCurrent(m_window);
    
    // Initialize GLAD
    if (!gladLoadGL()) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // For FPS cam at a later time
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }
void Window::swapBuffers() { glfwSwapBuffers(m_window); }
void Window::pollEvents() { glfwPollEvents(); }
glm::vec2 Window::getSize() const { int w, h; glfwGetFramebufferSize(m_window, &w, &h); return {w,h}; }
