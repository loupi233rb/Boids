#include "render.h"
#include "shape.h"
#include "info.h"

void Renderer::initShader(const char* vertexPath, const char* fragmentPath){
    shader = Shader(vertexPath, fragmentPath);
}

inline void Renderer::bindDoubleBufferVBO(GLuint vao, GLuint shapevbo, GLuint shapeebo, GLuint instancevbo){
    glBindBuffer(GL_ARRAY_BUFFER, shapevbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bird_shape), bird_shape, GL_STATIC_DRAW);
    // Shape EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bird_shape_indices), bird_shape_indices, GL_STATIC_DRAW);
    // Instance VBO
    glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(birdInstance) * instances.size(), nullptr, GL_DYNAMIC_DRAW);
    // Vertex attributes
    glBindVertexArray(vao);
    // 鸟形状属性，位置0
    glBindBuffer(GL_ARRAY_BUFFER, shapevbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeebo);
    // Instance attributes
    glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(birdInstance), (void*)offsetof(birdInstance, color));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    int attrLoc = 3; 
    for (int i = 0; i < 4; ++i) {
        glVertexAttribPointer(attrLoc+i, 4, GL_FLOAT, GL_FALSE, sizeof(birdInstance), (void*)(offsetof(birdInstance, model) + i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(attrLoc+i);
        glVertexAttribDivisor(attrLoc+i, 1);
    }
    glBindVertexArray(0);
}

void Renderer::initBuffers(){
    // Generate and bind VAO, VBO, EBO, instance VBO
    glGenVertexArrays(2, VAO);
    glGenBuffers(1, &shapeVBO);
    glGenBuffers(1, &shapeEBO);
    glGenBuffers(2, instanceVBO);
    bindDoubleBufferVBO(VAO[0], shapeVBO, shapeEBO, instanceVBO[0]);
    bindDoubleBufferVBO(VAO[1], shapeVBO, shapeEBO, instanceVBO[1]);
}

inline double wrapPi(double a){
    while (a >  glm::pi<double>()) a -= 2*glm::pi<double>();
    while (a < -glm::pi<double>()) a += 2*glm::pi<double>();
    return a;
}

inline double lerpAngle(double a, double b, double t){
    float d = wrapPi(b - a);     // 最短角差
    return a + d * t;
}

void Renderer::updateInstances(){
    double t = Lfrc.getLERP();
    t = std::clamp(t, 0.0, 1.0);
    instances.clear();
    for(const auto& b : birds){
        birdInstance instance;
        glm::mat4 model = glm::mat4(1.0f);
        float theta = std::atan2(b->getV().y, b->getV().x);
        float prevTheta = std::atan2(b->prevVelocity.y, b->prevVelocity.x);
        theta = lerpAngle(prevTheta, theta, (float)t);
        vector2 interpPos = b->prevPosition * (1.0 - t) + b->getPos() * t;
        model = glm::translate(model, glm::vec3(interpPos.x, interpPos.y, 0.0f));
        model = glm::rotate(model, theta, glm::vec3(0.0f, 0.0f, 1.0f));
        float scale = 10.0f;
        model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
        instance.model = model;
        instance.color = glm::vec4(b->color, 1.0f);
        instances.push_back(instance);
    }
}

void Renderer::render(GLFWwindow *window){
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // 切换写入缓冲区，并对更新后的数据进行写入
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[curWrite]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(birdInstance) * instances.size()), instances.data(), GL_DYNAMIC_DRAW);
    if(start){
        start = false;
        std::swap(curWrite, curDraw);
        return;
    }
    glBindVertexArray(VAO[curDraw]);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[curDraw]);
    // 绘制
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)instances.size());
    glBindVertexArray(0);
    // 切换读写缓冲区索引
    std::swap(curWrite, curDraw);
    glfwSwapBuffers(window);
}

Renderer::~Renderer(){
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(1, &shapeVBO);
    glDeleteBuffers(1, &shapeEBO);
    glDeleteBuffers(2, instanceVBO);
    shader.deleteProgram();
}