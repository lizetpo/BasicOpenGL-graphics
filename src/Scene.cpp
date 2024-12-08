#include "Scene.h"

Scene::Scene(const glm::vec3 &bgColor, const glm::vec3 &ambient, const glm::vec3 &camera)
    : backgroundColor(bgColor), ambientLight(ambient), cameraPosition(camera) {}

void Scene::addObject(const std::shared_ptr<Object> &object) {
    objects.push_back(object);
}

void Scene::addLight(const std::shared_ptr<Light> &light) {
    lights.push_back(light);
}
