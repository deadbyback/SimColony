#pragma once

namespace game {
    struct TileProperties {
        bool walkable = true;      // Можно ли ходить
        bool buildable = true;     // Можно ли строить
        float elevation = 0.0f;    // Высота (от -1.0 до 1.0)
        float fertility = 0.0f;    // Плодородность (0.0 - 1.0)
        float temperature = 20.0f; // Температура в градусах
        float humidity = 0.5f;     // Влажность (0.0 - 1.0)
        
        // Конструктор по умолчанию
        TileProperties() = default;
        
        // Конструктор с параметрами
        TileProperties(bool w, bool b, float e, float f, float t, float h)
            : walkable(w), buildable(b), elevation(e)
            , fertility(f), temperature(t), humidity(h) {}
        
        // Явный оператор копирования
        TileProperties& operator=(const TileProperties& other) = default;
    };
}