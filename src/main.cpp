#include "engine/Window.hpp"
#include "engine/core/Renderer.hpp"
#include "engine/core/ResourceCache.hpp"
#include "engine/rendering/IsometricTile.hpp"
#include "engine/rendering/TileMap.hpp"
#include "engine/rendering/Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace engine;

int main() {
    try {
        float window_width = 1280.0f, window_height = 720.0f;
        float aspect = window_width / window_height;
        Window window(window_width, window_height, "SimColony");
        std::cout << "Window created successfully" << std::endl;

        bool show_demo_window = false;
        bool show_debug_window = true;

        // Инициализация основных систем
        auto renderer = std::make_unique<Renderer>();
        auto resourceCache = std::make_unique<ResourceCache>();

        // Инициализация камеры
        Camera camera(1.0f, aspect);
        window.setCamera(&camera);
        float lastFrame = 0.0f;

        // Загружаем текстуры через ResourceCache
        auto groundTexture = resourceCache->getTexture("textures/stone_ground.png");
        auto waterTexture = resourceCache->getTexture("textures/sea_water.png");

        // Создаем карту
        const int TILE_WIDTH = 64;
        TileMap map(TILE_WIDTH, TILE_WIDTH);
        
        // Заполняем карту тестовыми данными
        for (int y = 0; y < TILE_WIDTH; ++y) {
            for (int x = 0; x < TILE_WIDTH; ++x) {
                TileData data;
                // Создаем слегка хаотичный паттерн для теста
                int randomValue = std::rand() % 2;
                if (randomValue == 0) {
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

        // Включаем тест глубины для правильного наложения объектов
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        while (!window.shouldClose()) {
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

            // Обработка клика мыши
            if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
            }

            // Начинаем новый кадр
            renderer->beginFrame();
            
            // Устанавливаем матрицу вида-проекции
            renderer->setViewProjection(camera.getProjectionMatrix() * camera.getViewMatrix());

            // Отрисовываем карту используя новый рендерер
            // Инициализируем кэш тайлов при первом запуске
            static bool firstRun = true;
            if (firstRun) {
                renderer->clearTileCache();
                for (int y = 0; y < TILE_WIDTH; ++y) {
                    for (int x = 0; x < TILE_WIDTH; ++x) {
                        if (auto* tileData = map.getTileData(x, y)) {
                            glm::vec2 worldPos = map.gridToWorld({x, y});
                            renderer->cacheTile(x, y, worldPos, tileData->texture);
                        }
                    }
                }
                firstRun = false;
            }

            // Отрисовываем карту используя кэш
            for (int y = 0; y < TILE_WIDTH; ++y) {
                for (int x = 0; x < TILE_WIDTH; ++x) {
                    if (const CachedTileData* cachedTile = renderer->getCachedTile(x, y)) {
                        // Определяем, подсвечен ли тайл
                        bool isHighlighted = map.isHoveredTile(x, y);
                        
                        // Рисуем тайл используя кэшированные данные
                        renderer->drawTile(
                            cachedTile->worldPosition,
                            glm::vec2(1.0f),  // Размер тайла
                            cachedTile->texture,
                            isHighlighted,
                            glm::vec4(1.0f, 1.0f, 0.0f, 0.3f)  // Цвет подсветки
                        );
                    }
                }
            }

            // Завершаем рендеринг
            renderer->endFrame();

            // Начинаем новый кадр ImGui
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
                -1.0f,
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
                map.updateHoveredTile(tilePos);
            }

            // Отладочное окно
            if (show_debug_window) {
                ImGui::Begin("Debug Info", &show_debug_window);
                
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
                ImGui::Text("World Position: (%.2f, %.2f)", worldPos.x, worldPos.y);
                ImGui::Text("Tile Position: (%d, %d)", tilePos.x, tilePos.y);
                
                if (map.isValidPosition(tilePos.x, tilePos.y)) {
                    if (auto* tileData = map.getTileData(tilePos.x, tilePos.y)) {
                        ImGui::Text("Tile Type: %s", 
                            tileData->type == TileType::GROUND ? "Ground" :
                            tileData->type == TileType::WATER ? "Water" :
                            "Mountain");
                        ImGui::Text("Walkable: %s", tileData->walkable ? "Yes" : "No");
                    }
                }
                
                ImGui::Checkbox("Show Demo Window", &show_demo_window);
                ImGui::End();
            }

            if (show_demo_window) {
                ImGui::ShowDemoWindow(&show_demo_window);
            }

            window.endFrame();
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}