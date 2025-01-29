#pragma once
#include "../World.hpp"
#include "../components/TileComponent.hpp"
#include "../components/TransformComponent.hpp"
#include "../components/RenderableComponent.hpp"
#include "../../../game/world/ExtendedTileComponent.hpp"

namespace engine {
    class TileSystem {
    public:
        Entity* createTile(World& world, const TileData& data, const GridPosition& pos) {
            Entity* entity = world.createEntity();
            
            auto* tile = entity->addComponent<TileComponent>(data.type, data.walkable, pos);
            auto* transform = entity->addComponent<TransformComponent>();
            auto* renderable = entity->addComponent<RenderableComponent>();

            transform->position = gridToWorld(pos);
            renderable->texture = data.texture;
            renderable->size = glm::vec2(TILE_SIZE);

            auto* extTile = entity->addComponent<game::ExtendedTileComponent>();
            extTile->type = data.type;
            extTile->properties.walkable = data.walkable;
            extTile->properties.buildable = data.buildable;
            extTile->properties.elevation = data.elevation;
            extTile->properties.fertility = data.fertility;
            extTile->properties.humidity = data.humidity;
            extTile->properties.temperature = data.temperature;

            return entity;
        }

        static glm::vec2 gridToWorld(const GridPosition& pos) {
            return glm::vec2(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
        }

        static GridPosition worldToGrid(const glm::vec2& worldPos) {
            return GridPosition{
                static_cast<int>(std::floor(worldPos.x / TILE_SIZE)),
                static_cast<int>(std::floor(worldPos.y / TILE_SIZE))
            };
        }
    private:
        static constexpr float TILE_SIZE = 1.0f;
    };
}