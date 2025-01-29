#pragma once
#include "../Component.hpp"
#include "../../../game/Tile.hpp"

namespace engine {
    class TileComponent : public Component {
    public:
        TileType type = TileType::GROUND;
        bool walkable = true;
        GridPosition gridPosition{0, 0};

        TileComponent() = default;
        TileComponent(TileType t, bool w, const GridPosition& pos) 
            : type(t), walkable(w), gridPosition(pos) {}
    };
}