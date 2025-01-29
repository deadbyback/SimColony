#include <filesystem>
#include "TileRegistry.hpp"
#include <fstream>
#include <iostream>

namespace game {

std::filesystem::path findConfigFile(const std::string& filename) {
    std::vector<std::filesystem::path> searchPaths = {
        std::filesystem::current_path() / "content" / "config" / filename,
        std::filesystem::current_path() / ".." / "content" / "config" / filename,
        std::filesystem::current_path() / "config" / filename,
        std::filesystem::path(filename)
    };

    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    throw std::runtime_error("Cannot find configuration file: " + filename);
}

TileRegistry::TileRegistry(engine::ResourceCache& resourceCache) 
    : resourceCache(resourceCache) {
    loadTileConfigs();
}

void TileRegistry::loadTileConfigs() {
    try {
        std::filesystem::path configPath = findConfigFile("tiles.json");
        std::ifstream configFile(configPath);
        
        if (!configFile.is_open()) {
            throw std::runtime_error("Cannot open tiles configuration file: " + configPath.string());
        }

        json configJson;
        configFile >> configJson;

        // Парсим конфигурацию каждого тайла
        for (const auto& [typeStr, tileJson] : configJson["tile_types"].items()) {
            TileConfiguration config;
            config.id = tileJson["id"];
            config.name = tileJson["name"];
            config.texturePath = tileJson["texture"];
            std::cout << config.name << std::endl;
            // Загружаем свойства
            const auto& props = tileJson["properties"];
            config.properties.walkable = props["walkable"];
            config.properties.buildable = props["buildable"];
            config.properties.baseFertility = props["base_fertility"];
            config.properties.baseElevation = props["base_elevation"];

            // Добавляем конфигурацию в map
            TileType type = stringToTileType(typeStr);
            tileConfigs[type] = config;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading tile configurations: " << e.what() << std::endl;
        // В случае ошибки загружаем дефолтные конфигурации
        loadDefaultConfigs();
    }
}

void TileRegistry::loadDefaultConfigs() {
    // Дефолтная конфигурация на случай ошибки загрузки файла
    TileConfiguration groundConfig;
    groundConfig.id = 0;
    groundConfig.name = "Ground";
    groundConfig.texturePath = "tiles/ground/stone_ground.png";
    groundConfig.properties = {true, true, 0.5f, 0.0f};
    tileConfigs[TileType::GROUND] = groundConfig;

    TileConfiguration waterConfig;
    waterConfig.id = 1;
    waterConfig.name = "Water";
    waterConfig.texturePath = "tiles/water/sea_water.png";
    waterConfig.properties = {false, false, 0.0f, -1.0f};
    tileConfigs[TileType::WATER] = waterConfig;
}

TileData TileRegistry::createTileData(TileType type) {
    auto it = tileConfigs.find(type);
    if (it == tileConfigs.end()) {
        return createTileData(TileType::GROUND); // Возвращаем дефолтный тайл
    }

    const auto& config = it->second;
    TileData data;
    data.type = type;
    data.walkable = config.properties.walkable;
    data.texture = resourceCache.getTexture(config.texturePath);
    return data;
}

const TileConfiguration& TileRegistry::getTileConfig(TileType type) const {
    auto it = tileConfigs.find(type);
    if (it != tileConfigs.end()) {
        return it->second;
    }
    throw std::runtime_error("Tile configuration not found for type");
}

TileType TileRegistry::stringToTileType(const std::string& str) {
    if (str == "GROUND") return TileType::GROUND;
    if (str == "WATER") return TileType::WATER;
    if (str == "DIRT") return TileType::DIRT;
    if (str == "GRASS") return TileType::GRASS;
    if (str == "MOUNTAIN") return TileType::MOUNTAIN;
    if (str == "SAND") return TileType::SAND;
    if (str == "SNOW") return TileType::SNOW;
    if (str == "FOREST") return TileType::FOREST;
    // Добавим новые типы тайлов здесь
    
    throw std::runtime_error("Unknown tile type: " + str);
}

} // namespace game