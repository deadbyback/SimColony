#include "engine/Window.hpp"
#include "engine/core/Renderer.hpp"
#include "engine/core/ResourceCache.hpp"
#include "engine/ecs/World.hpp"
#include "engine/ecs/systems/RenderSystem.hpp"
#include "engine/ecs/systems/TileSystem.hpp"
#include "engine/ecs/systems/SelectionSystem.hpp"
#include "engine/ecs/systems/InteractionSystem.hpp"
#include "engine/ecs/systems/TileEditSystem.hpp"
#include "engine/ecs/systems/SerializationSystem.hpp"

#include "game/world/WorldMap.hpp"
#include "game/world/LocalMapGenerator.hpp"
#include "game/world/TileRegistry.hpp"
#include "game/world/BiomeType.hpp"
#include "game/Tile.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ctime>

using namespace engine;
using namespace game;

std::string getTileTypeName(TileType type) {
    switch(type) {
        case TileType::NONE: return "None";
        case TileType::GROUND: return "Ground";
        case TileType::WATER: return "Water";
        case TileType::DIRT: return "Dirt";
        case TileType::GRASS: return "Grass";
        case TileType::MOUNTAIN: return "Mountain";
        case TileType::SAND: return "Sand";
        case TileType::SNOW: return "Snow";
        case TileType::FOREST: return "Forest";
        default: return "Unknown";
    }
}

std::string getBiomeTypeName(BiomeType biome) {
    switch(biome) {
        case BiomeType::DESERT: return "Desert";
        case BiomeType::TROPICAL: return "Tropical";
        case BiomeType::TUNDRA: return "Tundra";
        case BiomeType::TEMPERATE: return "Temperate";
        case BiomeType::MOUNTAIN: return "Mountain";
        case BiomeType::BOREAL: return "Boreal";
        case BiomeType::SAVANNA: return "Savanna";
        default: return "Unknown";
    }
}

int main()
{
    try
    {
        float window_width = 1280.0f, window_height = 720.0f;
        float aspect = window_width / window_height;
        Window window(window_width, window_height, "SimColony");

        auto renderer = std::make_unique<Renderer>();
        auto resourceCache = std::make_unique<ResourceCache>();

        // ECS системы
        World world;
        RenderSystem renderSystem(*renderer);
        TileSystem tileSystem;
        SelectionSystem selectionSystem;
        InteractionSystem interactionSystem;
        TileEditSystem editSystem;
        SerializationSystem serializationSystem;
        TileRegistry tileRegistry(*resourceCache);

        // Создаем генераторы карт
        WorldMap worldMap(50, 50); // Создаем глобальную карту 500x500
        LocalMapGenerator mapGenerator(*resourceCache, tileRegistry);

        // Параметры генерации локальной карты
        LocalMapGenerator::GenerationParams genParams;
        genParams.width = 50;
        genParams.height = 50;
        genParams.detailLevel = 1.0f;
        genParams.roughness = 1.0f;
        genParams.seed = static_cast<uint32_t>(std::time(nullptr));

        // Генерируем глобальную карту
        worldMap.generate(genParams.seed);

        // Берем центральный тайл глобальной карты для генерации локальной
        const auto &globalTile = worldMap.getTile(25, 25);

        // Генерируем локальную карту на основе глобального тайла
        mapGenerator.generateMap(world, tileSystem, globalTile, genParams);

        Camera camera(1.0f, aspect);
        window.setCamera(&camera);
        float lastFrame = 0.0f;

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        bool show_demo_window = false;
        bool show_debug_window = true;
        bool show_generation_window = true;

        while (!window.shouldClose())
        {
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            camera.update(deltaTime);

            // Обработка клавиш движения
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
                camera.moveUp(deltaTime);
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
                camera.moveDown(deltaTime);
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
                camera.moveRight(deltaTime);
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
                camera.moveLeft(deltaTime);

            glm::vec2 mousePos = window.getMousePosition();
            glm::vec2 windowSize(window.getWidth(), window.getHeight());

            glm::mat4 proj = camera.getProjectionMatrix();
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 invMatrix = glm::inverse(proj * view);

            glm::vec4 rayStart = glm::vec4(
                (2.0f * mousePos.x) / windowSize.x - 1.0f,
                1.0f - (2.0f * mousePos.y) / windowSize.y,
                -1.0f,
                1.0f);

            rayStart = invMatrix * rayStart;
            rayStart /= rayStart.w;
            glm::vec2 worldPos(rayStart.x, rayStart.y);

            GridPosition hoveredPos = TileSystem::worldToGrid(worldPos);
            selectionSystem.updateSelection(world, hoveredPos);

            // Обработка контрольных клавиш (для сохранения/загрузки - Ctrl + S/L)
            static bool ctrlPressed = false;
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            {
                ctrlPressed = true;
            }
            else
            {
                ctrlPressed = false;
            }

            if (ctrlPressed)
            {
                static bool sPressedLast = false;
                static bool lPressedLast = false;

                bool sPressed = glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS;
                bool lPressed = glfwGetKey(window.getGLFWwindow(), GLFW_KEY_L) == GLFW_PRESS;

                if (sPressed && !sPressedLast)
                {
                    if (serializationSystem.saveMap(world, "world.bin"))
                    {
                        std::cout << "Map saved successfully" << std::endl;
                    }
                }

                if (lPressed && !lPressedLast)
                {
                    if (serializationSystem.loadMap(world, "world.bin", *resourceCache, tileSystem))
                    {
                        std::cout << "Map loaded successfully" << std::endl;
                    }
                }

                sPressedLast = sPressed;
                lPressedLast = lPressed;
            }

            // Основной рендеринг
            renderer->beginFrame();
            renderer->setViewProjection(camera.getProjectionMatrix() * camera.getViewMatrix());
            renderSystem.render(world);
            renderer->endFrame();

            // UI
            window.beginFrame();

            // Окно генерации карты
            if (show_generation_window)
            {
                ImGui::Begin("Map Generation", &show_generation_window);

                // Параметры генерации
                ImGui::SliderFloat("Detail Level", &genParams.detailLevel, 0.1f, 2.0f, "%.1f");
                ImGui::SliderFloat("Roughness", &genParams.roughness, 0.1f, 2.0f, "%.1f");

                if (ImGui::Button("Regenerate Map"))
                {
                    // Генерируем новый сид
                    genParams.seed = static_cast<uint32_t>(std::time(nullptr));

                    // Пересоздаем карты
                    worldMap.generate(genParams.seed);
                    world = World(); // Очищаем текущий мир
                    mapGenerator.generateMap(world, tileSystem, globalTile, genParams);
                }

                ImGui::Separator();

                // Информация о текущем биоме
                ImGui::Text("Current Biome: %s", getBiomeTypeName(globalTile.biome));
                ImGui::Text("Base Temperature: %.1f°C", globalTile.temperature);
                ImGui::Text("Base Rainfall: %.2f", globalTile.rainfall);

                ImGui::End();
            }

            if (show_debug_window)
            {
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
                ImGui::Text("Grid Position: (%d, %d)", hoveredPos.x, hoveredPos.y);

                if (const auto *selectedTile = selectionSystem.getSelectedTile())
                {
                    ImGui::Separator();
                    ImGui::Text("Selected Tile Info:");

                    // Basic tile info
                    ImGui::Text("Position: (%d, %d)",
                                selectedTile->gridPosition.x,
                                selectedTile->gridPosition.y);
                    
                    // Get extended tile component
                    auto *tileEntity = world.getEntity(selectedTile->getOwner());
                    if (tileEntity)
                    {
                        auto *extTile = tileEntity->getComponent<game::ExtendedTileComponent>();
                        if (extTile)
                        {
                            // Tile type and biome
                            ImGui::Text("Type: %s", getTileTypeName(extTile->type).c_str());
                            ImGui::Text("Biome: %s", getBiomeTypeName(extTile->biome).c_str());
                            
                            ImGui::Separator();
                            
                            // Properties
                            ImGui::Text("Properties:");
                            ImGui::Text("Elevation: %.2f", extTile->properties.elevation);
                            ImGui::Text("Fertility: %.2f", extTile->properties.fertility);
                            ImGui::Text("Temperature: %.1f°C", extTile->properties.temperature);
                            ImGui::Text("Humidity: %.2f", extTile->properties.humidity);
                            ImGui::Text("Walkable: %s", extTile->properties.walkable ? "Yes" : "No");
                            ImGui::Text("Buildable: %s", extTile->properties.buildable ? "Yes" : "No");
                            
                            ImGui::Separator();
                            
                            // Modifiers
                            ImGui::Text("Modifiers:");
                            for (const auto& [key, value] : extTile->modifiers) {
                                ImGui::Text("%s: %.2f", key.c_str(), value);
                            }
                        }
                    }
                }

                ImGui::Checkbox("Show Demo Window", &show_demo_window);

                ImGui::Separator();
                ImGui::Text("Map Controls:");

                if (ImGui::Button("Save Map"))
                {
                    if (serializationSystem.saveMap(world, "world.bin"))
                    {
                        std::cout << "Map saved successfully" << std::endl;
                    }
                    else
                    {
                        std::cerr << "Failed to save map" << std::endl;
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Load Map"))
                {
                    if (serializationSystem.loadMap(world, "world.bin", *resourceCache, tileSystem))
                    {
                        std::cout << "Map loaded successfully" << std::endl;
                    }
                    else
                    {
                        std::cerr << "Failed to load map" << std::endl;
                    }
                }

                ImGui::End();
            }

            if (show_demo_window)
            {
                ImGui::ShowDemoWindow(&show_demo_window);
            }

            window.endFrame();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}