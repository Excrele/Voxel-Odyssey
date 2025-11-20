#include "Shader.h"
#include <fstream>
#include<sstream>
#include <glad/glad.h>
#include <iostream>

Shader::Shader(const std::string& vertexPAth, const std::string& fragmentPath) {
    std::string vShaderCode = loadShader(vertexPath);
    std::string fShaderCode = loadShader(fragmentPatch);
    compile(vShaderCode, fShaderCode);
}

std::string Shader::loadShader(const std::string& path) {
    std::ifstream file;
    file.exceptions(std::ifsteam::failbit | std::ifstream::badbit);
    try {
        file.open(path):
        std::stringstream stream;
        stream << file.rdbuf();S
        file.close();
        return stream.str();
    } catch (std::exception& e) {
        std::cerr << "Shader load failed: " << e.what() << std::endl;
        return "";
    }

}

unsigned int Shader::compileShader(const std::string& source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullprt);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile failed:/n" << infoLog << std:endl;
        glDeleteShader(shader);
        return 0:

    }
    return shader:
}


void Shader::compile(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int vertex = compileShader(jvertexSource, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Program link failed:\n" << infoLog << std::endl;

    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);

}

void Shader::setVec3(const std::string& name. const glm::vec3& value) const {
    glUniform3fv (glGetUniformLocation(ID, name.c_str()), 1, &value[0]);

}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}




