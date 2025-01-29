#pragma once
#include "../Component.hpp"
#include <glm/glm.hpp>

namespace engine {
    class TransformComponent : public Component {
    public:
        glm::vec2 position{0.0f};
        glm::vec2 scale{1.0f};
        float rotation{0.0f};

        TransformComponent() = default;
        TransformComponent(const glm::vec2& pos) : position(pos) {}
    };
}