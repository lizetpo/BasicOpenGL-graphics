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
        material.ambient = computeCheckerboardColor(material.ambient, hitPoint);
    }

    color = computePhongLighting(hitPoint, normal, viewDir, material);

    // if (material.specular != glm::vec3(0.0f)) {
    //     glm::vec3 reflectDir = glm::reflect(ray.direction, normal);
    //     Ray reflectedRay(hitPoint + 1e-4f * reflectDir, reflectDir);
    //     color += trace(reflectedRay, depth - 1) * material.specular;
    // }

    // if (material.transparency > 0.0f) {
    //     float eta = 1.0f / 1.5f;
    //     if (glm::dot(ray.direction, normal) > 0.0f) {
    //         normal = -normal;
    //         eta = 1.5f / 1.0f;
    //     }
    //     glm::vec3 refractDir = glm::refract(ray.direction, normal, eta);
    //     Ray refractedRay(hitPoint + 1e-4f * refractDir, refractDir);
    //     color += trace(refractedRay, depth - 1) * material.transparency;
    // }
    return glm::clamp(color, 0.0f, 1.0f);
}

glm::vec3 Renderer::computeCheckerboardColor(const glm::vec3 &baseColor, const glm::vec3 &hitPoint) {
   float scaleParameter = 0.5f;
    float checkerboard = 0;
    if (hitPoint.x < 0) {
        checkerboard += floor((0.5 - hitPoint.x) / scaleParameter);
    } else {
        checkerboard += floor(hitPoint.x / scaleParameter);
    }
    if (hitPoint.z < 0) {
        checkerboard += floor((0.5 - hitPoint.z) / scaleParameter);
    } else {
        checkerboard += floor(hitPoint.z / scaleParameter);
    }
    checkerboard = (checkerboard * 0.5) - int(checkerboard * 0.5);
    checkerboard *= 2;
    return checkerboard > 0.5f ? 0.5f * baseColor : baseColor;
}

float Renderer::findNearestIntersection(const Ray &ray, glm::vec3 &hitPoint, glm::vec3 &normal,
                                         Material &material, int &hitObjectIndex) {
    float nearestT = FLT_MAX;
    hitObjectIndex = -1;

    if (scene.objects.empty()) {
        return nearestT;
    }

    for (size_t i = 0; i < scene.objects.size(); ++i) {
        float t;
        glm::vec3 n;
        if (scene.objects[i]->intersect(ray, t, n) && t < nearestT) {
            nearestT = t;
            hitObjectIndex = i;
            hitPoint = ray.origin + t * ray.direction;
            normal = glm::normalize(n);
            material = scene.objects[i]->material;
        }
    }

    return nearestT;
}


// glm::vec3 Renderer::computePhongLighting(const glm::vec3 &hitPoint, const glm::vec3 &normal,
//                                          const glm::vec3 &viewDir, const Material &material) {
//     glm::vec3 ambient = scene.ambientLight * material.ambient;
//     glm::vec3 diffuse(0.0f);
//     glm::vec3 specular(0.0f);

//     for (const auto &light : scene.lights) {
//         glm::vec3 lightDir = glm::normalize(light->getDirection(hitPoint));

    
//         // Diffuse lighting
//         float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
//         diffuse += material.diffuse * light->intensity * diff;
        
//         glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
//         float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), material.shininess);
//         specular += material.specular * light->intensity * spec;
//     }

//     return glm::clamp(ambient + diffuse + specular, 0.0f, 1.0f);
// }
glm::vec3 Renderer::computePhongLighting(const glm::vec3 &hitPoint, const glm::vec3 &normal,
                                         const glm::vec3 &viewDir, const Material &material) {
    glm::vec3 ambient = scene.ambientLight * material.ambient;
    glm::vec3 diffuse(0.0f);
    glm::vec3 specular(0.0f);
    
    for (const auto &light : scene.lights) {
        glm::vec3 lightDir;
        glm::vec3 lightPos;
        float attenuation = 1.0f;  // Default attenuation is 1 (no attenuation)

        // Check if the light is a directional light
        if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
            lightDir = directionalLight->getDirection(hitPoint);  // Directional light direction
        }
        // Check if the light is a spotlight
        else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(light)) {
            lightPos = spotLight->position;  // Spotlight position
            lightDir = spotLight->getDirection(hitPoint);  // Spotlight direction
            
            // Calculate the distance from the point to the light
            float distance = glm::length(lightPos - hitPoint);

            // Apply attenuation (you can tune k_c, k_1, k_q)
            float k_c = 1.0f, k_1 = 0.1f, k_q = 0.01f;  // Example constants
            attenuation = 1.0f / (k_c + k_1 * distance + k_q * distance * distance);
        } else {
            continue;  // Skip any other type of light (e.g., point light)
        }

        glm::vec3 lightDirNormalized = glm::normalize(lightDir);
        Ray shadowRay(hitPoint + lightDirNormalized * 1e-4f, lightDirNormalized);  // Add small bias to avoid self-intersection

        bool inShadow = false;
        for (const auto &object : scene.objects) {
            float t;
            glm::vec3 n;
            if (object->intersect(shadowRay, t, n) && t >= 1.0f) {
                inShadow = true;
                break;
            }
        }

        if (inShadow) {
            continue;  // If in shadow, skip this light source
        }

        // Diffuse lighting
        float diff = glm::max(glm::dot(normal, lightDirNormalized), 0.0f);
        diffuse += material.diffuse * light->intensity * diff * attenuation;  // Apply attenuation

        // Specular lighting
        glm::vec3 reflectDir = glm::reflect(-lightDirNormalized, normal);
        float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), material.shininess);
        specular += material.specular * light->intensity * spec * attenuation;  // Apply attenuation
    }

    return glm::clamp(ambient + diffuse + specular, 0.0f, 1.0f);
}