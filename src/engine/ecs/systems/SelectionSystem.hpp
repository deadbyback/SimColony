#pragma once
#include "../World.hpp"
#include "../components/TileComponent.hpp"
#include "../components/RenderableComponent.hpp"

namespace engine {
    class SelectionSystem {
    public:
        void updateSelection(World& world, const GridPosition& hoveredPos) {
            selectedTile = nullptr;
            auto tiles = world.getComponents<TileComponent>();
            
            for (auto* tile : tiles) {
                auto* renderable = world.getEntity(tile->getOwner())
                    ->getComponent<RenderableComponent>();
                
                if (tile->gridPosition.x == hoveredPos.x && 
                    tile->gridPosition.y == hoveredPos.y) {
                    renderable->isHighlighted = true;
                    selectedTile = tile;
                } else {
                    renderable->isHighlighted = false;
                }
            }
        }

        const TileComponent* getSelectedTile() const { return selectedTile; }

    private:
        TileComponent* selectedTile = nullptr;
    };
}