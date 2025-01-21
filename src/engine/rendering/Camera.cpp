#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.hpp"
#include "TileMap.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(float initialZoom, float aspect) 
    : position(0.0f, 0.0f, -30.0f)
    , targetPosition(0.0f, 0.0f, -30.0f)
    , isMovingToTarget(false)
    , smoothSpeed(5.0f)
    , zoomLevel(initialZoom)
    , aspect(aspect)
    , moveSpeed(15.0f)
    , zoomSpeed(2.0f)
    , rotationX(glm::radians(0.0f))
    , rotationY(glm::radians(0.0f)) {
    updateViewMatrix();
}

void Camera::focusOn(const glm::vec2& target) {
    // Преобразуем target из мировых координат в координаты камеры
    targetPosition = glm::vec3(target.x, target.y, position.z);
    isMovingToTarget = true;
}

void Camera::update(float deltaTime) {
    if (isMovingToTarget) {
        // Плавная интерполяция к целевой позиции
        glm::vec3 direction = targetPosition - position;
        float distance = glm::length(direction);
        
        if (distance > 0.01f) {  // Продолжаем движение только если не достигли цели
            position += direction * smoothSpeed * deltaTime;
        } else {
            isMovingToTarget = false;  // Достигли цели
        }
        
        updateViewMatrix();
    }
}

void Camera::moveRight(float deltaTime) {
    isMovingToTarget = false;
    position.x += moveSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveLeft(float deltaTime) {
    isMovingToTarget = false;
    position.x -= moveSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveUp(float deltaTime) {
    isMovingToTarget = false;
    position.y -= moveSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveDown(float deltaTime) {
    isMovingToTarget = false;
    position.y += moveSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::zoom(float offset) {
    zoomLevel = glm::clamp(zoomLevel + offset * zoomSpeed, 1.0f, 40.0f);
    updateViewMatrix();
}

glm::mat4 Camera::getProjectionMatrix() const {
    float baseSize = 20.0f;
    float zoom = baseSize / std::max(zoomLevel, 0.1f);
    float zRange = std::max(baseSize * 5.0f, zoomLevel * baseSize);
    
    return glm::ortho(
        -zoom * aspect, zoom * aspect,
        -zoom, zoom,
        -zRange, zRange
    );
}

void Camera::updateViewMatrix() {
    glm::mat4 view = glm::mat4(1.0f);
    
    // Сначала применяем зум (отдаление/приближение)
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -30.0f * (zoomLevel / 20.0f)));
    
    // Затем позиционируем камеру
    view = glm::translate(view, position);
    
    // Применяем повороты
    view = glm::rotate(view, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    
    //std::cout << glm::to_string(view) << std::endl;
    viewMatrix = view;
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getScreenToWorldMatrix(const glm::vec2& windowSize) const {
    glm::mat4 projection = getProjectionMatrix();
    glm::mat4 invProjection = glm::inverse(projection);
    glm::mat4 invView = glm::inverse(viewMatrix);
    return invView * invProjection;
}

glm::vec3 Camera::screenToWorld(const glm::vec2& screenPos, const glm::vec2& windowSize) const {
    // Получаем матрицы преобразования
    glm::mat4 projection = getProjectionMatrix();
    glm::mat4 view = viewMatrix;
    glm::mat4 invVP = glm::inverse(projection * view);

    // Преобразуем экранные координаты в нормализованные координаты устройства (NDC)
    glm::vec4 screenPosNDC(
        (2.0f * screenPos.x) / windowSize.x - 1.0f,
        1.0f - (2.0f * screenPos.y) / windowSize.y,
        1.0f,  // Z = 1.0 для дальней плоскости отсечения
        1.0f
    );

    // Преобразуем NDC в мировые координаты
    glm::vec4 worldPosH = invVP * screenPosNDC;
    worldPosH /= worldPosH.w;

    // Вычисляем луч из камеры
    glm::vec3 rayOrigin = position;
    glm::vec3 rayDir = glm::normalize(glm::vec3(worldPosH) - rayOrigin);

    // Находим пересечение с плоскостью Y=0
    float t = -rayOrigin.z / rayDir.z;
    glm::vec3 intersection = rayOrigin + rayDir * t;

    return intersection;
}

glm::vec4 Camera::unproject(const glm::vec3& screenPos, const glm::vec2& windowSize) const {
    // Получаем обратные матрицы проекции и вида
    glm::mat4 projection = getProjectionMatrix();
    glm::mat4 invProjection = glm::inverse(projection);
    glm::mat4 invView = glm::inverse(viewMatrix);
    
    // Преобразуем точку обратно в мировое пространство
    glm::vec4 viewPos = invProjection * glm::vec4(screenPos, 1.0);
    return invView * viewPos;
}