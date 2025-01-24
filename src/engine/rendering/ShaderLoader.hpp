#pragma once

#include <string>
#include <memory>
#include "Shader.hpp"

namespace engine {

class ShaderLoader {
public:
    // Загружает шейдер из файлов, возвращает умный указатель
    static std::shared_ptr<engine::Shader> loadFromFiles(
        const std::string& vertexPath, 
        const std::string& fragmentPath
    );
    
private:
    // Вспомогательный метод для чтения содержимого файла
    static std::string readFile(const std::string& filePath);
};

} // namespace engine