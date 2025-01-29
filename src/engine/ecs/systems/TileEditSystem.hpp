#pragma once
#include "../World.hpp"
#include "../components/TileComponent.hpp"
#include "../components/RenderableComponent.hpp"

namespace engine {
    class TileEditSystem {
    public:
        void changeTileType(World& world, Entity* entity, TileType newType, 
                        const std::shared_ptr<Texture>& texture) {
            if (!entity) return;
            
            // Remove const to modify components
            auto* tile = const_cast<TileComponent*>(entity->getComponent<TileComponent>());
            auto* renderable = const_cast<RenderableComponent*>(entity->getComponent<RenderableComponent>());
            if (tile && renderable) {
                tile->type = newType;
                tile->walkable = (newType == TileType::GROUND);
                renderable->texture = texture;
            }
        }
    };
}