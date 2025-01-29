#include "WorldMap.hpp"
#include <FastNoiseLite.h>
#include <algorithm>
#include <ctime>

namespace game {

void WorldMap::generate(uint32_t seed) {
   currentSeed = seed == 0 ? static_cast<uint32_t>(std::time(nullptr)) : seed;
   rng.seed(currentSeed);

   // Генерируем базовые параметры мира
   generateElevation();
   generateTemperature();
   generateRainfall();
   determineBiomes();
}

void WorldMap::generateElevation() {
   FastNoiseLite noise;
   noise.SetSeed(rng());
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   
   // Настраиваем параметры шума для elevation
   noise.SetFrequency(0.02f);
   
   // Генерируем карту высот с несколькими октавами
   for (int y = 0; y < height; ++y) {
       for (int x = 0; x < width; ++x) {
           float e = 0.0f;
           float amp = 1.0f;
           float freq = 1.0f;
           
           // Добавляем несколько слоев шума
           for (int i = 0; i < 4; i++) {
               e += noise.GetNoise(x * freq, y * freq) * amp;
               amp *= 0.5f;
               freq *= 2.0f;
           }
           
           // Нормализуем значение в диапазон [-1, 1]
           e = std::clamp(e, -1.0f, 1.0f);
           
           WorldTile& tile = getTile(x, y);
           tile.elevation = e;
           tile.baseElevation = e;
       }
   }
}

void WorldMap::generateTemperature() {
   FastNoiseLite noise;
   noise.SetSeed(rng());
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   noise.SetFrequency(0.01f);

   for (int y = 0; y < height; ++y) {
       for (int x = 0; x < width; ++x) {
           // Базовая температура зависит от широты (y координаты)
           float latitudeTemp = 1.0f - std::abs(float(y - height/2) / (height/2));
           latitudeTemp = latitudeTemp * 40.0f - 10.0f; // преобразуем в температуру (-10 до 30)
           
           // Добавляем случайные вариации
           float variation = noise.GetNoise(x * 1.0f, y * 1.0f) * 10.0f;
           
           // Учитываем высоту (понижение температуры с высотой)
           float elevationEffect = getTile(x, y).elevation * -10.0f;
           
           float finalTemp = latitudeTemp + variation + elevationEffect;
           finalTemp = std::clamp(finalTemp, -30.0f, 50.0f);
           
           WorldTile& tile = getTile(x, y);
           tile.temperature = finalTemp;
           tile.baseTemperature = finalTemp;
       }
   }
}

void WorldMap::generateRainfall() {
   FastNoiseLite noise;
   noise.SetSeed(rng());
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   noise.SetFrequency(0.015f);

   for (int y = 0; y < height; ++y) {
       for (int x = 0; x < width; ++x) {
           float r = noise.GetNoise(x * 1.0f, y * 1.0f);
           // Преобразуем в диапазон [0, 1]
           r = (r + 1.0f) * 0.5f;
           
           // Учитываем температуру (более теплый воздух может содержать больше влаги)
           WorldTile& tile = getTile(x, y);
           float tempEffect = (tile.temperature + 30.0f) / 80.0f; // нормализуем температуру
           r *= tempEffect;
           
           // Добавляем эффект горного барьера
           if (tile.elevation > 0.5f) {
               r *= 1.5f; // Больше осадков в горах
           }
           
           tile.rainfall = std::clamp(r, 0.0f, 1.0f);
       }
   }
}

void WorldMap::determineBiomes() {
   for (int y = 0; y < height; ++y) {
       for (int x = 0; x < width; ++x) {
           WorldTile& tile = getTile(x, y);
           
           // Определяем биом на основе температуры, осадков и высоты
           if (tile.elevation > 0.7f) {
               tile.biome = BiomeType::MOUNTAIN;
           }
           else if (tile.temperature < -10.0f) {
               tile.biome = BiomeType::ICE_SHEET;
           }
           else if (tile.temperature < 0.0f) {
               tile.biome = BiomeType::TUNDRA;
           }
           else {
               if (tile.rainfall < 0.2f) {
                   tile.biome = BiomeType::DESERT;
               }
               else if (tile.rainfall < 0.4f) {
                   tile.biome = BiomeType::SAVANNA;
               }
               else if (tile.temperature > 20.0f) {
                   tile.biome = BiomeType::TROPICAL;
               }
               else if (tile.temperature > 5.0f) {
                   tile.biome = BiomeType::TEMPERATE;
               }
               else {
                   tile.biome = BiomeType::BOREAL;
               }
           }
       }
   }
}

const WorldMap::WorldTile& WorldMap::getTile(int x, int y) const {
   return tiles[getIndex(x, y)];
}

WorldMap::WorldTile& WorldMap::getTile(int x, int y) {
   return tiles[getIndex(x, y)];
}

} // namespace game