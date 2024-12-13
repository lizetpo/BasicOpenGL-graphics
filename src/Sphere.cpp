#include "Sphere.h"
#include <cmath>

Sphere::Sphere(const glm::vec3 &c, float r, const Material &mat)
    : Object(mat), center(c), radius(r) {}

bool Sphere::intersect(const Ray &ray, float &t, glm::vec3 &normal) const {
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius*radius;
    float discriminant = b * b - 4 * a * c;

    
    if (discriminant < 0) return false;
    float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + sqrt(discriminant)) / (2.0f * a);

    if (t1 > 0) {
        t = t1;
    } else if (t2 > 0) {
        t = t2;
    } else {
        return false; // Both roots are negative, intersection is behind the ray origin
    }
    glm::vec3 hitPoint = ray.origin + t * ray.direction;
    normal = glm::normalize(hitPoint - center);
    return true;
}
