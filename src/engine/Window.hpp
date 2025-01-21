#pragma once
#include "rendering/Camera.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    
    // Методы для работы с ImGui
    void beginFrame(); // Начало нового кадра (включая ImGui)
    void endFrame();   // Завершение кадра (включая ImGui)

    glm::vec2 getMousePosition() const;
    bool isMouseButtonPressed(int button) const;

    GLFWwindow* getGLFWwindow() const { return window; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Добавляем коллбэк для скролла
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    // Функция для получения указателя на камеру
    static void setCamera(Camera* cam) { camera = cam; }
    
private:
    GLFWwindow* window;
    static Camera* camera;  // Статический указатель на камеру
    int width;
    int height;
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void initImGui(); // Метод инициализации ImGui
    void shutdownImGui(); // Метод очистки ImGui
};