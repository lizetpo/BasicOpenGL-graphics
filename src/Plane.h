#ifndef PLANE_H
#define PLANE_H

#include "Object.h"

class Plane : public Object {
public:
    glm::vec3 normal;
    float d;

    Plane(const glm::vec3 &n, float dVal, const Material &mat);
    bool intersect(const Ray &ray, float &t, glm::vec3 &normalOut) const override;
};

#endif // PLANE_H
