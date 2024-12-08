#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Object.h"
#include "Light.h"

class Scene {
public:
    glm::vec3 backgroundColor;
    glm::vec3 ambientLight;
    glm::vec3 cameraPosition;
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    Scene(const glm::vec3 &bgColor, const glm::vec3 &ambient, const glm::vec3 &camera);
    
    void addObject(const std::shared_ptr<Object> &object);
    void addLight(const std::shared_ptr<Light> &light);
};

#endif // SCENE_H