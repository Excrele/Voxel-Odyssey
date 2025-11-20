#include "Chunk.h"
#include "renderer/Shader.h"
#include "Block.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstdint>

Chunk::Chunk(glm::ivec3 position) 
    : m_position(position), m_needsMeshUpdate(true) {
    // Initialize all blocks to air
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                m_blocks[x][y][z].type = BlockType::AIR;
            }
        }
    }
}

Block& Chunk::getBlock(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        static Block airBlock;
        airBlock.type = BlockType::AIR;
        return airBlock;
    }
    return m_blocks[x][y][z];
}

const Block& Chunk::getBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        static const Block airBlock = {BlockType::AIR};
        return airBlock;
    }
    return m_blocks[x][y][z];
}

BlockType Chunk::getBlockType(int x, int y, int z) const {
    return getBlock(x, y, z).type;
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        return;
    }
    m_blocks[x][y][z].type = type;
    m_needsMeshUpdate = true;
}

BlockType Chunk::getNeighborBlockType(int x, int y, int z, 
                                      std::function<BlockType(int, int, int)> worldBlockQuery) const {
    // Check if neighbor is within this chunk
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_SIZE) {
        return getBlockType(x, y, z);
    }
    
    // Neighbor is in adjacent chunk - use world query if available
    if (worldBlockQuery) {
        int worldX = m_position.x * CHUNK_SIZE + x;
        int worldY = m_position.y * CHUNK_HEIGHT + y;
        int worldZ = m_position.z * CHUNK_SIZE + z;
        return worldBlockQuery(worldX, worldY, worldZ);
    }
    
    // Default to AIR if no world query available (for backwards compatibility)
    return BlockType::AIR;
}

float Chunk::calculateAO(int x, int y, int z, int dx, int dy, int dz,
                         std::function<BlockType(int, int, int)> worldBlockQuery) const {
    // Calculate AO for a corner by checking the 3 adjacent blocks
    // dx, dy, dz indicate which corner (e.g., 1,1,1 for top-front-right corner)
    
    int solidCount = 0;
    
    // Check the 3 blocks that share this corner
    // Block in X direction
    BlockType blockX = getNeighborBlockType(x + dx, y, z, worldBlockQuery);
    if (blockX != BlockType::AIR) solidCount++;
    
    // Block in Y direction
    BlockType blockY = getNeighborBlockType(x, y + dy, z, worldBlockQuery);
    if (blockY != BlockType::AIR) solidCount++;
    
    // Block in Z direction
    BlockType blockZ = getNeighborBlockType(x, y, z + dz, worldBlockQuery);
    if (blockZ != BlockType::AIR) solidCount++;
    
    // Also check the diagonal block (corner block)
    BlockType blockDiag = getNeighborBlockType(x + dx, y + dy, z + dz, worldBlockQuery);
    if (blockDiag != BlockType::AIR) {
        // If diagonal is solid, it contributes more darkness
        solidCount += 2;
    }
    
    // Convert solid count to AO value (0.0 = darkest, 1.0 = brightest)
    // 0 solids: 1.0, 1 solid: 0.8, 2 solids: 0.6, 3+ solids: 0.4
    if (solidCount == 0) return 1.0f;
    if (solidCount == 1) return 0.8f;
    if (solidCount == 2) return 0.6f;
    if (solidCount == 3) return 0.5f;
    return 0.4f; // 4+ solids
}

void Chunk::addFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                    glm::vec3 pos, int face, BlockType type,
                    std::function<BlockType(int, int, int)> worldBlockQuery) {
    // Face definitions: 0=front, 1=back, 2=top, 3=bottom, 4=right, 5=left
    glm::vec3 normal;
    glm::vec3 v0, v1, v2, v3;
    glm::vec2 uv0, uv1, uv2, uv3;
    
    float size = 0.5f;
    glm::vec2 texCoord = getTexCoord(type, face);
    float texSize = 0.25f; // Size of one texture in atlas
    
    switch (face) {
        case 0: // Front (Z+)
            normal = glm::vec3(0, 0, 1);
            v0 = pos + glm::vec3(-size, -size, size);
            v1 = pos + glm::vec3(size, -size, size);
            v2 = pos + glm::vec3(size, size, size);
            v3 = pos + glm::vec3(-size, size, size);
            uv0 = glm::vec2(texCoord.x, texCoord.y);
            uv1 = glm::vec2(texCoord.x + texSize, texCoord.y);
            uv2 = glm::vec2(texCoord.x + texSize, texCoord.y + texSize);
            uv3 = glm::vec2(texCoord.x, texCoord.y + texSize);
            break;
        case 1: // Back (Z-)
            normal = glm::vec3(0, 0, -1);
            v0 = pos + glm::vec3(size, -size, -size);
            v1 = pos + glm::vec3(-size, -size, -size);
            v2 = pos + glm::vec3(-size, size, -size);
            v3 = pos + glm::vec3(size, size, -size);
            uv0 = glm::vec2(texCoord.x + texSize, texCoord.y);
            uv1 = glm::vec2(texCoord.x, texCoord.y);
            uv2 = glm::vec2(texCoord.x, texCoord.y + texSize);
            uv3 = glm::vec2(texCoord.x + texSize, texCoord.y + texSize);
            break;
        case 2: // Top (Y+)
            normal = glm::vec3(0, 1, 0);
            v0 = pos + glm::vec3(-size, size, -size);
            v1 = pos + glm::vec3(-size, size, size);
            v2 = pos + glm::vec3(size, size, size);
            v3 = pos + glm::vec3(size, size, -size);
            uv0 = glm::vec2(texCoord.x, texCoord.y + texSize);
            uv1 = glm::vec2(texCoord.x, texCoord.y);
            uv2 = glm::vec2(texCoord.x + texSize, texCoord.y);
            uv3 = glm::vec2(texCoord.x + texSize, texCoord.y + texSize);
            break;
        case 3: // Bottom (Y-)
            normal = glm::vec3(0, -1, 0);
            v0 = pos + glm::vec3(-size, -size, size);
            v1 = pos + glm::vec3(-size, -size, -size);
            v2 = pos + glm::vec3(size, -size, -size);
            v3 = pos + glm::vec3(size, -size, size);
            uv0 = glm::vec2(texCoord.x, texCoord.y);
            uv1 = glm::vec2(texCoord.x, texCoord.y + texSize);
            uv2 = glm::vec2(texCoord.x + texSize, texCoord.y + texSize);
            uv3 = glm::vec2(texCoord.x + texSize, texCoord.y);
            break;
        case 4: // Right (X+)
            normal = glm::vec3(1, 0, 0);
            v0 = pos + glm::vec3(size, -size, size);
            v1 = pos + glm::vec3(size, -size, -size);
            v2 = pos + glm::vec3(size, size, -size);
            v3 = pos + glm::vec3(size, size, size);
            uv0 = glm::vec2(texCoord.x + texSize, texCoord.y);
            uv1 = glm::vec2(texCoord.x, texCoord.y);
            uv2 = glm::vec2(texCoord.x, texCoord.y + texSize);
            uv3 = glm::vec2(texCoord.x + texSize, texCoord.y + texSize);
            break;
        case 5: // Left (X-)
            normal = glm::vec3(-1, 0, 0);
            v0 = pos + glm::vec3(-size, -size, -size);
            v1 = pos + glm::vec3(-size, -size, size);
            v2 = pos + glm::vec3(-size, size, size);
            v3 = pos + glm::vec3(-size, size, -size);
            uv0 = glm::vec2(texCoord.x, texCoord.y);
            uv1 = glm::vec2(texCoord.x + texSize, texCoord.y);
            uv2 = glm::vec2(texCoord.x + texSize, texCoord.y + texSize);
            uv3 = glm::vec2(texCoord.x, texCoord.y + texSize);
            break;
    }
    
    // Calculate AO for each corner based on face direction
    int blockX = static_cast<int>(pos.x);
    int blockY = static_cast<int>(pos.y);
    int blockZ = static_cast<int>(pos.z);
    
    float ao0 = 1.0f, ao1 = 1.0f, ao2 = 1.0f, ao3 = 1.0f;
    
    // Calculate AO for each vertex based on face and corner position
    switch (face) {
        case 0: // Front (Z+)
            ao0 = calculateAO(blockX, blockY, blockZ, -1, -1, 1, worldBlockQuery); // Bottom-left
            ao1 = calculateAO(blockX, blockY, blockZ, 1, -1, 1, worldBlockQuery);  // Bottom-right
            ao2 = calculateAO(blockX, blockY, blockZ, 1, 1, 1, worldBlockQuery);    // Top-right
            ao3 = calculateAO(blockX, blockY, blockZ, -1, 1, 1, worldBlockQuery);    // Top-left
            break;
        case 1: // Back (Z-)
            ao0 = calculateAO(blockX, blockY, blockZ, 1, -1, -1, worldBlockQuery);
            ao1 = calculateAO(blockX, blockY, blockZ, -1, -1, -1, worldBlockQuery);
            ao2 = calculateAO(blockX, blockY, blockZ, -1, 1, -1, worldBlockQuery);
            ao3 = calculateAO(blockX, blockY, blockZ, 1, 1, -1, worldBlockQuery);
            break;
        case 2: // Top (Y+)
            ao0 = calculateAO(blockX, blockY, blockZ, -1, 1, -1, worldBlockQuery);
            ao1 = calculateAO(blockX, blockY, blockZ, -1, 1, 1, worldBlockQuery);
            ao2 = calculateAO(blockX, blockY, blockZ, 1, 1, 1, worldBlockQuery);
            ao3 = calculateAO(blockX, blockY, blockZ, 1, 1, -1, worldBlockQuery);
            break;
        case 3: // Bottom (Y-)
            ao0 = calculateAO(blockX, blockY, blockZ, -1, -1, 1, worldBlockQuery);
            ao1 = calculateAO(blockX, blockY, blockZ, -1, -1, -1, worldBlockQuery);
            ao2 = calculateAO(blockX, blockY, blockZ, 1, -1, -1, worldBlockQuery);
            ao3 = calculateAO(blockX, blockY, blockZ, 1, -1, 1, worldBlockQuery);
            break;
        case 4: // Right (X+)
            ao0 = calculateAO(blockX, blockY, blockZ, 1, -1, 1, worldBlockQuery);
            ao1 = calculateAO(blockX, blockY, blockZ, 1, -1, -1, worldBlockQuery);
            ao2 = calculateAO(blockX, blockY, blockZ, 1, 1, -1, worldBlockQuery);
            ao3 = calculateAO(blockX, blockY, blockZ, 1, 1, 1, worldBlockQuery);
            break;
        case 5: // Left (X-)
            ao0 = calculateAO(blockX, blockY, blockZ, -1, -1, -1, worldBlockQuery);
            ao1 = calculateAO(blockX, blockY, blockZ, -1, -1, 1, worldBlockQuery);
            ao2 = calculateAO(blockX, blockY, blockZ, -1, 1, 1, worldBlockQuery);
            ao3 = calculateAO(blockX, blockY, blockZ, -1, 1, -1, worldBlockQuery);
            break;
    }
    
    unsigned int baseIndex = vertices.size();
    
    vertices.push_back({v0, normal, uv0, ao0});
    vertices.push_back({v1, normal, uv1, ao1});
    vertices.push_back({v2, normal, uv2, ao2});
    vertices.push_back({v3, normal, uv3, ao3});
    
    // Two triangles per face
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

void Chunk::generateMesh(std::function<BlockType(int, int, int)> worldBlockQuery) {
    if (!m_needsMeshUpdate) {
        return;
    }
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                BlockType type = getBlockType(x, y, z);
                if (type == BlockType::AIR) {
                    continue;
                }
                
                glm::vec3 blockPos(x, y, z);
                
                // Check each face and add if visible (using cross-chunk queries)
                if (getNeighborBlockType(x, y, z + 1, worldBlockQuery) == BlockType::AIR) {
                    addFace(vertices, indices, blockPos, 0, type, worldBlockQuery); // Front
                }
                if (getNeighborBlockType(x, y, z - 1, worldBlockQuery) == BlockType::AIR) {
                    addFace(vertices, indices, blockPos, 1, type, worldBlockQuery); // Back
                }
                if (getNeighborBlockType(x, y + 1, z, worldBlockQuery) == BlockType::AIR) {
                    addFace(vertices, indices, blockPos, 2, type, worldBlockQuery); // Top
                }
                if (getNeighborBlockType(x, y - 1, z, worldBlockQuery) == BlockType::AIR) {
                    addFace(vertices, indices, blockPos, 3, type, worldBlockQuery); // Bottom
                }
                if (getNeighborBlockType(x + 1, y, z, worldBlockQuery) == BlockType::AIR) {
                    addFace(vertices, indices, blockPos, 4, type, worldBlockQuery); // Right
                }
                if (getNeighborBlockType(x - 1, y, z, worldBlockQuery) == BlockType::AIR) {
                    addFace(vertices, indices, blockPos, 5, type, worldBlockQuery); // Left
                }
            }
        }
    }
    
    m_mesh.updateMesh(vertices, indices);
    m_needsMeshUpdate = false;
}

void Chunk::serialize(std::vector<uint8_t>& data) const {
    // Write chunk position (12 bytes: 3 ints)
    int pos[3] = {m_position.x, m_position.y, m_position.z};
    data.insert(data.end(), reinterpret_cast<const uint8_t*>(pos), 
                reinterpret_cast<const uint8_t*>(pos) + sizeof(pos));
    
    // Write all blocks (CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE bytes)
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t blockType = static_cast<uint8_t>(m_blocks[x][y][z].type);
                data.push_back(blockType);
            }
        }
    }
}

bool Chunk::deserialize(const std::vector<uint8_t>& data) {
    // Minimum size: 12 bytes (position) + CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE bytes (blocks)
    size_t expectedSize = 12 + CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;
    if (data.size() < expectedSize) {
        return false;
    }
    
    // Read chunk position
    const int* pos = reinterpret_cast<const int*>(data.data());
    m_position = glm::ivec3(pos[0], pos[1], pos[2]);
    
    // Read all blocks
    size_t offset = 12;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (offset >= data.size()) {
                    return false;
                }
                m_blocks[x][y][z].type = static_cast<BlockType>(data[offset++]);
            }
        }
    }
    
    // Mark as dirty so mesh gets regenerated
    m_needsMeshUpdate = true;
    return true;
}

void Chunk::getBoundingBox(glm::vec3& min, glm::vec3& max) const {
    min.x = m_position.x * CHUNK_SIZE;
    min.y = m_position.y * CHUNK_HEIGHT;
    min.z = m_position.z * CHUNK_SIZE;
    
    max.x = min.x + CHUNK_SIZE;
    max.y = min.y + CHUNK_HEIGHT;
    max.z = min.z + CHUNK_SIZE;
}

void Chunk::render(Shader& shader, unsigned int texture) const {
    if (m_mesh.isEmpty()) {
        return;
    }
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_position.x * CHUNK_SIZE, 
                                            m_position.y * CHUNK_HEIGHT, 
                                            m_position.z * CHUNK_SIZE));
    shader.setMat4("model", model);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("texture1", 0);
    
    m_mesh.draw();
}

