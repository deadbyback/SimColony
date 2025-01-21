#include "Window.hpp"
#include <stdexcept>
#include <iostream>

Camera* Window::camera = nullptr;

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (camera) {
        camera->zoom(static_cast<float>(yoffset));
    }
}

Window::Window(int width, int height, const std::string& title)
    : width(width), height(height) {
    // Инициализация GLFW - это первый шаг, без которого ничего работать не будет
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Настраиваем версию OpenGL и профиль
    // Мы используем Core profile, так как он предоставляет современный функционал
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Создаём окно с указанными размерами и заголовком
    // nullptr в качестве monitor означает оконный режим
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Делаем контекст OpenGL текущим для нашего окна
    glfwMakeContextCurrent(window);
    
    // Устанавливаем callback для изменения размера окна
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Инициализируем GLAD - загрузчик OpenGL функций
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetScrollCallback(window, scrollCallback);

    // После успешной инициализации OpenGL настраиваем ImGui
    initImGui();
}

Window::~Window() {
    // Сначала освобождаем ImGui
    shutdownImGui();
    
    // Затем освобождаем окно GLFW
    if (window) {
        glfwMakeContextCurrent(window);  // Важно: активируем контекст перед очисткой
        glfwDestroyWindow(window);
        window = nullptr;  // Важно обнулить указатель
    }
    
    // И только потом завершаем GLFW
    glfwTerminate();
}

void Window::initImGui() {
    // Проверяем версию ImGui
    IMGUI_CHECKVERSION();
    
    // Создаём контекст ImGui
    ImGui::CreateContext();
    
    // Получаем доступ к конфигурации ввода/вывода
    ImGuiIO& io = ImGui::GetIO();
    
    // Включаем поддержку навигации с клавиатуры
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Устанавливаем тёмную тему оформления
    ImGui::StyleColorsDark();

    // Инициализируем бэкенды ImGui для GLFW и OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void Window::shutdownImGui() {
    // Очищаем ресурсы ImGui в правильном порядке
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // При изменении размера окна обновляем область просмотра OpenGL
    glViewport(0, 0, width, height);
}

glm::vec2 Window::getMousePosition() const {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

bool Window::isMouseButtonPressed(int button) const {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void Window::beginFrame() {
    // Начинаем новый кадр ImGui
    // Порядок вызовов важен: сначала OpenGL, потом GLFW, затем сам ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::endFrame() {
    // Завершаем кадр ImGui и отрисовываем его
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Обмениваем буферы и обрабатываем события GLFW
    swapBuffers();
    pollEvents();
}
