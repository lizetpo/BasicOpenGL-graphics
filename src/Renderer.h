#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Ray.h"

class Renderer {
public:
    Renderer(Scene &scene, int width, int height, int maxDepth);

    void render(std::vector<glm::vec3> &imageBuffer);

private:
    Scene &scene;
    int width;
    int height;
    int maxDepth;

    glm::vec3 trace(const Ray &ray, int depth);
    glm::vec3 computePhongLighting(const glm::vec3 &hitPoint, const glm::vec3 &normal,
                                   const glm::vec3 &viewDir, const Material &material);
    glm::vec3 computeCheckerboardColor(const glm::vec3 &baseColor, const glm::vec3 &hitPoint);
    float findNearestIntersection(const Ray &ray, glm::vec3 &hitPoint, glm::vec3 &normal,
                                  Material &material, int &hitObjectIndex);
    bool checkShadow(const glm::vec3& hitPoint, const glm::vec3& lightPos, const glm::vec3& lightDir, const std::vector<std::shared_ptr<Object>>& objects);

};

#endif // RENDERER_H
