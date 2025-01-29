#pragma once
#include "../Component.hpp"
#include "../../rendering/Texture.hpp"
#include "../../rendering/Shader.hpp"
#include <memory>

namespace engine {
    class RenderableComponent : public Component {
    public:
        std::shared_ptr<Texture> texture;
        std::shared_ptr<Shader> shader;
        glm::vec2 size{1.0f};
        glm::vec4 color{1.0f};
        mutable bool isHighlighted = false;
        glm::vec4 highlightColor{1.0f, 1.0f, 0.0f, 0.3f};

        RenderableComponent() = default;
        RenderableComponent(
            const std::shared_ptr<Texture>& tex,
            const std::shared_ptr<Shader>& sh,
            const glm::vec2& sz = glm::vec2(1.0f)
        ) : texture(tex), shader(sh), size(sz) {}
    };
}