#pragma once
#include "../Component.hpp"
#include "../../core/Renderer.hpp"
#include "../components/TransformComponent.hpp"
#include "../components/RenderableComponent.hpp"
#include "../World.hpp"

namespace engine {
    class RenderSystem {
    public:
        explicit RenderSystem(Renderer& renderer) : renderer(renderer) {}

        void render(World& world) {
            auto renderables = world.getComponents<RenderableComponent>();
            
            for (auto* renderable : renderables) {
                auto* transform = world.getEntity(renderable->getOwner())
                    ->getComponent<TransformComponent>();
                
                if (!transform) continue;

                renderer.drawTile(
                    transform->position,
                    renderable->size,
                    renderable->texture,
                    renderable->isHighlighted,
                    renderable->highlightColor
                );
            }
        }

    private:
        Renderer& renderer;
    };
}