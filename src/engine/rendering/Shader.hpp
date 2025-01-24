#pragma once

#include <string>
#include <glm/glm.hpp>

namespace engine {

class Shader {
public:
    // Конструктор для загрузки из файлов
    Shader(const std::string& name);
    
    // Конструктор для создания из кода
    Shader(const std::string& vertexCode, const std::string& fragmentCode);
    
    ~Shader();

    void use();
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat2(const std::string& name, const glm::mat2& mat);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);

private:
    unsigned int ID;
    void compileAndLink(const char* vertexCode, const char* fragmentCode);
};

} // namespace engine