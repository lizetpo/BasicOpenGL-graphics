#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"

class Sphere : public Object {
public:
    glm::vec3 center;
    float radius;

    Sphere(const glm::vec3 &c, float r, const Material &mat);
    bool intersect(const Ray &ray, float &t, glm::vec3 &normal) const override;
};

#endif // SPHERE_H
