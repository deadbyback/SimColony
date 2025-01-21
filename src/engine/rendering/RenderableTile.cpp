#include "RenderableTile.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

RenderableTile::RenderableTile(float width, float height) : width(width), height(height) {
    initializeBuffers();
}

RenderableTile::~RenderableTile() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void RenderableTile::initializeBuffers() {
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

void RenderableTile::setPosition(const glm::vec2& pos) {
    position = pos;
}

void RenderableTile::setTexture(const std::shared_ptr<Texture>& tex) {
    texture = tex;
}

void RenderableTile::draw(const std::shared_ptr<Shader>& shader) {
    if (!shader || !texture) return;

    shader->use();
    
    // Устанавливаем матрицу модели для позиционирования тайла
    glm::mat4 model = glm::mat4(1.0f);
    // Смещаем тайл на половину его размера для центрирования
    model = glm::translate(model, glm::vec3(position.x - width/2.0f, position.y - height/2.0f, 0.0f));
    shader->setMat4("model", model);

    // Устанавливаем цвет подсветки
    shader->setVec4("highlightColor", isHighlighted ? 
        glm::vec4(1.0f, 1.0f, 0.0f, 0.3f) :  // Желтая подсветка
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));  // Без подсветки
    shader->setFloat("isHighlighted", isHighlighted ? 1.0f : 0.0f);

    // Привязываем текстуру
    texture->bind();

    // Отрисовываем тайл
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
