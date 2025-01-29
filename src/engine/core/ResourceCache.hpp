#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <any>
#include "../rendering/Texture.hpp"
#include "../rendering/Shader.hpp"

namespace engine {

    class Shader;
    class Texture;

    class ResourceCache {
    public:
        ResourceCache() {
            // Базовые пути для ресурсов
            contentPath = "../../content/";
            texturePath = contentPath + "textures/";
            shaderPath = contentPath + "shaders/";
        }
        ~ResourceCache();

        // Запрещаем копирование
        ResourceCache(const ResourceCache&) = delete;
        ResourceCache& operator=(const ResourceCache&) = delete;

        // Шаблонный метод для загрузки ресурсов
        template<typename T>
        std::shared_ptr<T> load(const std::string& path);

        // Специализированные методы для часто используемых ресурсов
        std::shared_ptr<Shader> getShader(const std::string& name);
        std::shared_ptr<Texture> getTexture(const std::string& path);

        // Очистка ресурсов
        void clear();
        void clearUnused();

    private:
        std::string contentPath;
        std::string texturePath;
        std::string shaderPath;
        // Кэш ресурсов, использует type_index для хранения разных типов
        std::unordered_map<std::type_index, 
                        std::unordered_map<std::string, std::weak_ptr<void>>> m_cache;

        // Кэш шейдеров и текстур
        std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

        // Вспомогательные методы
        template<typename T>
        std::shared_ptr<T> loadResource(const std::string& path);
        
        template<typename T>
        std::shared_ptr<T> getCached(const std::string& path);
    };

} // namespace engine
