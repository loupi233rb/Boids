#pragma once

#include "info.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>


class KeyManager{
private:
    bool keyStates[GLFW_KEY_LAST + 1];
    bool keyBuffer[GLFW_KEY_LAST + 1];
    std::vector<int> registeredKeys;
public:
    KeyManager();
    void registerKey();
    void r(int key);
    void bufferRefresh();
    void keyRefresh();
    bool isKeyDown(int key);
    bool isKeyPressed(int key);
    bool isKeyHeld(int key);
};