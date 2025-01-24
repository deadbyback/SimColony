#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "RenderableTile.hpp"
#include "../../game/Tile.hpp"

namespace engine {

class TileMap {
public:
    // Конструктор принимает размеры карты
    TileMap(int width, int height);

    // Методы для работы с тайлами
    void setTile(int x, int y, const TileData& data);
    TileData* getTileData(int x, int y);
    // Отрисовка всей карты
    void draw(const std::shared_ptr<engine::Shader>& shader);
    // Обновление подсвеченного тайла
    void updateHoveredTile(const GridPosition& pos);
    // Проверка, является ли тайл подсвеченным
    bool isHoveredTile(int x, int y) const { return hoveredTile.x == x && hoveredTile.y == y; }

    // Геометрические преобразования
    glm::vec2 gridToWorld(const GridPosition& pos) const;  // Из координат сетки в мировые
    GridPosition worldToGrid(const glm::vec2& worldPos) const;  // Из мировых в координаты сетки

    // Получение размеров карты
    glm::ivec2 getSize() const { return size; }

    bool isValidPosition(int x, int y) const;
    int getIndex(int x, int y) const;

private:
    glm::ivec2 size;  // Размеры карты в тайлах
    std::vector<TileData> tiles;  // Данные тайлов
    std::vector<std::shared_ptr<engine::RenderableTile>> renderTiles;  // Визуальное представление тайлов
    GridPosition hoveredTile{-1, -1};  // Текущий подсвеченный тайл
    
    // Вспомогательные методы
    void updateTileVisual(int x, int y);

    // Константы для настройки размеров и отступов тайлов
    static const float TILE_WIDTH;
    static const float TILE_HEIGHT;
    static const float TILE_GAP; 
    static const float TILE_OFFSET_X_MULTIPLIER;
    static const float TILE_OFFSET_Y_MULTIPLIER;
    static const float TILE_OFFSET_X;  // Смещение для изометрического вида
    static const float TILE_OFFSET_Y;
};

} // namespace engine