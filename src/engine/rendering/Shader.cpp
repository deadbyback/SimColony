#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace engine {

Shader::Shader(const std::string& name) {
    // Загружаем шейдеры из файлов
    std::string vertPath = "../../src/engine/rendering/shaders/" + name + ".vert";
    std::string fragPath = "../../src/engine/rendering/shaders/" + name + ".frag";

    std::cout << "Loading vertex shader: " << vertPath << std::endl;
    std::cout << "Loading fragment shader: " << fragPath << std::endl;

    // Читаем вершинный шейдер
    std::ifstream vertFile(vertPath);
    if (!vertFile.is_open()) {
        std::cerr << "Failed to open vertex shader: " << vertPath << std::endl;
        throw std::runtime_error("Failed to open vertex shader file");
    }
    std::stringstream vertStream;
    vertStream << vertFile.rdbuf();
    std::string vertCode = vertStream.str();

    // Читаем фрагментный шейдер
    std::ifstream fragFile(fragPath);
    if (!fragFile.is_open()) {
        std::cerr << "Failed to open fragment shader: " << fragPath << std::endl;
        throw std::runtime_error("Failed to open fragment shader file");
    }
    std::stringstream fragStream;
    fragStream << fragFile.rdbuf();
    std::string fragCode = fragStream.str();

    // Компилируем шейдеры
    compileAndLink(vertCode.c_str(), fragCode.c_str());
}

Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode) {
    compileAndLink(vertexCode.c_str(), fragmentCode.c_str());
}

void Shader::compileAndLink(const char* vertexCode, const char* fragmentCode) {
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        std::cerr << "Vertex Shader Code:\n" << vertexCode << std::endl;
    }

    // Фрагментный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        std::cerr << "Fragment Shader Code:\n" << fragmentCode << std::endl;
    }

    // Шейдерная программа
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Удаляем шейдеры, они уже связаны с программой
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

} // namespace engine