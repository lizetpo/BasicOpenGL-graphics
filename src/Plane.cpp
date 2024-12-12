#include "Plane.h"

Plane::Plane(const glm::vec3 &n, float dVal, const Material &mat)
    : Object(mat), normal(n), d(dVal) {}

// Intersection function
bool Plane::intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const {
    float denom = glm::dot(ray.direction, glm::normalize(normal));
    if (fabs(denom) > 1e-6) { // Ensure there's no division by zero
        t = -(glm::dot(ray.origin, glm::normalize(normal)) + d) / denom;
        if (t >= 0) {  // Ensure intersection point is in front of the ray
            normalOut = glm::normalize(normal);  // The plane normal at the intersection point
            return true;
        }
    }
    return false; // No intersection
}
