#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

class Material {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float transparency;

    Material(const glm::vec3 &a = glm::vec3(0.0f),
             const glm::vec3 &d = glm::vec3(0.0f),
             const glm::vec3 &s = glm::vec3(0.0f),
             float shin = 0.0f,
             float transp = 0.0f);
};

#endif // MATERIAL_H
