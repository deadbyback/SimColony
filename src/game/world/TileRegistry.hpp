#pragma once
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include "../../engine/core/ResourceCache.hpp"
#include "../../game/Tile.hpp"

using json = nlohmann::json;

namespace game {
    struct TileConfiguration {
        int id;
        std::string name;
        std::string texturePath;
        struct {
            bool walkable;
            bool buildable;
            float baseFertility;
            float baseElevation;
        } properties;
    };

    class TileRegistry {
    public:
        TileRegistry(engine::ResourceCache& resourceCache);
        
        TileData createTileData(TileType type);
        const TileConfiguration& getTileConfig(TileType type) const;

    private:
        engine::ResourceCache& resourceCache;
        std::unordered_map<TileType, TileConfiguration> tileConfigs;

        void loadTileConfigs();
        void loadDefaultConfigs();
        TileType stringToTileType(const std::string& str);
    };
}