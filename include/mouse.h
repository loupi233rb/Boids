#pragma once

#include "info.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

class Mouse{
private:
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
public:
    Mouse() = default;
    ~Mouse() = default;
    static void scroll_thunk(GLFWwindow* window, double xoffset, double yoffset){
        auto* self = static_cast<Mouse*>(glfwGetWindowUserPointer(window));
        if(self) self->scroll_callback(window, xoffset, yoffset);
    }
    static void mouse_move_thunk(GLFWwindow* window, double xpos, double ypos){
        auto* self = static_cast<Mouse*>(glfwGetWindowUserPointer(window));
        if(self) self->mouse_move_callback(window, xpos, ypos);
    };
    static void mouse_button_thunk(GLFWwindow* window, int button, int action, int mods){
        auto* self = static_cast<Mouse*>(glfwGetWindowUserPointer(window));
        if(self) self->mouse_button_callback(window, button, action, mods);
    }
    void attachToWindow(GLFWwindow* window);
};
