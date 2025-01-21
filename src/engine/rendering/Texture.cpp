#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& path, bool flip) : id(0), width(0), height(0), channels(0) {
    // Устанавливаем настройку переворота изображения
    stbi_set_flip_vertically_on_load(flip);
    
    // Загружаем изображение
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }

    // Определяем формат текстуры на основе количества каналов
    GLenum format;
    switch (channels) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: format = GL_RGB; break;
    }

    // Создаем текстуру в OpenGL
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Устанавливаем параметры фильтрации и повторения текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Загружаем данные текстуры в OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Освобождаем память изображения
    stbi_image_free(data);

    // Проверяем на ошибки OpenGL
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error while loading texture: " << error << std::endl;
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}