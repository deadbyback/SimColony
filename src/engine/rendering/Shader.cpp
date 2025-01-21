#include "Shader.hpp"
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode) {
    compileAndLink(vertexCode, fragmentCode);
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

void Shader::use() const {
    glUseProgram(programId);
}

void Shader::compileAndLink(const std::string& vertexCode, const std::string& fragmentCode) {
    // Компиляция вершинного шейдера
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vCode = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vCode, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // Компиляция фрагментного шейдера
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fCode = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fCode, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    // Создание и линковка программы
    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);
    checkCompileErrors(programId, "PROGRAM");

    // Очистка
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error("Shader compilation error of type: " + type + "\n" + infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error("Shader linking error of type: " + type + "\n" + infoLog);
        }
    }
}

GLint Shader::getUniformLocation(const std::string& name) {
    // Проверяем кэш
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        return uniformLocationCache[name];
    }
    
    // Если не найдено в кэше, запрашиваем у OpenGL
    GLint location = glGetUniformLocation(programId, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

// Методы установки uniform-переменных
void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}