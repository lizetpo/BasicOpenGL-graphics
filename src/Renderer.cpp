#include "Renderer.h"
#include "Plane.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Renderer::Renderer(Scene &scene, int width, int height, int maxDepth)
    : scene(scene), width(width), height(height), maxDepth(5) {}

void Renderer::render(std::vector<glm::vec3> &imageBuffer) {
    // Number of samples per pixel (NxN sampling grid)
    int samplesPerPixel = 4; // 2x2 grid of subpixels
    float sampleStep = 1.0f / samplesPerPixel; // Step size for subpixel sampling

    // Calculate the width and height of a single pixel in normalized device coordinates (NDC)
    float pixelWidth = 2.0f / width;  // Pixel width in NDC [-1, 1]
    float pixelHeight = 2.0f / height; // Pixel height in NDC [-1, 1]

    // Iterate over each pixel on the screen
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            glm::vec3 pixelColor(0.0f); // Initialize pixel color

            // Perform supersampling for anti-aliasing
            for (int i = 0; i < samplesPerPixel; ++i) {
                for (int j = 0; j < samplesPerPixel; ++j) {
                    // Calculate the sample's coordinates within the pixel
                    float u = -1.0f + (0.5f*sampleStep + x) * pixelWidth; // Subpixel horizontal offset
                    float v = 1.0f - (0.5f*sampleStep + y ) * pixelHeight; // Subpixel vertical offset                    
                    
                    // Create a ray for this subpixel
                    glm::vec3 rayDir = glm::vec3(u, v, 0)-scene.cameraPosition; // Perspective projection ray direction
                    Ray ray(scene.cameraPosition, rayDir); // Create ray with origin at camera position

                    // Trace the ray and accumulate the color
                    pixelColor += trace(ray, maxDepth);
                }
            }

            // Average the accumulated color from all samples
            pixelColor /= (samplesPerPixel * samplesPerPixel);

            // Store the averaged color in the image buffer
            imageBuffer[y * width + x] = glm::clamp(pixelColor, 0.0f, 1.0f);
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

    glm::vec3 viewDir = -ray.direction;
    glm::vec3 color(0.0f); // Initialize color to black
    
    if (dynamic_cast<Plane *>(scene.objects[hitObjectIndex].get())) {
        material.diffuse = computeCheckerboardColor(material.diffuse, hitPoint);
        material.ambient = computeCheckerboardColor(material.ambient, hitPoint);
    }
    if (material.transparency == 0.0f) {
        glm::vec3 reflectionDir = glm::normalize(glm::reflect(ray.direction, normal));
        Ray reflectionRay(hitPoint + reflectionDir * 1e-4f, reflectionDir);
        color = trace(reflectionRay, depth - 1); // Recursive call for reflection
    } 
    else if (material.transparency > 0.0f) {
        float eta = glm::dot(ray.direction, normal) < 0 ? 1.0f / 1.5f : 1.5f;
        glm::vec3 adjustedNormal = glm::dot(ray.direction, normal) < 0 ? normal : -normal;
        glm::vec3 refractionDir = glm::refract(ray.direction, adjustedNormal, eta);

        glm::vec3 refractionColor(0.0f); // Color from refraction
        glm::vec3 reflectionColor(0.0f); // Color from reflection
        

        // Compute refraction color
        if (glm::length(refractionDir) > 0.0f) { // Refraction occurred
            Ray refractionRay(hitPoint + refractionDir * 1e-4f, refractionDir);
            refractionColor = trace(refractionRay, depth - 1); // Recursive call for refraction
        }
        else {
            // Total internal reflection
            glm::vec3 totalReflectionDir = glm::reflect(ray.direction, normal);
            Ray totalReflectionRay(hitPoint + totalReflectionDir * 1e-4f, totalReflectionDir);
            reflectionColor = trace(totalReflectionRay, depth - 1);
        }

        // Compute reflection color
        glm::vec3 reflectionDir = glm::reflect(ray.direction, normal);
        Ray reflectionRay(hitPoint + reflectionDir * 1e-4f, reflectionDir);
        reflectionColor = trace(reflectionRay, depth - 1);

        // Combine refraction and reflection
        float reflectivity = 0.1f; // Adjust reflectivity factor as needed
        color = (1.0f - material.transparency) * computePhongLighting(hitPoint, normal, viewDir, material) +
                material.transparency * ((1.0f - reflectivity) * refractionColor + reflectivity * reflectionColor);

        // Clamp the final color
        color = glm::clamp(color, 0.0f, 1.0f);
    }
    else {
        // Opaque object - Use Phong shading
        color = computePhongLighting(hitPoint, normal, viewDir, material);
    }

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
    if (hitPoint.y < 0) {
        checkerboard += floor((0.5 - hitPoint.y) / scaleParameter);
    } else {
        checkerboard += floor(hitPoint.y / scaleParameter);
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


glm::vec3 Renderer::computePhongLighting(const glm::vec3 &hitPoint, const glm::vec3 &normal,
                                         const glm::vec3 &viewDir, const Material &material) {
    glm::vec3 ambient = scene.ambientLight * material.ambient;  // Ambient lighting component
    glm::vec3 diffuse(0.0f);  // Initialize diffuse component
    glm::vec3 specular(0.0f);  // Initialize specular component

    for (const auto &light : scene.lights) {
        glm::vec3 lightDir;
        glm::vec3 lightPos;

        if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
            lightDir = directionalLight->getDirection(hitPoint);  // Direction from directional light
        } else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(light)) {
            lightPos = spotLight->position; 
            lightDir = spotLight->getDirection(hitPoint);  
            float distance = glm::length(lightPos - hitPoint);
            float k_c = 1.0f, k_l = 0.1f, k_q = 0.01f;
        } else{
           continue;
        }

        bool inShadow = false;
        if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(light)) {
            inShadow = checkShadow(
                hitPoint,
                spotLight->position,
                spotLight->getDirection(hitPoint),
                scene.objects,
                false,                           // Not a directional light
                spotLight->cutoff,               // Spotlight cutoff angle
                spotLight->direction             // Spotlight direction
            );
        } else if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
            inShadow = checkShadow(
                hitPoint,
                glm::vec3(0.0f),                 // Directional lights have no position
                directionalLight->getDirection(hitPoint),
                scene.objects,
                true                             // This is a directional light
            );
        }
        if (inShadow) continue;  // Skip this light if it's shadowed
        // Calculate Diffuse lighting
        float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
        diffuse += material.diffuse * light->intensity * diff;

        //Calculate Specular lighting
        glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
        float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), material.shininess);
        specular +=  light->intensity * spec;
    }

    return glm::clamp(ambient + diffuse + specular, 0.0f, 1.0f);  // Clamping the color components to be within [0, 1]
}


bool Renderer::checkShadow(const glm::vec3& hitPoint, const glm::vec3& lightPos, const glm::vec3& lightDir, const std::vector<std::shared_ptr<Object>>& objects, bool isDirectional, float cutoff, const glm::vec3& spotlightDir) {
    glm::vec3 shadowRayOrigin = hitPoint + lightDir * 1e-4f;
    glm::vec3 normalizedLightDir = lightDir;
    Ray shadowRay(shadowRayOrigin, glm::normalize(lightDir));

    float lightDistance = isDirectional ? std::numeric_limits<float>::max() : glm::length(lightPos - hitPoint);

    if (!isDirectional && cutoff > 0.0f) {
        float cutoffAngle = glm::degrees(glm::acos(cutoff)); 
        glm::vec3 lightToPointDir = glm::normalize(hitPoint - lightPos);
        float cosTheta = glm::dot(glm::normalize(spotlightDir), lightToPointDir);
        if (glm::acos(cosTheta)> glm::radians(cutoffAngle)) {
            return true;
        }
    }

    for (const auto& object : objects) {
        float t;
        glm::vec3 n; 
        if (object->intersect(shadowRay, t, n) && t > 0 && t < lightDistance) {
            return true;  
        }
    }

    return false;  
}