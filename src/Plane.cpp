#include "Plane.h"

Plane::Plane(const glm::vec3 &n, float dVal, const Material &mat)
    : Object(mat), normal(glm::normalize(n)), d(dVal) {}

bool Plane::intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const {
    float denom = glm::dot(ray.direction, normal);
    if (fabs(denom) > 1e-6) {
        t = -(glm::dot(ray.origin, normal) + d) / denom;
        if (t >= 0) {
            normalOut = normal;
            return true;
        }
    }
    return false;
}
