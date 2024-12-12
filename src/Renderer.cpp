#include "Renderer.h"
#include "Plane.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Renderer::Renderer(Scene &scene, int width, int height, int maxDepth)
    : scene(scene), width(width), height(height), maxDepth(maxDepth) {}

void Renderer::render(std::vector<glm::vec3> &imageBuffer) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = (2.0f * (x + 0.5f) / width - 1.0f);
            float v = (1.0f - 2.0f * (y + 0.5f) / height);
            glm::vec3 rayDir = glm::normalize(glm::vec3(u, v, -1.0f));
            Ray ray(scene.cameraPosition, rayDir);
            imageBuffer[y * width + x] = trace(ray, maxDepth);
        }
    }
}

glm::vec3 Renderer::trace(const Ray &ray, int depth) {
    if (depth <= 0) return scene.backgroundColor;

    glm::vec3 hitPoint, normal;
    Material material;
    int hitObjectIndex;

    float t = findNearestIntersection(ray, hitPoint, normal, material, hitObjectIndex);
    if (t == FLT_MAX) {
        return scene.backgroundColor; // No intersection
    }

    glm::vec3 viewDir = glm::normalize(-ray.direction);
    glm::vec3 color = glm::vec3(0.0f); // Initialize color to black

    if (dynamic_cast<Plane *>(scene.objects[hitObjectIndex].get())) {
        material.diffuse = computeCheckerboardColor(material.diffuse, hitPoint);
    }

    color = computePhongLighting(hitPoint, normal, viewDir, material);

    if (material.specular != glm::vec3(0.0f)) {
        glm::vec3 reflectDir = glm::reflect(ray.direction, normal);
        Ray reflectedRay(hitPoint + 1e-4f * reflectDir, reflectDir);
        color += trace(reflectedRay, depth - 1) * material.specular;
    }

    if (material.transparency > 0.0f) {
        float eta = 1.0f / 1.5f;
        if (glm::dot(ray.direction, normal) > 0.0f) {
            normal = -normal;
            eta = 1.5f / 1.0f;
        }
        glm::vec3 refractDir = glm::refract(ray.direction, normal, eta);
        Ray refractedRay(hitPoint + 1e-4f * refractDir, refractDir);
        color += trace(refractedRay, depth - 1) * material.transparency;
    }

    return glm::clamp(color, 0.0f, 1.0f);
}

glm::vec3 Renderer::computeCheckerboardColor(const glm::vec3 &baseColor, const glm::vec3 &hitPoint) {
    float scale = 0.5f;
    int checkX = static_cast<int>(floor(hitPoint.x / scale)) % 2;
    int checkZ = static_cast<int>(floor(hitPoint.z / scale)) % 2;
    bool isDark = (checkX + checkZ) % 2 == 0;
    return isDark ? 0.5f * baseColor : baseColor;
}

float Renderer::findNearestIntersection(const Ray &ray, glm::vec3 &hitPoint, glm::vec3 &normal,
                                         Material &material, int &hitObjectIndex) {
    float nearestT = FLT_MAX;
    hitObjectIndex = -1;

    for (size_t i = 0; i < scene.objects.size(); ++i) {
        float t;
        glm::vec3 n;
        if (scene.objects[i]->intersect(ray, t, n) && t < nearestT) {
            nearestT = t;
            hitObjectIndex = i;
            hitPoint = ray.origin + t * ray.direction;
            normal = n;
            material = scene.objects[i]->material;
        }
    }

    return nearestT;
}

glm::vec3 Renderer::computePhongLighting(const glm::vec3 &hitPoint, const glm::vec3 &normal,
                                         const glm::vec3 &viewDir, const Material &material) {
    glm::vec3 ambient = scene.ambientLight * material.ambient;
    glm::vec3 diffuse(0.0f);
    glm::vec3 specular(0.0f);

    for (const auto &light : scene.lights) {
        glm::vec3 lightDir = glm::normalize(light->getDirection(hitPoint));

        // Shadows
        Ray shadowRay(hitPoint + 1e-4f * lightDir, lightDir);
        bool isShadowed = false;
        for (const auto &object : scene.objects) {
            float t;
            glm::vec3 n;
            if (object->intersect(shadowRay, t, n)) {
                isShadowed = true;
                break;
            }
        }
        if (isShadowed) continue;

        // Diffuse
        float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
        diffuse += material.diffuse * light->intensity * diff;

        // Specular
        glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
        float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), material.shininess);
        specular += material.specular * light->intensity * spec;
    }

    return ambient + diffuse + specular;
}