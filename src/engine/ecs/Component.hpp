#pragma once
#include <cstdint>

namespace engine {
    using EntityID = std::uint32_t;
    
    class Component {
    public:
        virtual ~Component() = default;
        EntityID getOwner() const { return ownerID; }
    protected:
        EntityID ownerID = 0;
        friend class Entity;
        friend class World;
        friend class TileSystem;
    };
}