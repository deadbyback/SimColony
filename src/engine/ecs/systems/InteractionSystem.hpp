#pragma once
#include "../World.hpp"
#include "../components/TileComponent.hpp"
#include <functional>

namespace engine {
   class InteractionSystem {
   public:
       using TileClickCallback = std::function<void(Entity*, const TileComponent*)>;

       void processClick(World& world, const GridPosition& clickPos, const TileClickCallback& callback) {
           auto tiles = world.getComponents<TileComponent>();
           
           for (const auto* tile : tiles) {
               if (tile->gridPosition.x == clickPos.x && 
                   tile->gridPosition.y == clickPos.y) {
                   Entity* entity = const_cast<Entity*>(world.getEntity(tile->getOwner()));
                   // Pass the const tile to the callback
                   callback(entity, tile);
                   break;
               }
           }
       }
   };
}