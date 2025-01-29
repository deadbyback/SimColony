#pragma once
#include "../World.hpp"
#include "../components/TileComponent.hpp"
#include "../components/RenderableComponent.hpp"
#include <fstream>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

namespace engine {
    struct MapHeader {
        uint32_t magic;         // Магическое число для проверки формата файла
        uint32_t version;       // Версия формата
        uint32_t width;         // Ширина карты
        uint32_t height;        // Высота карты
    };

    class SerializationSystem {
    public:
        // Структура для записи тайла в файл
        struct TileRecord {
            uint32_t x;
            uint32_t y;
            uint8_t type;
            uint8_t walkable;
        };

        SerializationSystem() {
            // Создаем директорию для сохранений, если её нет
            savePath = "saves";
            if (!fs::exists(savePath)) {
                fs::create_directory(savePath);
            }
        }

        bool saveMap(const World& world, const std::string& filename) {
            fs::path fullPath = savePath / filename;
            std::ofstream file(fullPath, std::ios::binary);
        
            if (!file) {
                std::cerr << "Cannot create save file: " << fullPath << std::endl;
                return false;
            }
            
            // Получаем тайлы
            auto tiles = world.getComponents<TileComponent>();
            
            // Проверяем, есть ли тайлы для сохранения
            if (tiles.empty()) {
                std::cerr << "No tiles to save" << std::endl;
                return false;
            }
            
            // Подготовка данных
            MapHeader header{
                0x434F4C53,  // SLOC в ASCII
                1,           // Версия 1
                64, 64,      // Размеры карты
            };
            // Записываем заголовок
            file.write(reinterpret_cast<const char*>(&header), sizeof(header));

            // Записываем данные тайлов
            for (const auto* tile : tiles) {
                TileRecord record;

                record.x = tile->gridPosition.x;
                record.y = tile->gridPosition.y;
                record.type = static_cast<uint8_t>(tile->type);
                record.walkable = tile->walkable ? 1 : 0;

                file.write(reinterpret_cast<const char*>(&record), sizeof(record));
            }
            
            return true;
        }

        bool loadMap(World& world, const std::string& filename, 
                    ResourceCache& resourceCache, TileSystem& tileSystem) {
            fs::path fullPath = savePath / filename;
            
            if (!fs::exists(fullPath)) {
                std::cerr << "Save file does not exist: " << fullPath << std::endl;
                return false;
            }

            std::ifstream file(fullPath, std::ios::binary);
            if (!file) {
                std::cerr << "Cannot open save file: " << fullPath << std::endl;
                return false;
            }

            // Читаем заголовок
            MapHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(header));
            
            // Проверяем магическое число и версию формата
            if (header.magic != 0x434F4C53) {
                std::cerr << "Invalid file format" << std::endl;
                return false;
            }
            
            if (header.version != 1) {
                std::cerr << "Unsupported save file version: " << header.version << std::endl;
                return false;
            }

            // Очищаем текущий мир
            world = World();

            // Читаем и создаем тайлы
            TileRecord record;
            bool tilesLoaded = false;

            while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
                TileData data;
                data.type = static_cast<TileType>(record.type);
                data.walkable = record.walkable != 0;

                std::string texturePath;
                switch(data.type) {
                    case TileType::WATER:
                        texturePath = "tiles/water/sea_water.png";
                        break;
                    case TileType::SAND:
                        texturePath = "tiles/ground/sand.png";
                        break;
                    case TileType::GRASS:
                        texturePath = "tiles/ground/grass.png";
                        break;
                    case TileType::SNOW:
                        texturePath = "tiles/ground/snow.png";
                        break;
                    case TileType::FOREST:
                        texturePath = "tiles/ground/forest.png";
                        break;
                    case TileType::MOUNTAIN:
                        texturePath = "tiles/ground/mountain.png";
                        break;
                    case TileType::DIRT:
                        texturePath = "tiles/ground/dirt.png";
                        break;
                    default:
                        texturePath = "tiles/ground/stone_ground.png";
                }
                
                data.texture = resourceCache.getTexture(texturePath);
                
                GridPosition pos{static_cast<int>(record.x), static_cast<int>(record.y)};
                tileSystem.createTile(world, data, pos);
                tilesLoaded = true;
            }

            if (!tilesLoaded) {
                std::cerr << "No tiles loaded from save file" << std::endl;
                return false;
            }

            return true;
        }
    private:
        fs::path savePath;
    };
}