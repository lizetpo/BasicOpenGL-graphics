#include "Plane.h"

Plane::Plane(const glm::vec3 &n, float dVal, const Material &mat)
    : Object(mat), normal(n), d(dVal) {}  // Normalize once at creation


bool Plane::intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const {
    glm::vec3 normalizedNormal = glm::normalize(normal); // Normalize the plane's normal vector
    float denom = glm::dot(normalizedNormal, ray.direction);
    if (fabs(denom) ==0) {
        return false;
    }
    t = glm::dot(normalizedNormal, (-d * normalizedNormal - ray.origin)) / denom;
    if (t < 0) {
        return false;
    }
    normalOut = -normalizedNormal;
    return true;
}