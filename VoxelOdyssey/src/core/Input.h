#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input {
public:
    static void initialize(GLFWwindow* window);
    
    // Keyboard
    static bool isKeyPressed(int key);
    static bool isKeyJustPressed(int key);
    
    // Mouse
    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonJustPressed(int button);
    static void setMouseLocked(bool locked);
    static bool isMouseLocked() { return s_mouseLocked; }
    
    // Update (call at end of frame)
    static void update();
    
    // Callbacks (set by initialize)
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
private:
    static GLFWwindow* s_window;
    static bool s_keys[1024];
    static bool s_keysLastFrame[1024];
    static bool s_mouseButtons[8];
    static bool s_mouseButtonsLastFrame[8];
    static glm::vec2 s_mousePosition;
    static glm::vec2 s_mouseLastPosition;
    static glm::vec2 s_mouseDelta;
    static bool s_firstMouse;
    static bool s_mouseLocked;
};

