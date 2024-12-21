#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class Ray {
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(const glm::vec3 &o, const glm::vec3 &d);
};

#endif // RAY_H
