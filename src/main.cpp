#include "engine/Window.hpp"
#include "engine/rendering/ShaderLoader.hpp"
#include "engine/rendering/IsometricTile.hpp"
#include "engine/rendering/Texture.hpp"
#include "engine/rendering/TileMap.hpp"
#include "engine/rendering/Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        float window_width = 1280.0f, window_height = 720.0f;
        float aspect = window_width / window_height;
        Window window(window_width, window_height, "SimColony");
        std::cout << "Window created successfully" << std::endl;

        bool show_demo_window = false;
        bool show_debug_window = true;

        // Инициализация камеры
        Camera camera(1.0f, aspect);
        window.setCamera(&camera);
        float lastFrame = 0.0f;

        // Загружаем шейдер для изометрической графики
        auto shader = ShaderLoader::loadFromFiles(
            "src/engine/rendering/shaders/Tile.vert",
            "src/engine/rendering/shaders/Tile.frag"
        );

        // Создаем тестовую текстуру
        auto groundTexture = std::make_shared<Texture>("src/engine/rendering/textures/stone_ground.png", true);
        auto waterTexture = std::make_shared<Texture>("src/engine/rendering/textures/sea_water.png", true);

        // Создаем карту
        const int TILE_WIDTH = 64;
        TileMap map(TILE_WIDTH, TILE_WIDTH);
        
        // Заполняем карту тестовыми данными
        for (int y = 0; y < TILE_WIDTH; ++y) {
            for (int x = 0; x < TILE_WIDTH; ++x) {
                TileData data;
                // Создаем шахматный паттерн для теста
                if ((x + y) % 2 == 0) {
                    data.type = TileType::GROUND;
                    data.walkable = true;
                    data.texture = groundTexture;
                } else {
                    data.type = TileType::WATER;
                    data.walkable = false;
                    data.texture = waterTexture;
                }
                map.setTile(x, y, data);
            }
        }

        GLint maxVertexAttribs;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
        std::cout << "Maximum vertex attributes supported: " << maxVertexAttribs << std::endl;

        // Вычисляем позицию камеры для изометрического вида
        const float iso_angle_x = glm::radians(60.0f); 
        const float iso_angle_y = glm::radians(45.0f);

        // Создаем матрицу вида для изометрической проекции
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));  // Отодвигаем камеру
        view = glm::rotate(view, iso_angle_x, glm::vec3(1.0f, 0.0f, 0.0f));  // Поворот вокруг X
        view = glm::rotate(view, iso_angle_y, glm::vec3(0.0f, 1.0f, 0.0f));  // Поворот вокруг Y

        // Включаем тест глубины для правильного наложения объектов
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        while (!window.shouldClose()) {
            // Рассчитываем deltaTime
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Обновляем состояние камеры
            camera.update(deltaTime);

            // Обрабатываем ввод
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS) {
                camera.moveUp(deltaTime);
            }
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS) {
                camera.moveDown(deltaTime);
            }
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS) {
                camera.moveRight(deltaTime);
            }
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS) {
                camera.moveLeft(deltaTime);
            }

            // Добавим возможность фокусироваться на точке по клику
            if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
                
                // Здесь нужно будет добавить преобразование координат экрана в мировые координаты
                // Пока что просто для теста:
                // camera.focusOn(glm::vec2(xpos - window_width/2, ypos - window_height/2));
            }

            // Очищаем буфер и устанавливаем цвет фона
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

            // Проверяем ошибки OpenGL перед рендерингом
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cout << "OpenGL Error before rendering: " << error << std::endl;
            }

            // Активируем наш шейдер для рендеринга
            shader->use();
            shader->setMat4("projection", camera.getProjectionMatrix());
            shader->setMat4("view", camera.getViewMatrix());

            // Отрисовываем всю карту
            map.draw(shader);

            // Проверяем ошибки после рендеринга
            error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cout << "OpenGL Error after rendering: " << error << std::endl;
            }

            // Начинаем новый кадр (включая ImGui)
            window.beginFrame();

                // Получаем позицию мыши и обновляем подсвеченный тайл
                glm::vec2 mousePos = window.getMousePosition();
                glm::vec2 windowSize(window.getWidth(), window.getHeight());

                // Получаем матрицы преобразования
                glm::mat4 proj = camera.getProjectionMatrix();
                glm::mat4 view = camera.getViewMatrix();
                glm::mat4 invMatrix = glm::inverse(proj * view);

                // Преобразуем экранные координаты в нормализованные координаты устройства (NDC)
                glm::vec4 rayStart = glm::vec4(
                    (2.0f * mousePos.x) / windowSize.x - 1.0f,
                    1.0f - (2.0f * mousePos.y) / windowSize.y,
                    -1.0f, // Ближняя плоскость отсечения
                    1.0f
                );

                // Преобразуем в мировые координаты
                rayStart = invMatrix * rayStart;
                rayStart /= rayStart.w;

                // Позиция в мировых координатах
                glm::vec2 worldPos(rayStart.x, rayStart.y);
                
                // Получаем координаты тайла
                GridPosition tilePos = map.worldToGrid(worldPos);
                if (map.isValidPosition(tilePos.x, tilePos.y)) {
                    std::cout << "Hovering tile: " << tilePos.x << ", " << tilePos.y << std::endl;
                }
                
                // Обновляем подсветку тайла
                map.updateHoveredTile(tilePos);

                // Отладочное окно с информацией о производительности
                if (show_debug_window) {
                    ImGui::Begin("Debug Info", &show_debug_window);
                    
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    
                    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
                    
                    ImGui::Separator();
                    
                    ImGui::Text("Camera:");
                    auto camPos = camera.getPosition();
                    ImGui::Text("Position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
                    ImGui::Text("Zoom: %.2f", camera.getZoomLevel());
                    
                    ImGui::Separator();
                    
                    ImGui::Text("Mouse & Tile:");
                    ImGui::Text("Screen Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
                    ImGui::Text("World Position: (%.2f, %.2f, %.2f)", worldPos.x, worldPos.y, rayStart.z);
                    ImGui::Text("Tile Position: (%d, %d)", tilePos.x, tilePos.y);
                    ImGui::Text("Valid Tile: %s", map.isValidPosition(tilePos.x, tilePos.y) ? "Yes" : "No");
                    
                    if (map.isValidPosition(tilePos.x, tilePos.y)) {
                        if (auto* tileData = map.getTileData(tilePos.x, tilePos.y)) {
                            ImGui::Text("Tile Type: %s", 
                                tileData->type == TileType::GROUND ? "Ground" :
                                tileData->type == TileType::WATER ? "Water" :
                                tileData->type == TileType::MOUNTAIN ? "Mountain" : "None");
                            ImGui::Text("Walkable: %s", tileData->walkable ? "Yes" : "No");
                            ImGui::Text("Elevation: %.2f", tileData->elevation);
                        }
                    }
                    
                    ImGui::Checkbox("Show Demo Window", &show_demo_window);
                    ImGui::End();
                }

                // Демонстрационное окно ImGui
                if (show_demo_window) {
                    ImGui::ShowDemoWindow(&show_demo_window);
                }

                // Завершаем кадр (это включает swap buffers и poll events)
                window.endFrame();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}