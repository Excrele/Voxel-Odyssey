#include "Input.h"
#include <cstring>

GLFWwindow* Input::s_window = nullptr;
bool Input::s_keys[1024] = { false };
bool Input::s_keysLastFrame[1024] = { false };
bool Input::s_mouseButtons[8] = { false };
bool Input::s_mouseButtonsLastFrame[8] = { false };
glm::vec2 Input::s_mousePosition = glm::vec2(0.0f);
glm::vec2 Input::s_mouseLastPosition = glm::vec2(0.0f);
glm::vec2 Input::s_mouseDelta = glm::vec2(0.0f);
bool Input::s_firstMouse = true;
bool Input::s_mouseLocked = true;

void Input::initialize(GLFWwindow* window) {
    s_window = window;
    s_firstMouse = true;
    s_mouseLocked = true;
    
    // Set mouse callbacks
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // Lock cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Input::isKeyPressed(int key) {
    if (s_window == nullptr) return false;
    return glfwGetKey(s_window, key) == GLFW_PRESS;
}

bool Input::isKeyJustPressed(int key) {
    return isKeyPressed(key) && !s_keysLastFrame[key];
}

glm::vec2 Input::getMousePosition() {
    return s_mousePosition;
}

glm::vec2 Input::getMouseDelta() {
    return s_mouseDelta;
}

bool Input::isMouseButtonPressed(int button) {
    if (s_window == nullptr) return false;
    return glfwGetMouseButton(s_window, button) == GLFW_PRESS;
}

bool Input::isMouseButtonJustPressed(int button) {
    return isMouseButtonPressed(button) && !s_mouseButtonsLastFrame[button];
}

void Input::setMouseLocked(bool locked) {
    s_mouseLocked = locked;
    if (s_window) {
        glfwSetInputMode(s_window, GLFW_CURSOR, 
                        locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (locked) {
            s_firstMouse = true;
        }
    }
}

void Input::update() {
    // Store last frame's key states
    for (int i = 0; i < 1024; i++) {
        s_keysLastFrame[i] = s_keys[i];
        s_keys[i] = isKeyPressed(i);
    }
    
    // Store last frame's mouse button states
    for (int i = 0; i < 8; i++) {
        s_mouseButtonsLastFrame[i] = s_mouseButtons[i];
        s_mouseButtons[i] = isMouseButtonPressed(i);
    }
    
    // Reset mouse delta at end of frame (will be updated by callback next frame)
    if (!s_mouseLocked) {
        s_mouseDelta = glm::vec2(0.0f);
    }
}

void Input::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (s_firstMouse) {
        s_mouseLastPosition.x = static_cast<float>(xpos);
        s_mouseLastPosition.y = static_cast<float>(ypos);
        s_mousePosition.x = static_cast<float>(xpos);
        s_mousePosition.y = static_cast<float>(ypos);
        s_firstMouse = false;
        return;
    }
    
    s_mousePosition.x = static_cast<float>(xpos);
    s_mousePosition.y = static_cast<float>(ypos);
    
    if (s_mouseLocked) {
        // Calculate delta (reversed y since y-coordinates go from bottom to top)
        s_mouseDelta.x = static_cast<float>(xpos) - s_mouseLastPosition.x;
        s_mouseDelta.y = s_mouseLastPosition.y - static_cast<float>(ypos);
        
        // Update last position
        s_mouseLastPosition.x = static_cast<float>(xpos);
        s_mouseLastPosition.y = static_cast<float>(ypos);
    }
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Can be used for zoom or other scroll-based features
}

