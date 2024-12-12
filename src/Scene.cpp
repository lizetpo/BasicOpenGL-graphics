#include "Scene.h"

// Default constructor
Scene::Scene()
    : backgroundColor(glm::vec3(0.0f, 0.0f, 0.0f)),
      ambientLight(glm::vec3(0.1f, 0.1f, 0.1f)),
      cameraPosition(glm::vec3(0.0f, 0.0f, 5.0f)) {}

// Parameterized constructor
Scene::Scene(const glm::vec3 &bgColor, const glm::vec3 &ambient, const glm::vec3 &camera)
    : backgroundColor(bgColor), ambientLight(ambient), cameraPosition(camera) {}

// Add an object to the scene
void Scene::addObject(const std::shared_ptr<Object>& obj) { // Match header
    objects.push_back(obj);
}

// Add a light source to the scene
void Scene::addLight(const std::shared_ptr<Light>& light) { // Match header
    lights.push_back(light);
}
