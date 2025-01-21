#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "Texture.hpp"

class RenderableTile {
public:
    RenderableTile(float width, float height);
    ~RenderableTile();

    void setPosition(const glm::vec2& pos);
    void setTexture(const std::shared_ptr<Texture>& texture);
    void setHighlighted(bool highlighted) { isHighlighted = highlighted; }
    void draw(const std::shared_ptr<Shader>& shader);

private:
    unsigned int VAO, VBO, EBO;
    glm::vec2 position;
    std::shared_ptr<Texture> texture;
    float width, height;
    bool isHighlighted = false;

    void initializeBuffers();
};
