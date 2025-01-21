#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.hpp"
#include "Texture.hpp"

class IsometricTile {
public:
    IsometricTile(float width = 1.0f, float height = 1.0f);
    ~IsometricTile();

    // Метод для отрисовки тайла
    void draw(const std::shared_ptr<Shader>& shader);

    // Методы для управления позицией и трансформацией
    void setPosition(const glm::vec2& position);
    void setScale(const glm::vec2& scale);
    void setRotation(float rotation);

    // Метод установки текстуры
    void setTexture(const std::shared_ptr<Texture>& tex);

    void setDepthOffset(float offset) {
        depthOffset = offset;
    }

    void setHighlighted(bool highlight) { isHighlighted = highlight; }
    bool getHighlighted() const { return isHighlighted; }

    // Getter
    glm::mat4 getModelMatrix();
private:
    // OpenGL объекты для хранения геометрии
    GLuint VAO; // Vertex Array Object
    GLuint VBO; // Vertex Buffer Object
    GLuint EBO; // Element Buffer Object

    // Матрицы трансформации
    glm::mat4 model;
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;

    float depthOffset = 0.0f;
    bool isHighlighted = false;

    std::shared_ptr<Texture> texture;

    // Вспомогательные методы
    void setupMesh(float width, float height);
    void updateModelMatrix();
};