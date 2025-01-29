#pragma once
#include "../../engine/ecs/Component.hpp"
#include "BiomeType.hpp"
#include "TileProperties.hpp"
#include <unordered_map>
#include <string>

namespace game {
   class ExtendedTileComponent : public engine::Component {
   public:
       ExtendedTileComponent() = default;
       ExtendedTileComponent(TileType type, BiomeType biome) 
           : type(type), biome(biome) {}

       // Основные свойства
       TileType type = TileType::GROUND;
       BiomeType biome = BiomeType::TEMPERATE;
       TileProperties properties;
       
       // Система модификаторов
       std::unordered_map<std::string, float> modifiers;
       
       // Методы для работы с модификаторами
       void addModifier(const std::string& key, float value) {
           modifiers[key] = value;
       }
       
       void removeModifier(const std::string& key) {
           modifiers.erase(key);
       }
       
       float getModifier(const std::string& key) const {
           auto it = modifiers.find(key);
           return it != modifiers.end() ? it->second : 0.0f;
       }

       // Получение итогового значения с учетом всех модификаторов
       float getModifiedValue(const std::string& key, float baseValue) const {
           float totalModifier = 1.0f;
           for (const auto& [modKey, value] : modifiers) {
               if (modKey.find(key) == 0) { // Если модификатор начинается с key
                   totalModifier += value;
               }
           }
           return baseValue * totalModifier;
       }
   };
}