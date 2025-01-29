// game/world/WorldMap.hpp
#pragma once
#include "BiomeType.hpp"
#include "TileProperties.hpp"
#include <vector>
#include <random>
#include <memory>

namespace game {
   class WorldMap {
   public:
       struct WorldTile {
           BiomeType biome = BiomeType::TEMPERATE;
           float elevation = 0.0f;
           float temperature = 20.0f;
           float rainfall = 0.5f;
           
           // Кэшированные данные для генерации локальной карты
           float baseElevation = 0.0f;
           float baseFertility = 0.0f;
           float baseTemperature = 20.0f;
       };

       WorldMap(int width, int height) 
           : width(width), height(height), 
             tiles(width * height),
             rng(std::random_device{}()) {}

       void generate(uint32_t seed = 0);
       const WorldTile& getTile(int x, int y) const;
       WorldTile& getTile(int x, int y);
       
       int getWidth() const { return width; }
       int getHeight() const { return height; }

       // Геттеры для параметров генерации
       uint32_t getSeed() const { return currentSeed; }
       
       // Проверка координат
       bool isValidPosition(int x, int y) const {
           return x >= 0 && x < width && y >= 0 && y < height;
       }

   private:
       int width;
       int height;
       std::vector<WorldTile> tiles;
       std::mt19937 rng;
       uint32_t currentSeed = 0;

       // Вспомогательные методы для генерации
       void generateElevation();
       void generateTemperature();
       void generateRainfall();
       void determineBiomes();
       
       int getIndex(int x, int y) const {
           return y * width + x;
       }
   };
}