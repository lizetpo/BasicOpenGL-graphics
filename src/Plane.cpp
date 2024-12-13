#include "Plane.h"

Plane::Plane(const glm::vec3 &n, float dVal, const Material &mat)
    : Object(mat), normal(glm::normalize(n)), d(dVal) {}  // Normalize once at creation


bool Plane::intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const {
    // Normalize the plane normal
    glm::vec3 normalizedNormal = glm::normalize(normal);

    // Calculate the denominator (N . D)
    float denom = glm::dot(ray.direction, normalizedNormal);

    // Check if the ray is parallel to the plane
    if (fabs(denom) > 1e-6) {  // Avoid division by near-zero values
        // Calculate t using the ray-plane intersection formula
        glm::vec3 pointOnPlane = -d * normalizedNormal;  // Q = -d * N (point on plane)
        float numerator = glm::dot(pointOnPlane - ray.origin, normalizedNormal);

        t = numerator / denom;

        // If t < 0, the intersection is behind the ray origin
        if (t < 0) {
            return false;  // No valid intersection
        }

        // Correct the normal's orientation
        if (denom > 0) {
            normalOut = -normalizedNormal;  // Flip the normal to point away from the ray
        } else {
            normalOut = normalizedNormal;  // Normal already faces the correct direction
        }

        return true;  // Valid intersection
    }

    // No intersection: ray is parallel to the plane
    return false;
}