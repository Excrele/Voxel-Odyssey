#pragma once
#include "Chunk.h"
#include "renderer/Shader.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>

enum class Biome {
    GRASSLAND,
    DESERT,
    SNOW,
    FOREST,
    OCEAN
};

struct ChunkKey {
    int x, y, z;
    
    bool operator==(const ChunkKey& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

namespace std {
    template<>
    struct hash<ChunkKey> {
        size_t operator()(const ChunkKey& key) const {
            return ((hash<int>()(key.x) ^ (hash<int>()(key.y) << 1)) >> 1) ^ (hash<int>()(key.z) << 1);
        }
    };
}

class World {
public:
    World();
    ~World();
    
    void update(const glm::vec3& playerPos);
    void render(Shader& shader, unsigned int texture);
    void render(Shader& shader, unsigned int texture, const class Frustum& frustum);
    
    Chunk* getChunk(int chunkX, int chunkY, int chunkZ);
    BlockType getBlock(int worldX, int worldY, int worldZ);
    void setBlock(int worldX, int worldY, int worldZ, BlockType type);
    
    // Mark chunk as dirty when block is modified
    void markChunkDirty(int worldX, int worldY, int worldZ);
    
    // Save/Load
    void setWorldName(const std::string& worldName) { m_worldName = worldName; }
    void saveAllChunks() const;
    
private:
    std::unordered_map<ChunkKey, std::unique_ptr<Chunk>> m_chunks;
    std::string m_worldName;
    static constexpr int RENDER_DISTANCE = 4;
    
    glm::ivec3 worldToChunk(int x, int y, int z) const;
    glm::ivec3 worldToBlock(int x, int y, int z) const;
    Chunk* getOrCreateChunk(int chunkX, int chunkY, int chunkZ);
    void generateTerrain(Chunk& chunk);
    void unloadDistantChunks(const glm::vec3& playerPos);
    Biome determineBiome(int worldX, int worldZ, float height) const;
    
    // Save/Load system
    bool saveChunk(const Chunk* chunk, const std::string& worldName) const;
    bool loadChunk(Chunk& chunk, int chunkX, int chunkY, int chunkZ, const std::string& worldName) const;
    std::string getChunkFilePath(int chunkX, int chunkY, int chunkZ, const std::string& worldName) const;
};

