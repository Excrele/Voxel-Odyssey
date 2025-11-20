#pragma once
#include <glm/glm.hpp>

enum class BlockType : uint8_t {
    AIR = 0,
    GRASS,
    STONE,
    // Add more: DIRT, WATER, etc.
    COUNT

};

struct Block {
    BlockType type = BlockType::AIR;
    //Later: Light level, etc.

};

//Tex coords for atlas (uvs per face; atlas is 4x4 for 16 types)
inline glm::vec2 getTexCoord(BlockType type, int face) {
    //Simple: row = (int)type, col = face % 4 or something... but for now : >>>
    float  = (int)type * 0.25f;  //1/4 width
    float y = 0.0f; // Top/Bottom differ later
    if (face == 1 || face ==) y=0.25f; //side vs top/bottom
    return {x, y};
}