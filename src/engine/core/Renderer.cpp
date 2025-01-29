#include "Renderer.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace engine {

namespace {
    constexpr size_t MAX_BATCH_SIZE = 1000;
}

    Renderer::Renderer() {
        initializeBuffers();
        initializeShaders();
    }

    void Renderer::initializeBuffers() {
        // Создаем VAO и VBO для спрайта
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glGenBuffers(1, &m_quadEBO);
        glGenBuffers(1, &m_instanceVBO);

        glBindVertexArray(m_quadVAO);

        // Вершины для квадрата
        float vertices[] = {
            // Позиции        // Текстурные координаты
            0.0f, 0.0f,      0.0f, 0.0f,  // Нижний левый
            1.0f, 0.0f,      1.0f, 0.0f,  // Нижний правый
            1.0f, 1.0f,      1.0f, 1.0f,  // Верхний правый
            0.0f, 1.0f,      0.0f, 1.0f   // Верхний левый
        };

        // Индексы для двух треугольников
        unsigned int indices[] = {
            0, 1, 2,  // Первый треугольник
            0, 2, 3   // Второй треугольник
        };

        // Загружаем вершины
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Загружаем индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Настраиваем атрибуты вершин
        // Позиция
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Текстурные координаты
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Буфер инстансов
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE * sizeof(TileBatchItem), nullptr, GL_DYNAMIC_DRAW);

        // Позиция инстанса
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TileBatchItem), (void*)offsetof(TileBatchItem, position));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);

        // Размер инстанса
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TileBatchItem), (void*)offsetof(TileBatchItem, size));
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);

        // isHighlighted
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(TileBatchItem), (void*)offsetof(TileBatchItem, isHighlighted));
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);

        // highlightColor
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(TileBatchItem), (void*)offsetof(TileBatchItem, highlightColor));
        glEnableVertexAttribArray(5);
        glVertexAttribDivisor(5, 1);

        glBindVertexArray(0);
    }

    void Renderer::initializeShaders() {
        m_spriteShader = std::make_shared<Shader>("Sprite");
        m_tileShader = std::make_shared<Shader>("Tile");
    }

    void Renderer::beginFrame() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_currentBatch = BatchData();
    }

    void Renderer::endFrame() {
        flushBatch();
    }

    void Renderer::setViewProjection(const glm::mat4& viewProjection) {
        m_viewProjection = viewProjection;
    }

    void Renderer::cacheTile(int x, int y, const glm::vec2& worldPos, const std::shared_ptr<Texture>& texture) {
        TileCacheKey key{x, y};
        CachedTileData data{worldPos, texture};
        m_tileCache[key] = data;
    }

    void Renderer::clearTileCache() {
        m_tileCache.clear();
    }

    bool Renderer::isTileCached(int x, int y) const {
        return m_tileCache.find(TileCacheKey{x, y}) != m_tileCache.end();
    }

    const CachedTileData* Renderer::getCachedTile(int x, int y) const {
        auto it = m_tileCache.find(TileCacheKey{x, y});
        if (it != m_tileCache.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void Renderer::drawSprite(const glm::vec2& position, const glm::vec2& size,
                            const std::shared_ptr<Texture>& texture, const glm::vec4& color,
                            bool highlighted) {
        m_spriteShader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

        m_spriteShader->setMat4("uModel", model);
        m_spriteShader->setMat4("uViewProjection", m_viewProjection);
        m_spriteShader->setVec4("uColor", color);

        if (texture) {
            glActiveTexture(GL_TEXTURE0);
            texture->bind();
            m_spriteShader->setInt("uTexture", 0);
        }

        glBindVertexArray(m_quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Renderer::drawTile(const glm::vec2& position, const glm::vec2& size,
                        const std::shared_ptr<Texture>& texture, bool highlighted,
                        const glm::vec4& highlightColor) {
        // Если текстура изменилась или достигнут максимальный размер батча, сбрасываем текущий батч
        if (m_currentBatch.texture && m_currentBatch.texture != texture) {
            flushBatch();
        }

        // Добавляем тайл в текущий батч
        TileBatchItem item;
        item.position = position;
        item.size = size;
        item.isHighlighted = highlighted ? 1.0f : 0.0f;
        item.highlightColor = highlightColor;

        if (m_currentBatch.items.empty()) {
            m_currentBatch.texture = texture;
        }
        m_currentBatch.items.push_back(item);

        // Если достигнут максимальный размер батча, сбрасываем его
        if (m_currentBatch.items.size() >= MAX_BATCH_SIZE) {
            flushBatch();
        }
    }

    void Renderer::flushBatch() {
        if (m_currentBatch.items.empty()) {
            return;
        }

        m_tileShader->use();
        m_tileShader->setMat4("view", glm::mat4(1.0f));
        m_tileShader->setMat4("projection", m_viewProjection);

        if (m_currentBatch.texture) {
            glActiveTexture(GL_TEXTURE0);
            m_currentBatch.texture->bind();
            m_tileShader->setInt("texture1", 0);
        }

        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    m_currentBatch.items.size() * sizeof(TileBatchItem),
                    m_currentBatch.items.data());

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 
                            static_cast<GLsizei>(m_currentBatch.items.size()));
        glBindVertexArray(0);

        m_currentBatch = BatchData();
    }

} // namespace engine
