#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>


class Shader {
    public: 
        unsigned int ID;
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        void use();
        void setMat4(const std::string& name, const glm::mat4& mat) const;
        void setVec3(const std::string& name, const glm:: vec3& value) const;
        void setInt(const std::string& name, int value) const;

    private:
        void compile(const std::string& vertexSource, const std::string & fragmentSource);
        std::string loadShader(const std::string& path);
        unsigned int compileShader(const std::string& source, unsigned int type);
        std::unordered_map<std::string, int > uniformCache;

};