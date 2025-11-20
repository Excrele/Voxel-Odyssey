#pragma once
#include <glm/glm.hpp>

enum class BlockType : uint8_t {
    AIR = 0,
    GRASS,
    DIRT,
    STONE,
    SAND,
    WOOD,
    LEAVES,
    GRAVEL,
    SNOW,
    COAL_ORE,
    IRON_ORE,
    COUNT
};

struct Block {
    BlockType type = BlockType::AIR;
    //Later: Light level, etc.

};

//Tex coords for atlas (uvs per face; atlas is 4x4 for 16 types)
inline glm::vec2 getTexCoord(BlockType type, int face) {
    // Texture atlas: 4x4 grid, each block type gets one cell
    // Face: 0=front, 1=back, 2=top, 3=bottom, 4=right, 5=left
    
    float texSize = 0.25f; // Size of one texture in atlas
    
    // Get base texture coordinates for this block type
    int typeIndex = (int)type;
    float baseX = (typeIndex % 4) * texSize;
    float baseY = (typeIndex / 4) * texSize;
    
    // Different blocks have different face textures
    switch (type) {
        case BlockType::GRASS:
            // Grass: top uses grass, sides use grass side, bottom uses dirt
            if (face == 2) { // Top
                return {baseX, baseY};
            } else if (face == 3) { // Bottom
                int dirtIndex = (int)BlockType::DIRT;
                return {(dirtIndex % 4) * texSize, (dirtIndex / 4) * texSize};
            } else { // Sides - use grass side texture (assume it's next to grass)
                return {baseX + texSize, baseY}; // Offset for side texture
            }
            
        case BlockType::WOOD:
            // Wood: top/bottom use wood rings, sides use wood bark
            if (face == 2 || face == 3) { // Top/Bottom
                return {baseX, baseY};
            } else { // Sides
                return {baseX + texSize, baseY}; // Side texture
            }
            
        case BlockType::LEAVES:
        case BlockType::SAND:
        case BlockType::GRAVEL:
        case BlockType::DIRT:
        case BlockType::STONE:
        case BlockType::SNOW:
        case BlockType::COAL_ORE:
        case BlockType::IRON_ORE:
        default:
            // All faces use same texture
            return {baseX, baseY};
    }
}