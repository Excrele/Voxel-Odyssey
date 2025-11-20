#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Frustum planes for culling
struct Frustum {
    glm::vec4 planes[6]; // left, right, bottom, top, near, far
    
    // Extract frustum from view-projection matrix
    void extractFromMatrix(const glm::mat4& mvp);
    
    // Test if a point is inside the frustum
    bool isPointInside(const glm::vec3& point) const;
    
    // Test if an AABB (axis-aligned bounding box) intersects the frustum
    bool isAABBInside(const glm::vec3& min, const glm::vec3& max) const;
    
    // Normalize a plane equation
    void normalizePlane(glm::vec4& plane);
};

