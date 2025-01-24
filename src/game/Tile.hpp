#pragma once
#include <memory>
#include "../engine/rendering/Texture.hpp"

// Перечисление для типов тайлов
enum class TileType {
    NONE,           // Пустой/недоступный тайл
    GROUND,         // Обычная земля
    WATER,          // Вода
    MOUNTAIN,       // Гора/камень
    // Добавим другие типы позже
};

// Структура для хранения данных о тайле
struct TileData {
    TileType type;                                  // Тип тайла
    std::shared_ptr<engine::Texture> texture;       // Текстура тайла
    bool walkable;                                  // Можно ли ходить по тайлу
    float elevation;                                // Высота тайла (для будущего использования)
    
    // Конструктор по умолчанию
    TileData() 
        : type(TileType::NONE)
        , walkable(false)
        , elevation(0.0f) {
    }

    // Конструктор с параметрами
    TileData(TileType type, bool walkable, float elevation = 0.0f)
        : type(type)
        , walkable(walkable)
        , elevation(elevation) {
    }
};

// Структура для координат тайла в сетке
struct GridPosition {
    int x;
    int y;

    GridPosition(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const GridPosition& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const GridPosition& other) const {
        return !(*this == other);
    }
};

// Вспомогательная функция для хеширования GridPosition (если понадобится для контейнеров)
namespace std {
    template<>
    struct hash<GridPosition> {
        size_t operator()(const GridPosition& pos) const {
            return hash<int>()(pos.x) ^ (hash<int>()(pos.y) << 1);
        }
    };
}