#pragma once
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    // Конструктор принимает код шейдеров в виде строк
    Shader(const std::string& vertexCode, const std::string& fragmentCode);
    ~Shader();

    // Активация шейдерной программы
    void use() const;

    // Утилиты для установки uniform-переменных
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& value);

private:
    // ID шейдерной программы в OpenGL
    GLuint programId;
    
    // Кэш расположений uniform-переменных
    std::unordered_map<std::string, GLint> uniformLocationCache;

    // Вспомогательные методы
    void compileAndLink(const std::string& vertexCode, const std::string& fragmentCode);
    void checkCompileErrors(GLuint shader, const std::string& type);
    GLint getUniformLocation(const std::string& name);
};