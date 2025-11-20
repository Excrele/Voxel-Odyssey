#pragma once
#include "Block.h"
#include <glm/glm.hpp>
#include <functional>

struct RaycastResult {
    bool hit;
    glm::ivec3 blockPos;      // Position of the hit block
    glm::ivec3 faceNormal;    // Normal of the face that was hit
    float distance;            // Distance from ray origin to hit point
};

class Raycast {
public:
    // Cast a ray from origin in direction, returns the first solid block hit
    // Uses DDA (Digital Differential Analyzer) algorithm for voxel raycasting
    static RaycastResult cast(const glm::vec3& origin, const glm::vec3& direction, 
                             float maxDistance,
                             std::function<BlockType(int, int, int)> blockQuery);
    
    // Helper to get the adjacent block position (for block placement)
    static glm::ivec3 getAdjacentBlock(const glm::ivec3& blockPos, const glm::ivec3& faceNormal);
};

