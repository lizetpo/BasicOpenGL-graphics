#include "Material.h"

Material::Material(const glm::vec3 &a, const glm::vec3 &d, const glm::vec3 &s, float sh)
    : ambient(a), diffuse(d), specular(s), shininess(sh) {}