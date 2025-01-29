#pragma once
#include "WorldMap.hpp"
#include "../../engine/ecs/World.hpp"
#include "../../engine/core/ResourceCache.hpp"
#include "../../engine/ecs/systems/TileSystem.hpp"
#include "TileRegistry.hpp"
#include "BiomeType.hpp"
#include "ExtendedTileComponent.hpp"
#include <FastNoiseLite.h>

namespace game {
    class LocalMapGenerator {
    public:
        struct GenerationParams {
            int width = 500;
            int height = 500;
            float detailLevel = 1.0f;      // Уровень детализации для шума
            float roughness = 1.0f;        // Шероховатость ландшафта
            uint32_t seed = 0;             // Сид для генерации
        };

        LocalMapGenerator(engine::ResourceCache& resourceCache, TileRegistry& tileRegistry) 
            : resourceCache(resourceCache), tileRegistry(tileRegistry) {}

        void generateMap(engine::World& world, 
                        engine::TileSystem& tileSystem,
                        const WorldMap::WorldTile& globalTile,
                        const GenerationParams& params = GenerationParams());
        
        void applyBiomeModifiers(const ExtendedTileComponent* tile, BiomeType biome);

        void setTileProperties(const ExtendedTileComponent* extTile, 
                            float elevation, float moisture, 
                            const WorldMap::WorldTile& globalTile);

    private:
        engine::ResourceCache& resourceCache;
        TileRegistry& tileRegistry;
        FastNoiseLite noise;

        // Вспомогательные методы генерации
        float generateElevation(int x, int y, const GenerationParams& params);
        float generateMoisture(int x, int y, const GenerationParams& params);
        TileType determineTileType(float elevation, float moisture, const WorldMap::WorldTile& globalTile);
        TileProperties generateTileProperties(float elevation, float moisture, 
                                           WorldMap::WorldTile& globalTile);
    };
}