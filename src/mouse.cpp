#include "mouse.h"

void Mouse::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    (void)xoffset;
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    glm::vec2 worldBefore = camera.screen2world((float)mx, (float)my);
    // 更新 zoom（指数缩放更平滑）
    float newZoom = camera.zoom * std::pow(camera.zoomSpeed, (float)yoffset);
    camera.zoom = std::clamp(newZoom, camera.minZoom, camera.maxZoom);

    // 缩放后再次计算鼠标对应世界坐标，让它保持不变，通过调整 center 实现
    glm::vec2 worldAfter = camera.screen2world((float)mx, (float)my);
    camera.center += (worldBefore - worldAfter);
}

void Mouse::mouse_move_callback(GLFWwindow* window, double xpos, double ypos){
    mousePosition = glm::vec2((float)xpos, (float)ypos);
    if(!camera.dragging) return;
    double dx = xpos - camera.lastX;
    double dy = -ypos + camera.lastY;
    camera.lastX = xpos;
    camera.lastY = ypos;

    glm::vec2 dworld = camera.screenDelta2world(dx, dy);
    camera.center -= dworld * camera.dragSpeed;
}

void Mouse::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    (void)mods;
    ImGuiIO& io = ImGui::GetIO();
    if(io.WantCaptureMouse){
        camera.dragging = false;
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            camera.dragging = true;
            glfwGetCursorPos(window, &camera.lastX, &camera.lastY);
        }
        else if (action == GLFW_RELEASE)
        {
            camera.dragging = false;
        }
    }
    else if(button == GLFW_MOUSE_BUTTON_RIGHT){
        if(action == GLFW_PRESS){
            RBUTTON_DOWN = true;
        }else if(action == GLFW_RELEASE){
            RBUTTON_DOWN = false;
        }
    }
}

void Mouse::attachToWindow(GLFWwindow* window){
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, &Mouse::scroll_thunk);
    glfwSetCursorPosCallback(window, &Mouse::mouse_move_thunk);
    glfwSetMouseButtonCallback(window, &Mouse::mouse_button_thunk);
}