#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "Entity.hpp"

namespace engine {
    class World {
    public:
        Entity* createEntity() {
            auto entity = std::make_unique<Entity>(nextEntityID++);
            auto ptr = entity.get();
            entities[entity->getID()] = std::move(entity);
            return ptr;
        }

        void destroyEntity(EntityID id) {
            entities.erase(id);
        }

        Entity* getEntity(EntityID id) const {
            auto it = entities.find(id);
            return it != entities.end() ? it->second.get() : nullptr;
        }

        template<typename T>
        std::vector<T*> getComponents() const {
            std::vector<T*> result;
            for (const auto& [id, entity] : entities) {
                // Use const_cast to remove const-ness when adding to result
                if (auto comp = const_cast<T*>(entity->getComponent<T>())) {
                    result.push_back(comp);
                }
            }
            return result;
        }

    private:
        EntityID nextEntityID = 1;
        std::unordered_map<EntityID, std::unique_ptr<Entity>> entities;
    };
}