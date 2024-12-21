#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
public:
    glm::vec3 intensity;

    Light(const glm::vec3 &i);
    virtual glm::vec3 getDirection(const glm::vec3 &point) const = 0;
    virtual ~Light() {}
};

class DirectionalLight : public Light {
public:
    glm::vec3 direction;

    DirectionalLight(const glm::vec3 &dir, const glm::vec3 &i);
    glm::vec3 getDirection(const glm::vec3 &point) const override;
};

class SpotLight : public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutoff;

    SpotLight(const glm::vec3 &pos, const glm::vec3 &dir, float co, const glm::vec3 &i);
    glm::vec3 getDirection(const glm::vec3 &point) const override;
    bool isWithinCutoff(const glm::vec3 &point) const;
};

#endif // LIGHT_H
