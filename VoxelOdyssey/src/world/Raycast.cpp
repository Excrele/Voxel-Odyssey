#include "Raycast.h"
#include <cmath>
#include <algorithm>

RaycastResult Raycast::cast(const glm::vec3& origin, const glm::vec3& direction, 
                           float maxDistance,
                           std::function<BlockType(int, int, int)> blockQuery) {
    RaycastResult result;
    result.hit = false;
    result.distance = maxDistance;
    
    // Normalize direction
    glm::vec3 dir = glm::normalize(direction);
    
    // Current block position (floored)
    glm::ivec3 currentBlock(
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    );
    
    // Step direction for each axis
    glm::ivec3 step(
        dir.x > 0 ? 1 : -1,
        dir.y > 0 ? 1 : -1,
        dir.z > 0 ? 1 : -1
    );
    
    // Calculate delta distances
    glm::vec3 deltaDist(
        dir.x == 0 ? 1e30f : std::abs(1.0f / dir.x),
        dir.y == 0 ? 1e30f : std::abs(1.0f / dir.y),
        dir.z == 0 ? 1e30f : std::abs(1.0f / dir.z)
    );
    
    // Calculate side distances
    glm::vec3 sideDist;
    if (dir.x < 0) {
        sideDist.x = (origin.x - currentBlock.x) * deltaDist.x;
    } else {
        sideDist.x = (currentBlock.x + 1.0f - origin.x) * deltaDist.x;
    }
    if (dir.y < 0) {
        sideDist.y = (origin.y - currentBlock.y) * deltaDist.y;
    } else {
        sideDist.y = (currentBlock.y + 1.0f - origin.y) * deltaDist.y;
    }
    if (dir.z < 0) {
        sideDist.z = (origin.z - currentBlock.z) * deltaDist.z;
    } else {
        sideDist.z = (currentBlock.z + 1.0f - origin.z) * deltaDist.z;
    }
    
    // DDA algorithm
    float distance = 0.0f;
    int side = 0;
    
    while (distance < maxDistance) {
        // Check current block
        BlockType blockType = blockQuery(currentBlock.x, currentBlock.y, currentBlock.z);
        
        if (blockType != BlockType::AIR) {
            result.hit = true;
            result.blockPos = currentBlock;
            result.distance = distance;
            
            // Determine which face was hit
            if (side == 0) {
                result.faceNormal = glm::ivec3(-step.x, 0, 0);
            } else if (side == 1) {
                result.faceNormal = glm::ivec3(0, -step.y, 0);
            } else {
                result.faceNormal = glm::ivec3(0, 0, -step.z);
            }
            
            return result;
        }
        
        // Move to next block
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            sideDist.x += deltaDist.x;
            currentBlock.x += step.x;
            side = 0;
            distance = sideDist.x - deltaDist.x;
        } else if (sideDist.y < sideDist.z) {
            sideDist.y += deltaDist.y;
            currentBlock.y += step.y;
            side = 1;
            distance = sideDist.y - deltaDist.y;
        } else {
            sideDist.z += deltaDist.z;
            currentBlock.z += step.z;
            side = 2;
            distance = sideDist.z - deltaDist.z;
        }
    }
    
    return result;
}

glm::ivec3 Raycast::getAdjacentBlock(const glm::ivec3& blockPos, const glm::ivec3& faceNormal) {
    return blockPos + faceNormal;
}

