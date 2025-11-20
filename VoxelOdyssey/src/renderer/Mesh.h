#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstddef>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    float ao; // Ambient occlusion value (0.0 = darkest, 1.0 = brightest)
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();
    
    void draw() const;
    void updateMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    
    bool isEmpty() const { return m_vertexCount == 0; }
    
private:
    unsigned int m_VAO, m_VBO, m_EBO;
    unsigned int m_vertexCount;
    unsigned int m_indexCount;
    
    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};

