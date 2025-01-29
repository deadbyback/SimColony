#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include "../rendering/Shader.hpp"
#include "../rendering/Texture.hpp"

namespace engine {

    struct TileBatchItem {
        glm::vec2 position;
        glm::vec2 size;
        float isHighlighted;
        glm::vec4 highlightColor;
    };

    struct BatchData {
        std::vector<TileBatchItem> items;
        std::shared_ptr<Texture> texture;
    };

    // Ключ для кэша тайлов
    struct TileCacheKey {
        int x;
        int y;
        bool operator==(const TileCacheKey& other) const {
            return x == other.x && y == other.y;
        }
    };

    // Хэш-функция для TileCacheKey
    struct TileCacheKeyHash {
        std::size_t operator()(const TileCacheKey& key) const {
            return std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1);
        }
    };

    // Данные кэшированного тайла
    struct CachedTileData {
        glm::vec2 worldPosition;  // Позиция в мировых координатах
        std::shared_ptr<Texture> texture;
    };

    class Renderer {
    public:
        Renderer();
        ~Renderer() = default;

        void beginFrame();
        void endFrame();
        void setViewProjection(const glm::mat4& viewProjection);

        void drawSprite(const glm::vec2& position, const glm::vec2& size,
                    const std::shared_ptr<Texture>& texture, const glm::vec4& color,
                    bool highlighted = false);

        void drawTile(const glm::vec2& position, const glm::vec2& size,
                    const std::shared_ptr<Texture>& texture, bool highlighted,
                    const glm::vec4& highlightColor);

        // Новые методы для работы с кэшем
        void cacheTile(int x, int y, const glm::vec2& worldPos, const std::shared_ptr<Texture>& texture);
        void clearTileCache();
        bool isTileCached(int x, int y) const;
        const CachedTileData* getCachedTile(int x, int y) const;

    private:
        void initializeBuffers();
        void initializeShaders();
        void flushBatch();

        unsigned int m_quadVAO;
        unsigned int m_quadVBO;
        unsigned int m_quadEBO;
        unsigned int m_instanceVBO;

        glm::mat4 m_viewProjection{1.0f};
        std::shared_ptr<Shader> m_spriteShader;
        std::shared_ptr<Shader> m_tileShader;

        BatchData m_currentBatch;

        // Кэш тайлов
        std::unordered_map<TileCacheKey, CachedTileData, TileCacheKeyHash> m_tileCache;
    };

} // namespace engine
