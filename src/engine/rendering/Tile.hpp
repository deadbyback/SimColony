#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "Texture.hpp"

class Tile {
public:
    Tile(float width, float height);
    ~Tile();

    void setPosition(const glm::vec2& pos);
    void setTexture(const std::shared_ptr<Texture>& texture);
    void draw(const std::shared_ptr<Shader>& shader);

private:
    unsigned int VAO, VBO, EBO;
    glm::vec2 position;
    std::shared_ptr<Texture> texture;
    float width, height;

    void initializeBuffers();
};
