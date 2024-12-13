#include "Plane.h"

Plane::Plane(const glm::vec3 &n, float dVal, const Material &mat)
    : Object(mat), normal(glm::normalize(n)), d(dVal) {}  // Normalize once at creation


bool Plane::intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const {
    glm::vec3 normalizedNormal = glm::normalize(normal);  // Normalize the normal here
    float denom = glm::dot(ray.direction, normalizedNormal);
    if (fabs(denom) > 1e-6) { // Ensure there's no division by zero
        t = -(glm::dot(ray.origin, normalizedNormal) + d) / denom;
        if (t >= 0) {  // Ensure intersection point is in front of the ray
            if (glm::dot(ray.direction, normal) > 0) {
            normalOut = -glm::normalize(normal);  // Flip normal if it points towards the ray
        } else {
            normalOut = glm::normalize(normal);
        }
        return true;
        }
    }
    return false; // No intersection
}

