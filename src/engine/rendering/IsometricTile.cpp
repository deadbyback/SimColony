#include "IsometricTile.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

IsometricTile::IsometricTile(float width, float height) 
    : model(1.0f)
    , position(0.0f)
    , scale(1.0f)
    , rotation(0.0f)
    , depthOffset(0.0f)
    , isHighlighted(false) {
    setupMesh(width, height);
}

IsometricTile::~IsometricTile() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void IsometricTile::draw(const std::shared_ptr<engine::Shader>& shader) {
    if (!shader || !texture) return;

    shader->use();

    // Обновляем матрицу модели
    updateModelMatrix();
    shader->setMat4("model", model);

    // Устанавливаем подсветку
    shader->setFloat("isHighlighted", isHighlighted ? 1.0f : 0.0f);

    // Привязываем текстуру
    texture->bind();

    // Отрисовываем тайл
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void IsometricTile::setPosition(const glm::vec2& pos) {
    position = pos;
}

void IsometricTile::setScale(const glm::vec2& s) {
    scale = s;
}

void IsometricTile::setRotation(float rot) {
    rotation = rot;
}

void IsometricTile::setTexture(const std::shared_ptr<engine::Texture>& tex) {
    texture = tex;
}

glm::mat4 IsometricTile::getModelMatrix() {
    updateModelMatrix();
    return model;
}

void IsometricTile::setupMesh(float width, float height) {
    float vertices[] = {
        // Позиции          // Текстурные координаты
        0.0f,    0.0f,     0.0f, 0.0f,  // Нижний левый
        width,   0.0f,     1.0f, 0.0f,  // Нижний правый
        width,   height,   1.0f, 1.0f,  // Верхний правый
        0.0f,    height,   0.0f, 1.0f   // Верхний левый
    };

    unsigned int indices[] = {
        0, 1, 2,  // Первый треугольник
        0, 2, 3   // Второй треугольник
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Позиции
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Текстурные координаты
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void IsometricTile::updateModelMatrix() {
    model = glm::mat4(1.0f);

    // Перемещаем в позицию
    model = glm::translate(model, glm::vec3(position.x, position.y, depthOffset));

    // Поворачиваем
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

    // Масштабируем
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
}

} // namespace engine