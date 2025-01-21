#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    // Загружает текстуру из файла. Второй параметр определяет, нужно ли переворачивать изображение
    Texture(const std::string& path, bool flip = true);
    ~Texture();

    // Привязывает текстуру к указанному текстурному слоту
    void bind(unsigned int slot = 0) const;
    
    // Получаем размеры текстуры
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    unsigned int id;        // OpenGL ID текстуры
    int width;             // Ширина изображения
    int height;            // Высота изображения
    int channels;          // Количество цветовых каналов
};