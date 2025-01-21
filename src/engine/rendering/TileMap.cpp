#include "TileMap.hpp"
#include <algorithm>

const float TileMap::TILE_WIDTH = 1.0f;
const float TileMap::TILE_HEIGHT = 1.0f;
const float TileMap::TILE_GAP = 0.0f;

TileMap::TileMap(int width, int height) : size(width, height) {
    tiles.resize(width * height);
    renderTiles.resize(width * height);
}

void TileMap::setTile(int x, int y, const TileData& data) {
    if (!isValidPosition(x, y)) {
        return;
    }

    int index = getIndex(x, y);
    tiles[index] = data;
    
    if (!renderTiles[index]) {
        renderTiles[index] = std::make_shared<RenderableTile>(TILE_WIDTH, TILE_HEIGHT);
    }
    
    renderTiles[index]->setTexture(data.texture);
    renderTiles[index]->setPosition(gridToWorld(GridPosition(x, y)));
}

TileData* TileMap::getTileData(int x, int y) {
    if (!isValidPosition(x, y)) {
        return nullptr;
    }
    return &tiles[getIndex(x, y)];
}

void TileMap::draw(const std::shared_ptr<Shader>& shader) {
    for (int y = 0; y < size.y; ++y) {
        for (int x = 0; x < size.x; ++x) {
            int index = getIndex(x, y);
            if (renderTiles[index]) {
                renderTiles[index]->draw(shader);
            }
        }
    }
}

glm::vec2 TileMap::gridToWorld(const GridPosition& pos) const {
    return glm::vec2(
        pos.x * (TILE_WIDTH + TILE_GAP),
        pos.y * (TILE_HEIGHT + TILE_GAP)
    );
}

GridPosition TileMap::worldToGrid(const glm::vec2& worldPos) const {
    // Размер тайла с учетом промежутка
    float tileSize = TILE_WIDTH + TILE_GAP;
    
    // Смещаем координаты на половину размера тайла для центрирования
    float offsetX = worldPos.x + (tileSize / 2.0f);
    float offsetY = worldPos.y + (tileSize / 2.0f);
    
    // Преобразуем в координаты тайла
    int gridX = static_cast<int>(std::floor(offsetX / tileSize));
    int gridY = static_cast<int>(std::floor(offsetY / tileSize));

    return GridPosition(gridX, gridY);
}

void TileMap::updateHoveredTile(const GridPosition& pos) {
    // Убираем подсветку с предыдущего тайла
    if (isValidPosition(hoveredTile.x, hoveredTile.y)) {
        int prevIndex = getIndex(hoveredTile.x, hoveredTile.y);
        if (renderTiles[prevIndex]) {
            renderTiles[prevIndex]->setHighlighted(false);
        }
    }

    // Устанавливаем подсветку на новый тайл
    hoveredTile = pos;
    if (isValidPosition(pos.x, pos.y)) {
        int newIndex = getIndex(pos.x, pos.y);
        if (renderTiles[newIndex]) {
            renderTiles[newIndex]->setHighlighted(true);
        }
    }
}

bool TileMap::isValidPosition(int x, int y) const {
    return x >= 0 && x < size.x && y >= 0 && y < size.y;
}

int TileMap::getIndex(int x, int y) const {
    return y * size.x + x;
}