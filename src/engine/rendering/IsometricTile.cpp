#include "IsometricTile.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

IsometricTile::IsometricTile(float width, float height) 
    : position(0.0f), scale(1.0f), rotation(0.0f), model(1.0f) {
    setupMesh(width, height);
    updateModelMatrix();
}

IsometricTile::~IsometricTile() {
    // Очищаем буферы OpenGL
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void IsometricTile::setupMesh(float width, float height) {
    // В изометрической проекции тайл представляет собой ромб
    // Вычисляем половину ширины и высоты для центрированного тайла
    float hw = width * 0.5f;  // half width
    float hh = height * 0.5f; // half height

    // Определяем вершины тайла (позиция + текстурные координаты)
    float vertices[] = {
        // Позиции            // Текстурные координаты
        -hw,  0.0f, -hh,     0.0f, 0.0f,  // Нижняя левая
         hw,  0.0f, -hh,     1.0f, 0.0f,  // Нижняя правая
         hw,  0.0f,  hh,     1.0f, 1.0f,  // Верхняя правая
        -hw,  0.0f,  hh,     0.0f, 1.0f   // Верхняя левая
    };

    // Индексы для отрисовки двух треугольников, составляющих тайл
    unsigned int indices[] = {
        0, 1, 2,  // Первый треугольник
        2, 3, 0   // Второй треугольник
    };

    // Создаем и настраиваем буферы OpenGL
    glGenVertexArrays(1, &VAO);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "Error generating VAO: " << error << std::endl;
    }
    
    glGenBuffers(1, &VBO);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "Error generating VBO: " << error << std::endl;
    }
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Загружаем вершины
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Загружаем индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Настраиваем атрибуты вершин
    // Позиция (3 float значения)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Текстурные координаты (2 float значения)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Проверяем ошибки OpenGL
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error in setupMesh: " << error << std::endl;
    }
}

void IsometricTile::updateModelMatrix() {
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, 0.0f, position.y));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale.x, 1.0f, scale.y));
}

void IsometricTile::draw(const std::shared_ptr<Shader>& shader) {
    GLenum error;

    // Проверяем ошибки перед отрисовкой
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error before drawing: " << error << std::endl;
    }

    if (VAO == 0) {
        std::cerr << "Invalid VAO" << std::endl;
        return;
    }
    
    shader->use();
    // Смещаем матрицу модели по Z в зависимости от позиции
    glm::mat4 modelWithOffset = glm::translate(
        model, 
        glm::vec3(0.0f, 0.0f, depthOffset)
    );
    shader->setMat4("model", modelWithOffset);
    shader->setInt("tileTexture", 0);
    shader->setBool("isHighlighted", isHighlighted);

    glDepthMask(GL_FALSE);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    if (texture) {
        texture->bind(0);
        shader->setInt("tileTexture", 0);
    }
    
    glBindVertexArray(VAO);
    if (error != GL_NO_ERROR) {
        std::cerr << "Error binding VAO: " << error << std::endl;
    }
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "Error drawing elements: " << error << std::endl;
    }
    
    glBindVertexArray(0);

    // Проверяем ошибки после отрисовки
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error after drawing: " << error << std::endl;
    }
}

void IsometricTile::setPosition(const glm::vec2& pos) {
    position = pos;
    updateModelMatrix();
}

void IsometricTile::setScale(const glm::vec2& s) {
    scale = s;
    updateModelMatrix();
}

void IsometricTile::setRotation(float rot) {
    rotation = rot;
    updateModelMatrix();
}

void IsometricTile::setTexture(const std::shared_ptr<Texture>& tex) {
    texture = tex;
};

glm::mat4 IsometricTile::getModelMatrix() {
    return model;
}