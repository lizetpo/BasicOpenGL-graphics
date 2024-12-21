#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include "Material.h"
#include "Ray.h"

class Object {
public:
    Material material;

    Object(const Material &mat);
    virtual bool intersect(const Ray &ray, float &t, glm::vec3 &normal) const = 0;
    virtual ~Object() {}
};

#endif // OBJECT_H
