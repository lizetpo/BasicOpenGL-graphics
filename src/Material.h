#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

class Material {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Material(const glm::vec3 &a, const glm::vec3 &d, const glm::vec3 &s, float sh);
};

#endif