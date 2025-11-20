#include "Frustum.h"
#include <algorithm>

void Frustum::normalizePlane(glm::vec4& plane) {
    float mag = std::sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
    if (mag > 0.0f) {
        plane.x /= mag;
        plane.y /= mag;
        plane.z /= mag;
        plane.w /= mag;
    }
}

void Frustum::extractFromMatrix(const glm::mat4& mvp) {
    // Extract the 6 planes from the view-projection matrix
    // Left plane
    planes[0].x = mvp[0][3] + mvp[0][0];
    planes[0].y = mvp[1][3] + mvp[1][0];
    planes[0].z = mvp[2][3] + mvp[2][0];
    planes[0].w = mvp[3][3] + mvp[3][0];
    
    // Right plane
    planes[1].x = mvp[0][3] - mvp[0][0];
    planes[1].y = mvp[1][3] - mvp[1][0];
    planes[1].z = mvp[2][3] - mvp[2][0];
    planes[1].w = mvp[3][3] - mvp[3][0];
    
    // Bottom plane
    planes[2].x = mvp[0][3] + mvp[0][1];
    planes[2].y = mvp[1][3] + mvp[1][1];
    planes[2].z = mvp[2][3] + mvp[2][1];
    planes[2].w = mvp[3][3] + mvp[3][1];
    
    // Top plane
    planes[3].x = mvp[0][3] - mvp[0][1];
    planes[3].y = mvp[1][3] - mvp[1][1];
    planes[3].z = mvp[2][3] - mvp[2][1];
    planes[3].w = mvp[3][3] - mvp[3][1];
    
    // Near plane
    planes[4].x = mvp[0][3] + mvp[0][2];
    planes[4].y = mvp[1][3] + mvp[1][2];
    planes[4].z = mvp[2][3] + mvp[2][2];
    planes[4].w = mvp[3][3] + mvp[3][2];
    
    // Far plane
    planes[5].x = mvp[0][3] - mvp[0][2];
    planes[5].y = mvp[1][3] - mvp[1][2];
    planes[5].z = mvp[2][3] - mvp[2][2];
    planes[5].w = mvp[3][3] - mvp[3][2];
    
    // Normalize all planes
    for (int i = 0; i < 6; i++) {
        normalizePlane(planes[i]);
    }
}

bool Frustum::isPointInside(const glm::vec3& point) const {
    for (int i = 0; i < 6; i++) {
        float distance = planes[i].x * point.x + planes[i].y * point.y + 
                        planes[i].z * point.z + planes[i].w;
        if (distance < 0.0f) {
            return false; // Point is outside this plane
        }
    }
    return true; // Point is inside all planes
}

bool Frustum::isAABBInside(const glm::vec3& min, const glm::vec3& max) const {
    // Test AABB against frustum planes
    // For each plane, check if the AABB is on the positive side
    for (int i = 0; i < 6; i++) {
        // Find the "positive vertex" - the vertex of the AABB that is farthest
        // in the positive direction of the plane normal
        glm::vec3 positiveVertex;
        positiveVertex.x = (planes[i].x >= 0.0f) ? max.x : min.x;
        positiveVertex.y = (planes[i].y >= 0.0f) ? max.y : min.y;
        positiveVertex.z = (planes[i].z >= 0.0f) ? max.z : min.z;
        
        // If the positive vertex is outside the plane, the AABB is outside
        float distance = planes[i].x * positiveVertex.x + 
                        planes[i].y * positiveVertex.y + 
                        planes[i].z * positiveVertex.z + 
                        planes[i].w;
        
        if (distance < 0.0f) {
            return false; // AABB is outside this plane
        }
    }
    return true; // AABB intersects or is inside the frustum
}

