#include "Plane.h"

Plane::Plane(const glm::vec3 &n, float dVal, const Material &mat)
    : Object(mat), normal(n), d(dVal) {}  // Normalize once at creation


bool Plane::intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const {
    // glm::vec3 normalizedNormal = glm::normalize(normal); // Normalize the plane's normal vector
    float denom = glm::dot(normal, ray.direction);
     if (fabs(denom) ==0) {
        return false;
    }
    t = -(glm::dot(normal, ray.origin)+d) / denom;
    if (t < 0) {
        return false;
    }
    normalOut = -normal;
    return true;
}