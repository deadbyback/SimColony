#include "ResourceCache.hpp"
#include "../rendering/Shader.hpp"
#include "../rendering/Texture.hpp"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace engine {

ResourceCache::ResourceCache() {}

ResourceCache::~ResourceCache() {
    clear();
}

template<typename T>
std::shared_ptr<T> ResourceCache::load(const std::string& path) {
    return getCached<T>(path);
}

template<typename T>
std::shared_ptr<T> ResourceCache::getCached(const std::string& path) {
    auto& typeCache = m_cache[std::type_index(typeid(T))];
    
    // Проверяем, есть ли ресурс в кэше
    auto it = typeCache.find(path);
    if (it != typeCache.end()) {
        if (auto resource = it->second.lock()) {
            return std::static_pointer_cast<T>(resource);
        } else {
            // Если weak_ptr истек, удаляем его из кэша
            typeCache.erase(it);
        }
    }

    // Загружаем новый ресурс
    auto resource = loadResource<T>(path);
    typeCache[path] = resource;
    return resource;
}

std::shared_ptr<Shader> ResourceCache::getShader(const std::string& name) {
    // Проверяем, есть ли шейдер уже в кэше
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }

    // Если нет, загружаем новый шейдер
    auto shader = std::make_shared<Shader>(name);
    m_shaders[name] = shader;
    return shader;
}

std::shared_ptr<Texture> ResourceCache::getTexture(const std::string& path) {
    // Проверяем, есть ли текстура уже в кэше
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    // Если нет, загружаем новую текстуру
    std::string fullPath = "../../src/engine/rendering/" + path;
    std::cout << "Loading texture: " << fullPath << std::endl;
    auto texture = std::make_shared<Texture>(fullPath);
    m_textures[path] = texture;
    return texture;
}

void ResourceCache::clear() {
    m_cache.clear();
    m_shaders.clear();
    m_textures.clear();
}

void ResourceCache::clearUnused() {
    for (auto& [type, typeCache] : m_cache) {
        for (auto it = typeCache.begin(); it != typeCache.end();) {
            if (it->second.expired()) {
                it = typeCache.erase(it);
            } else {
                ++it;
            }
        }
    }
}

// Специализации для разных типов ресурсов

template<>
std::shared_ptr<Shader> ResourceCache::loadResource<Shader>(const std::string& name) {
    return std::make_shared<Shader>(name);
}

template<>
std::shared_ptr<Texture> ResourceCache::loadResource<Texture>(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Texture file not found: " + path);
    }
    return std::make_shared<Texture>(path);
}

// Явные инстанциации шаблонов для поддерживаемых типов
template std::shared_ptr<Shader> ResourceCache::load<Shader>(const std::string&);
template std::shared_ptr<Texture> ResourceCache::load<Texture>(const std::string&);

} // namespace engine
