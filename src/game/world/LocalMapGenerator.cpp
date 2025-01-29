#include "LocalMapGenerator.hpp"
#include "TileRegistry.hpp"
#include "ExtendedTileComponent.hpp"
#include "BiomeType.hpp"
#include <random>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace game {

void LocalMapGenerator::generateMap(engine::World& world, 
                                 engine::TileSystem& tileSystem,
                                 const WorldMap::WorldTile& globalTile,
                                 const GenerationParams& params) {
   std::random_device rd;
   uint32_t seed = params.seed == 0 ? rd() : params.seed;
   
   // Pre-generate noise maps
   std::vector<float> elevationMap(params.width * params.height);
   std::vector<float> moistureMap(params.width * params.height);
   std::vector<TileType> tileTypeMap(params.width * params.height);
   std::vector<TileData> tileDataMap(params.width * params.height);
   
   noise.SetSeed(seed);
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   noise.SetFrequency(0.02f * params.detailLevel);

   // Parallel generation of noise maps
   //#pragma omp parallel for
   for (int y = 0; y < params.height; ++y) {
       for (int x = 0; x < params.width; ++x) {
           int index = y * params.width + x;
           elevationMap[index] = generateElevation(x, y, params);
           moistureMap[index] = generateMoisture(x, y, params);
           tileTypeMap[index] = determineTileType(elevationMap[index], moistureMap[index], globalTile);
           tileDataMap[index] = tileRegistry.createTileData(tileTypeMap[index]);
       }
   }

   // Batch entity creation
   std::vector<engine::Entity*> createdEntities;
   createdEntities.reserve(params.width * params.height);

   for (int y = 0; y < params.height; ++y) {
       for (int x = 0; x < params.width; ++x) {
           int index = y * params.width + x;
           auto* entity = tileSystem.createTile(world, tileDataMap[index], {x, y});
           createdEntities.push_back(entity);
           
           if (auto* extTile = entity->getComponent<ExtendedTileComponent>()) {
               if (tileRegistry.getTileConfig(tileTypeMap[index]).id != 0) {
                   setTileProperties(extTile, elevationMap[index], moistureMap[index], globalTile);
                   applyBiomeModifiers(extTile, globalTile.biome);
               }
           }
       }
   }
}

float LocalMapGenerator::generateElevation(int x, int y, const GenerationParams& params) {
   float e = 0.0f;
   float amp = 1.0f;
   float freq = 1.0f;
   
   for (int i = 0; i < 4; i++) {
       e += noise.GetNoise(x * freq, y * freq) * amp;
       amp *= 0.5f;
       freq *= 2.0f;
   }
   
   e *= params.roughness;
   return std::clamp(e, -1.0f, 1.0f);
}

float LocalMapGenerator::generateMoisture(int x, int y, const GenerationParams& params) {
   FastNoiseLite moistureNoise = noise;
   moistureNoise.SetSeed(1234); // Временный константный сид
   moistureNoise.SetFrequency(0.015f * params.detailLevel);

   float m = moistureNoise.GetNoise(x * 1.0f, y * 1.0f);
   return (m + 1.0f) * 0.5f;
}

TileType LocalMapGenerator::determineTileType(float elevation, float moisture, 
                                           const WorldMap::WorldTile& globalTile) {
    if (elevation < -0.2f) return TileType::WATER;
    
    switch(globalTile.biome) {
        case BiomeType::DESERT:
            return TileType::SAND;
        case BiomeType::TUNDRA:
            return TileType::SNOW;
        case BiomeType::TROPICAL:
            return moisture > 0.6f ? TileType::FOREST : TileType::GRASS;
        case BiomeType::MOUNTAIN:
            return TileType::MOUNTAIN;
        default:
            if (moisture < 0.2f) return TileType::DIRT;
            if (moisture > 0.6f) return TileType::GRASS;
            return TileType::GROUND;
    }
}

void LocalMapGenerator::setTileProperties(const ExtendedTileComponent* extTile, 
                                    float elevation, float moisture, 
                                    const WorldMap::WorldTile& globalTile) {
    // Use const_cast to modify properties
    auto* mutableTile = const_cast<ExtendedTileComponent*>(extTile);
    if (!mutableTile) return;

    mutableTile->properties.walkable = elevation >= -0.2f;
    mutableTile->properties.buildable = elevation >= -0.2f && elevation <= 0.7f;
    mutableTile->properties.elevation = elevation;
    mutableTile->properties.fertility = std::max(0.0f, moisture * (1.0f - std::abs(elevation)));
    mutableTile->properties.temperature = globalTile.temperature - (elevation * 10.0f);
    mutableTile->properties.humidity = (moisture + globalTile.rainfall) * 0.5f;
}

void LocalMapGenerator::applyBiomeModifiers(const ExtendedTileComponent* tile, BiomeType biome) {
    // Use const_cast to modify the tile properties
    auto* mutableTile = const_cast<ExtendedTileComponent*>(tile);
    
    if (!mutableTile) return;

    switch(biome) {
        case BiomeType::DESERT:
            mutableTile->addModifier("temperature", 0.3f);
            mutableTile->addModifier("fertility", -0.4f);
            break;
        case BiomeType::TROPICAL:
            mutableTile->addModifier("fertility", 0.2f);
            mutableTile->addModifier("humidity", 0.3f);
            break;
        case BiomeType::TUNDRA:
            mutableTile->addModifier("temperature", -0.3f);
            mutableTile->addModifier("fertility", -0.2f);
            break;
        case BiomeType::BOREAL:
            mutableTile->addModifier("temperature", -0.1f);
            mutableTile->addModifier("humidity", 0.1f);
            break;
        case BiomeType::SAVANNA:
            mutableTile->addModifier("temperature", 0.2f);
            mutableTile->addModifier("humidity", -0.2f);
            break;
    }
}

TileProperties LocalMapGenerator::generateTileProperties(float elevation, float moisture, 
                                                      WorldMap::WorldTile& globalTile) {
   TileProperties props;
   
   props.elevation = elevation;
   props.walkable = elevation >= -0.2f;
   props.buildable = elevation >= -0.2f && elevation <= 0.7f;
   props.fertility = std::max(0.0f, moisture * (1.0f - std::abs(elevation)));
   props.temperature = globalTile.temperature - (elevation * 10.0f);
   props.humidity = (moisture + globalTile.rainfall) * 0.5f;
   
   return props;
}

} // namespace game