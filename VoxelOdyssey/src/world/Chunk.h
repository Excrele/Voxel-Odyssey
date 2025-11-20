#pragma once
#include "Block.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <functional>

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 64;

// Forward declaration
class World;

class Chunk {
public:
    Chunk(glm::ivec3 position);
    
    Block& getBlock(int x, int y, int z);
    const Block& getBlock(int x, int y, int z) const;
    BlockType getBlockType(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);
    
    // Generate mesh with optional world query function for cross-chunk block queries
    void generateMesh(std::function<BlockType(int, int, int)> worldBlockQuery = nullptr);
    void render(Shader& shader, unsigned int texture) const;
    
    glm::ivec3 getPosition() const { return m_position; }
    bool needsMeshUpdate() const { return m_needsMeshUpdate; }
    void markDirty() { m_needsMeshUpdate = true; }
    
    bool isEmpty() const { return m_mesh.isEmpty(); }
    
    // Get chunk bounding box in world coordinates
    void getBoundingBox(glm::vec3& min, glm::vec3& max) const;
    
    // Serialization for save/load
    void serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
    
private:
    glm::ivec3 m_position;
    std::array<std::array<std::array<Block, CHUNK_SIZE>, CHUNK_HEIGHT>, CHUNK_SIZE> m_blocks;
    Mesh m_mesh;
    bool m_needsMeshUpdate;
    
    BlockType getNeighborBlockType(int x, int y, int z, 
                                   std::function<BlockType(int, int, int)> worldBlockQuery) const;
    float calculateAO(int x, int y, int z, int dx, int dy, int dz,
                      std::function<BlockType(int, int, int)> worldBlockQuery) const;
    void addFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                 glm::vec3 pos, int face, BlockType type,
                 std::function<BlockType(int, int, int)> worldBlockQuery);
};

