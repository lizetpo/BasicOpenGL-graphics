#include "Light.h"

// Base Light
Light::Light(const glm::vec3 &i) : intensity(i) {}

// Directional Light
DirectionalLight::DirectionalLight(const glm::vec3 &dir, const glm::vec3 &i)
    : Light(i), direction(glm::normalize(dir)) {}

glm::vec3 DirectionalLight::getDirection(const glm::vec3 &point) const {
    return -direction; // Directional light is from infinity
}

// SpotLight
SpotLight::SpotLight(const glm::vec3 &pos, const glm::vec3 &dir, float co, const glm::vec3 &i)
    : Light(i), position(pos), direction(glm::normalize(dir)), cutoff(co) {}

glm::vec3 SpotLight::getDirection(const glm::vec3 &point) const {
    return glm::normalize(position - point);
}

bool SpotLight::isWithinCutoff(const glm::vec3 &point) const {
    glm::vec3 dirToLight = glm::normalize(position - point);
    return glm::dot(dirToLight, direction) > cutoff;
}
