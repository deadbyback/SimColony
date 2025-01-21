#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float initialZoom = 20.0f, float aspect = 1280.0f / 720.0f);

    // Методы для управления камерой
    void moveRight(float deltaTime);
    void moveLeft(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);
    void zoom(float offset);  // Положительное значение - приближение, отрицательное - отдаление

    void focusOn(const glm::vec2& target);  // Фокусировка на точке
    void update(float deltaTime);           // Обновление состояния камеры

    // Получение матрицы вида
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    glm::mat4 Camera::getScreenToWorldMatrix(const glm::vec2& windowSize) const;
    glm::vec3 screenToWorld(const glm::vec2& screenPos, const glm::vec2& windowSize) const;
    glm::vec3 getPosition() const { return position; }
    float getZoomLevel() const { return zoomLevel; }

private:
    glm::vec3 position;            // Позиция камеры
    glm::vec3 targetPosition;      // Целевая позиция для плавного перемещения
    bool isMovingToTarget;         // Флаг движения к цели
    float smoothSpeed;             // Скорость плавного перемещения
    float zoomLevel;               // Уровень приближения
    float aspect;                  // Соотношение сторон окна
    const float moveSpeed;         // Скорость перемещения камеры
    const float zoomSpeed;         // Скорость зума
    const float rotationX;         // Угол поворота вокруг оси X (фиксированный для изометрии)
    const float rotationY;         // Угол поворота вокруг оси Y (фиксированный для изометрии)
    glm::mat4 viewMatrix;          // Матрица вида

    void updateViewMatrix();
    
    glm::vec4 unproject(const glm::vec3& screenPos, const glm::vec2& windowSize) const;
};