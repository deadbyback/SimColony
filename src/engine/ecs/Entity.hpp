#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include "Component.hpp"

namespace engine {
    class Entity {
    public:
        explicit Entity(EntityID id) : id(id) {}
        
        template<typename T, typename... Args>
        T* addComponent(Args&&... args) {
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            component->ownerID = id;
            auto ptr = component.get();
            components[std::type_index(typeid(T))] = std::move(component);
            return ptr;
        }

        template<typename T>
        const T* getComponent() const {
            auto it = components.find(std::type_index(typeid(T)));
            return it != components.end() ? static_cast<T*>(it->second.get()) : nullptr;
        }

        EntityID getID() const { return id; }

    private:
        EntityID id;
        std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
        friend class World;
    };
}