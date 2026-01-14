#pragma once

struct Camera2D{
    float zoom;
    float maxZoom;
    float minZoom;
    float baseHalfHeight;  //对应初始世界高度一半
    bool dragging;
    double lastX;
    double lastY;
    float dragSpeed;
    float zoomSpeed;
    int screenWidth;
    int screenHeight;
    glm::vec2 center;
    glm::mat4 getViewMatrix() {
        return glm::translate(glm::mat4(1.0f), glm::vec3(-center, 0.0f));
    }
    glm::mat4 getProjectionMatrix() {
        float aspect = (screenHeight > 0) ? (float)screenWidth / (float)screenHeight : 1.0f;
        float halfH = baseHalfHeight / zoom;
        float halfW = halfH * aspect;
        float left   = -halfW;
        float right  =  halfW;
        float bottom = -halfH;
        float top    =  halfH;
        return glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    }
    glm::vec2 screen2world(float sx, float sy){
        // 屏幕坐标系左上角原点，宽高为windowWidth, windowHeight
        float xNdc = (2.0f * sx) / screenWidth - 1.0f;
        float yNdc = -(2.0f * sy) / screenHeight + 1.0f;

        float aspect = (screenHeight > 0) ? (float)screenWidth / (float)screenHeight : 1.0f;
        float halfH = baseHalfHeight / zoom;
        float halfW = halfH * aspect;
        float camX = xNdc * halfW;
        float camY = yNdc * halfH;
        return glm::vec2(camX, camY) + center;
    };
    glm::vec2 screenDelta2world(double dpx, double dpy){
        float aspect = (screenHeight > 0) ? (float)screenWidth / (float)screenHeight : 1.0f;

        float halfH = baseHalfHeight / zoom;
        float halfW = halfH * aspect;

        // 视口宽高对应世界宽高：
        float worldW = 2.0f * halfW;
        float worldH = 2.0f * halfH;

        // 像素->世界：dxPixels/width * worldW
        // GLFW y 向下为正，因此 dyPixels 增加意味着鼠标向下移动，
        float dxWorld = (float)(dpx / (double)screenWidth)  * worldW;
        float dyWorld = (float)(dpy / (double)screenHeight) * worldH;

        return glm::vec2(dxWorld, dyWorld);
    };
};