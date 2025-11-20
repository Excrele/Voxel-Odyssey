#include "World.h"
#include "core/Frustum.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

World::World() : m_worldName("world1") {
}

World::~World() {
    m_chunks.clear();
}

glm::ivec3 World::worldToChunk(int x, int y, int z) const {
    int chunkX = x < 0 ? (x - CHUNK_SIZE + 1) / CHUNK_SIZE : x / CHUNK_SIZE;
    int chunkY = y < 0 ? (y - CHUNK_HEIGHT + 1) / CHUNK_HEIGHT : y / CHUNK_HEIGHT;
    int chunkZ = z < 0 ? (z - CHUNK_SIZE + 1) / CHUNK_SIZE : z / CHUNK_SIZE;
    return glm::ivec3(chunkX, chunkY, chunkZ);
}

glm::ivec3 World::worldToBlock(int x, int y, int z) const {
    int blockX = ((x % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
    int blockY = ((y % CHUNK_HEIGHT) + CHUNK_HEIGHT) % CHUNK_HEIGHT;
    int blockZ = ((z % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
    return glm::ivec3(blockX, blockY, blockZ);
}

Chunk* World::getOrCreateChunk(int chunkX, int chunkY, int chunkZ) {
    ChunkKey key{chunkX, chunkY, chunkZ};
    
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        return it->second.get();
    }
    
    // Try to load chunk from disk first
    auto chunk = std::make_unique<Chunk>(glm::ivec3(chunkX, chunkY, chunkZ));
    bool loaded = loadChunk(*chunk, chunkX, chunkY, chunkZ, m_worldName);
    
    // If not loaded, generate new terrain
    if (!loaded) {
        generateTerrain(*chunk);
    }
    
    // Generate mesh with world block query function
    auto worldQuery = [this](int x, int y, int z) { return this->getBlock(x, y, z); };
    chunk->generateMesh(worldQuery);
    
    Chunk* chunkPtr = chunk.get();
    m_chunks[key] = std::move(chunk);
    
    return chunkPtr;
}

Chunk* World::getChunk(int chunkX, int chunkY, int chunkZ) {
    ChunkKey key{chunkX, chunkY, chunkZ};
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        return it->second.get();
    }
    return nullptr;
}

BlockType World::getBlock(int worldX, int worldY, int worldZ) {
    glm::ivec3 chunkPos = worldToChunk(worldX, worldY, worldZ);
    glm::ivec3 blockPos = worldToBlock(worldX, worldY, worldZ);
    
    Chunk* chunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z);
    if (chunk) {
        return chunk->getBlockType(blockPos.x, blockPos.y, blockPos.z);
    }
    return BlockType::AIR;
}

void World::setBlock(int worldX, int worldY, int worldZ, BlockType type) {
    glm::ivec3 chunkPos = worldToChunk(worldX, worldY, worldZ);
    glm::ivec3 blockPos = worldToBlock(worldX, worldY, worldZ);
    
    Chunk* chunk = getOrCreateChunk(chunkPos.x, chunkPos.y, chunkPos.z);
    chunk->setBlock(blockPos.x, blockPos.y, blockPos.z, type);
    
    // Mark this chunk and adjacent chunks as dirty (in case block is on border)
    markChunkDirty(worldX, worldY, worldZ);
    
    // Save chunk when block is modified (optional - could be done periodically instead)
    // saveChunk(chunk, "world1");
}

void World::markChunkDirty(int worldX, int worldY, int worldZ) {
    glm::ivec3 chunkPos = worldToChunk(worldX, worldY, worldZ);
    
    // Mark the chunk containing this block
    Chunk* chunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z);
    if (chunk) {
        chunk->markDirty();
    }
    
    // Mark adjacent chunks if block is on border
    glm::ivec3 blockPos = worldToBlock(worldX, worldY, worldZ);
    
    if (blockPos.x == 0) {
        Chunk* adjChunk = getChunk(chunkPos.x - 1, chunkPos.y, chunkPos.z);
        if (adjChunk) adjChunk->markDirty();
    } else if (blockPos.x == CHUNK_SIZE - 1) {
        Chunk* adjChunk = getChunk(chunkPos.x + 1, chunkPos.y, chunkPos.z);
        if (adjChunk) adjChunk->markDirty();
    }
    
    if (blockPos.y == 0) {
        Chunk* adjChunk = getChunk(chunkPos.x, chunkPos.y - 1, chunkPos.z);
        if (adjChunk) adjChunk->markDirty();
    } else if (blockPos.y == CHUNK_HEIGHT - 1) {
        Chunk* adjChunk = getChunk(chunkPos.x, chunkPos.y + 1, chunkPos.z);
        if (adjChunk) adjChunk->markDirty();
    }
    
    if (blockPos.z == 0) {
        Chunk* adjChunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z - 1);
        if (adjChunk) adjChunk->markDirty();
    } else if (blockPos.z == CHUNK_SIZE - 1) {
        Chunk* adjChunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z + 1);
        if (adjChunk) adjChunk->markDirty();
    }
}

// Simple noise function for terrain generation
static float noise2D(float x, float z) {
    return std::sin(x * 0.1f) * std::cos(z * 0.1f) + 
           0.5f * std::sin(x * 0.2f) * std::cos(z * 0.2f) +
           0.25f * std::sin(x * 0.4f) * std::cos(z * 0.4f);
}

// Biome noise for determining biome type
static float biomeNoise(float x, float z) {
    return std::sin(x * 0.05f) * std::cos(z * 0.05f) + 
           0.5f * std::sin(x * 0.1f) * std::cos(z * 0.1f);
}

// Improved 3D noise for caves - multiple octaves for better variation
static float noise3D(float x, float y, float z) {
    return std::sin(x * 0.1f) * std::cos(y * 0.1f) * std::sin(z * 0.1f) +
           0.5f * std::sin(x * 0.2f) * std::cos(y * 0.2f) * std::sin(z * 0.2f) +
           0.25f * std::sin(x * 0.4f) * std::cos(y * 0.4f) * std::sin(z * 0.4f);
}

// Cave density function - creates more varied cave shapes
static float caveDensity(float x, float y, float z) {
    // Use multiple noise functions for varied cave systems
    float n1 = noise3D(x * 0.12f, y * 0.12f, z * 0.12f);
    float n2 = noise3D(x * 0.25f, y * 0.25f, z * 0.25f);
    float n3 = noise3D(x * 0.5f, y * 0.5f, z * 0.5f);
    
    // Combine with different weights
    float density = n1 * 0.6f + n2 * 0.3f + n3 * 0.1f;
    
    // Add vertical bias - more caves at certain depths
    float depthFactor = 1.0f - std::abs((y - 20.0f) / 30.0f); // Peak around y=20
    depthFactor = std::max(0.0f, depthFactor);
    
    return density * (0.5f + 0.5f * depthFactor);
}

// Ore generation noise
static float oreNoise(float x, float y, float z) {
    return std::sin(x * 0.3f) * std::cos(y * 0.3f) * std::sin(z * 0.3f);
}

// Generate a tree at given position
static void generateTree(Chunk& chunk, int x, int y, int z) {
    // Trunk (4-6 blocks tall)
    int treeHeight = 4 + (x + z) % 3; // Vary height
    for (int i = 0; i < treeHeight && y + i < CHUNK_HEIGHT; i++) {
        chunk.setBlock(x, y + i, z, BlockType::WOOD);
    }
    
    // Leaves (simple sphere)
    int leafY = y + treeHeight;
    for (int dx = -2; dx <= 2; dx++) {
        for (int dz = -2; dz <= 2; dz++) {
            for (int dy = 0; dy <= 2; dy++) {
                int distSq = dx*dx + dz*dz + dy*dy;
                if (distSq <= 4 && x + dx >= 0 && x + dx < CHUNK_SIZE &&
                    z + dz >= 0 && z + dz < CHUNK_SIZE &&
                    leafY + dy < CHUNK_HEIGHT) {
                    // Don't overwrite trunk
                    if (chunk.getBlockType(x + dx, leafY + dy, z + dz) != BlockType::WOOD) {
                        chunk.setBlock(x + dx, leafY + dy, z + dz, BlockType::LEAVES);
                    }
                }
            }
        }
    }
}

// Generate ore veins in stone
static void generateOre(Chunk& chunk, int x, int y, int z, int worldX, int worldY, int worldZ) {
    // Only generate ore in stone
    if (chunk.getBlockType(x, y, z) != BlockType::STONE) {
        return;
    }
    
    // Ore generation based on depth and noise
    float oreValue = oreNoise(worldX * 0.2f, worldY * 0.2f, worldZ * 0.2f);
    
    // Coal ore - common, found at all depths
    if (oreValue > 0.7f && (worldX * 13 + worldY * 17 + worldZ * 19) % 100 < 3) {
        chunk.setBlock(x, y, z, BlockType::COAL_ORE);
        return;
    }
    
    // Iron ore - less common, found deeper
    if (worldY < 30 && oreValue > 0.75f && (worldX * 13 + worldY * 17 + worldZ * 19) % 100 < 2) {
        chunk.setBlock(x, y, z, BlockType::IRON_ORE);
        return;
    }
}

// Generate stalactites (hanging from ceiling) and stalagmites (growing from floor)
static void generateStalactites(Chunk& chunk, int x, int y, int z, int worldX, int worldY, int worldZ) {
    // Only in caves (air blocks)
    if (chunk.getBlockType(x, y, z) != BlockType::AIR) {
        return;
    }
    
    // Check if there's stone above (stalactite) or below (stalagmite)
    bool stoneAbove = (y + 1 < CHUNK_HEIGHT && chunk.getBlockType(x, y + 1, z) == BlockType::STONE);
    bool stoneBelow = (y - 1 >= 0 && chunk.getBlockType(x, y - 1, z) == BlockType::STONE);
    
    // Stalactite (hanging from ceiling)
    if (stoneAbove && (worldX * 7 + worldY * 11 + worldZ * 13) % 200 < 2) {
        int length = 1 + (worldX + worldZ) % 3; // 1-3 blocks long
        for (int i = 0; i < length && y - i >= 0; i++) {
            if (chunk.getBlockType(x, y - i, z) == BlockType::AIR) {
                chunk.setBlock(x, y - i, z, BlockType::STONE);
            } else {
                break;
            }
        }
    }
    
    // Stalagmite (growing from floor)
    if (stoneBelow && (worldX * 7 + worldY * 11 + worldZ * 13) % 200 < 2) {
        int length = 1 + (worldX + worldZ) % 3; // 1-3 blocks long
        for (int i = 0; i < length && y + i < CHUNK_HEIGHT; i++) {
            if (chunk.getBlockType(x, y + i, z) == BlockType::AIR) {
                chunk.setBlock(x, y + i, z, BlockType::STONE);
            } else {
                break;
            }
        }
    }
}

Biome World::determineBiome(int worldX, int worldZ, float height) const {
    // Use biome noise to create regions
    float biomeValue = biomeNoise(worldX, worldZ);
    float distance = std::sqrt(worldX * worldX + worldZ * worldZ);
    
    // Ocean biome: low-lying areas
    if (height < 28.0f) {
        return Biome::OCEAN;
    }
    
    // Snow biome: high altitude or cold regions (north/south)
    if (height > 40.0f || std::abs(worldZ) > 150.0f) {
        return Biome::SNOW;
    }
    
    // Desert biome: far from origin, specific noise pattern
    if (distance > 100.0f && biomeValue > 0.3f) {
        return Biome::DESERT;
    }
    
    // Forest biome: moderate areas with specific noise
    if (biomeValue < -0.2f && height > 30.0f && height < 38.0f) {
        return Biome::FOREST;
    }
    
    // Default to grassland
    return Biome::GRASSLAND;
}

void World::generateTerrain(Chunk& chunk) {
    glm::ivec3 chunkPos = chunk.getPosition();
    
    // Generate terrain with biomes, caves, and trees
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int worldX = chunkPos.x * CHUNK_SIZE + x;
            int worldZ = chunkPos.z * CHUNK_SIZE + z;
            
            // Height map with noise
            float heightNoise = noise2D(worldX, worldZ);
            float baseHeight = 32.0f;
            float heightVariation = 12.0f;
            float height = baseHeight + heightVariation * heightNoise;
            int groundHeight = static_cast<int>(height);
            
            // Determine biome
            Biome biome = determineBiome(worldX, worldZ, height);
            
            // Adjust height variation based on biome
            if (biome == Biome::DESERT) {
                heightVariation = 4.0f;
                height = baseHeight + heightVariation * heightNoise;
                groundHeight = static_cast<int>(height);
            } else if (biome == Biome::OCEAN) {
                heightVariation = 2.0f;
                height = 26.0f + heightVariation * heightNoise; // Lower base for ocean
                groundHeight = static_cast<int>(height);
            } else if (biome == Biome::SNOW) {
                heightVariation = 8.0f;
                if (groundHeight < 40) {
                    height = 40.0f + heightVariation * heightNoise;
                    groundHeight = static_cast<int>(height);
                }
            }
            
            // Generate blocks
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int worldY = chunkPos.y * CHUNK_HEIGHT + y;
                
                // Improved cave generation - less caves in ocean
                if (biome != Biome::OCEAN) {
                    float density = caveDensity(worldX * 0.15f, worldY * 0.15f, worldZ * 0.15f);
                    bool isCave = density > 0.35f && worldY < groundHeight - 5 && worldY > 5;
                    
                    if (isCave) {
                        chunk.setBlock(x, y, z, BlockType::AIR);
                        continue;
                    }
                }
                
                // Terrain layers based on biome
                BlockType surfaceBlock = BlockType::GRASS;
                BlockType dirtBlock = BlockType::DIRT;
                
                switch (biome) {
                    case Biome::DESERT:
                        surfaceBlock = BlockType::SAND;
                        dirtBlock = BlockType::SAND;
                        break;
                    case Biome::SNOW:
                        surfaceBlock = BlockType::SNOW;
                        dirtBlock = BlockType::DIRT;
                        break;
                    case Biome::OCEAN:
                        surfaceBlock = BlockType::SAND;
                        dirtBlock = BlockType::SAND;
                        break;
                    case Biome::FOREST:
                    case Biome::GRASSLAND:
                    default:
                        surfaceBlock = BlockType::GRASS;
                        dirtBlock = BlockType::DIRT;
                        break;
                }
                
                if (worldY < groundHeight - 4) {
                    // Deep stone
                    chunk.setBlock(x, y, z, BlockType::STONE);
                } else if (worldY < groundHeight - 1) {
                    // Dirt layer
                    chunk.setBlock(x, y, z, dirtBlock);
                } else if (worldY == groundHeight - 1) {
                    // Top dirt layer
                    chunk.setBlock(x, y, z, dirtBlock);
                } else if (worldY == groundHeight) {
                    // Surface
                    chunk.setBlock(x, y, z, surfaceBlock);
                } else {
                    chunk.setBlock(x, y, z, BlockType::AIR);
                }
            }
            
            // Generate ores in stone layers (second pass)
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int worldY = chunkPos.y * CHUNK_HEIGHT + y;
                if (worldY < groundHeight - 4) { // Only in stone layers
                    generateOre(chunk, x, y, z, worldX, worldY, worldZ);
                }
            }
            
            // Generate stalactites/stalagmites in caves (third pass)
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int worldY = chunkPos.y * CHUNK_HEIGHT + y;
                if (chunk.getBlockType(x, y, z) == BlockType::AIR && 
                    worldY < groundHeight - 5 && worldY > 5) {
                    generateStalactites(chunk, x, y, z, worldX, worldY, worldZ);
                }
            }
            
            // Generate trees based on biome
            int treeChance = 0;
            if (biome == Biome::FOREST) {
                treeChance = 8; // ~17% chance in forests
            } else if (biome == Biome::GRASSLAND) {
                treeChance = 2; // ~4% chance in grasslands
            }
            // No trees in desert, snow, or ocean
            
            if (treeChance > 0 && groundHeight < CHUNK_HEIGHT - 10) {
                int worldY = groundHeight + 1;
                int localY = worldY - chunkPos.y * CHUNK_HEIGHT;
                
                // Random tree placement
                if (localY >= 0 && localY < CHUNK_HEIGHT &&
                    (worldX * 7 + worldZ * 11) % 47 < treeChance) {
                    generateTree(chunk, x, localY, z);
                }
            }
        }
    }
}

void World::unloadDistantChunks(const glm::vec3& playerPos) {
    glm::ivec3 playerChunk = worldToChunk(static_cast<int>(playerPos.x), 
                                          static_cast<int>(playerPos.y), 
                                          static_cast<int>(playerPos.z));
    
    auto it = m_chunks.begin();
    while (it != m_chunks.end()) {
        ChunkKey key = it->first;
        int dx = std::abs(key.x - playerChunk.x);
        int dy = std::abs(key.y - playerChunk.y);
        int dz = std::abs(key.z - playerChunk.z);
        
        if (dx > RENDER_DISTANCE || dy > RENDER_DISTANCE || dz > RENDER_DISTANCE) {
            // Save chunk before unloading
            saveChunk(it->second.get(), m_worldName);
            
            it = m_chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void World::update(const glm::vec3& playerPos) {
    glm::ivec3 playerChunk = worldToChunk(static_cast<int>(playerPos.x), 
                                          static_cast<int>(playerPos.y), 
                                          static_cast<int>(playerPos.z));
    
    // Load chunks around player
    for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
                int chunkX = playerChunk.x + x;
                int chunkY = playerChunk.y + y;
                int chunkZ = playerChunk.z + z;
                
                Chunk* chunk = getOrCreateChunk(chunkX, chunkY, chunkZ);
                if (chunk->needsMeshUpdate()) {
                    // Generate mesh with world block query function for cross-chunk culling
                    auto worldQuery = [this](int x, int y, int z) { return this->getBlock(x, y, z); };
                    chunk->generateMesh(worldQuery);
                }
            }
        }
    }
    
    // Unload distant chunks
    unloadDistantChunks(playerPos);
}

void World::render(Shader& shader, unsigned int texture) {
    for (auto& pair : m_chunks) {
        pair.second->render(shader, texture);
    }
}

void World::render(Shader& shader, unsigned int texture, const Frustum& frustum) {
    for (auto& pair : m_chunks) {
        Chunk* chunk = pair.second.get();
        
        // Get chunk bounding box
        glm::vec3 min, max;
        chunk->getBoundingBox(min, max);
        
        // Only render if chunk is inside frustum
        if (frustum.isAABBInside(min, max)) {
            chunk->render(shader, texture);
        }
    }
}

std::string World::getChunkFilePath(int chunkX, int chunkY, int chunkZ, const std::string& worldName) const {
    std::stringstream ss;
    ss << "saves/" << worldName << "/chunks/" << chunkX << "_" << chunkY << "_" << chunkZ << ".chunk";
    return ss.str();
}

void World::saveAllChunks() const {
    for (const auto& pair : m_chunks) {
        saveChunk(pair.second.get(), m_worldName);
    }
}

bool World::saveChunk(const Chunk* chunk, const std::string& worldName) const {
    if (!chunk) {
        return false;
    }
    
    // Serialize chunk data
    std::vector<uint8_t> data;
    chunk->serialize(data);
    
    // Get file path
    std::string filePath = getChunkFilePath(chunk->getPosition().x, 
                                            chunk->getPosition().y, 
                                            chunk->getPosition().z, 
                                            worldName);
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filePath);
    std::filesystem::create_directories(path.parent_path());
    
    // Write to file
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
    
    return true;
}

bool World::loadChunk(Chunk& chunk, int chunkX, int chunkY, int chunkZ, const std::string& worldName) const {
    // Get file path
    std::string filePath = getChunkFilePath(chunkX, chunkY, chunkZ, worldName);
    
    // Check if file exists
    if (!std::filesystem::exists(filePath)) {
        return false;
    }
    
    // Read file
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();
    
    // Deserialize chunk
    return chunk.deserialize(data);
}

