#include "key.h"

#include <cstring>

KeyManager::KeyManager(){
    for(int i=0;i<GLFW_KEY_LAST;i++){
        keyStates[i] = false;
        keyBuffer[i] = false;
    }
}

void KeyManager::r(int key){
    registeredKeys.push_back(key);
}

void KeyManager::registerKey(){
    r(GLFW_KEY_ESCAPE);
}

void KeyManager::bufferRefresh(){
    std::memcpy(keyBuffer, keyStates, sizeof(keyStates));
}

void KeyManager::keyRefresh(){
    bufferRefresh();
    for(auto key : registeredKeys){
        keyStates[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }
}

bool KeyManager::isKeyDown(int key){
    return keyStates[key];
}

bool KeyManager::isKeyPressed(int key){
    return keyStates[key] && !keyBuffer[key];
}

bool KeyManager::isKeyHeld(int key){
    return keyStates[key] && keyBuffer[key];
}