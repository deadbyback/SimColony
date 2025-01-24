#include "ShaderLoader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace engine {

std::shared_ptr<engine::Shader> ShaderLoader::loadFromFiles(
    const std::string& vertexPath, 
    const std::string& fragmentPath
) {
    // Читаем исходный код шейдеров из файлов
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    // Создаём новый шейдер
    return std::make_shared<engine::Shader>(vertexCode, fragmentCode);
}

std::string ShaderLoader::readFile(const std::string& filePath) {
    std::ifstream file;
    // Включаем исключения для операций с файлами
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        file.open(filePath);
        std::stringstream stream;
        // Читаем файл в строковый поток
        stream << file.rdbuf();

        file.close();
        return stream.str();
    }
    catch (const std::ifstream::failure& e) {
        throw std::runtime_error(
            "Failed to read shader file: " + filePath + 
            "\nError: " + std::string(e.what())
        );
    }
}

} // namespace engine