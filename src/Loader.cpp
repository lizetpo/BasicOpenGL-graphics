#include "Loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Sphere.h"
#include "Plane.h"

void Loader::loadScene(const std::string &filename, Scene &scene) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << filename << std::endl;
        return;
    }

    std::string line;
    int objectIndex = 0;
    int lightCounter = 0;  // Track the number of lights added (both directional and spotlights)
    int spotlightCounter = 0;  // Track the number of spotlights for positions

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'e') {  // Camera position
            float x, y, z, mode;
            iss >> x >> y >> z >> mode;
            scene.cameraPosition = glm::vec3(x, y, z);
        } 
        else if (type == 'a') {  // Ambient light
            float r, g, b, mode;
            iss >> r >> g >> b >> mode;
            scene.ambientLight = glm::vec3(r, g, b);
        } 
        else if (type == 'o') {  // Objects (Sphere or Plane)
            float x, y, z, r;
            iss >> x >> y >> z >> r;
            if (r > 0) {  // Sphere
                scene.addObject(std::make_shared<Sphere>(
                    glm::vec3(x, y, z), r, Material(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, 0.0f)
                ));
            } else {  // Plane
                scene.addObject(std::make_shared<Plane>(
                    glm::vec3(x, y, z), r, Material(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, 0.0f)
                ));
            }
        } 
        else if (type == 'c') {  // Colors for the objects
            float r, g, b, shininess;
            iss >> r >> g >> b >> shininess;
            if (objectIndex < scene.objects.size()) {
                scene.objects[objectIndex++]->material = Material(
                    glm::vec3(r, g, b), glm::vec3(r, g, b), glm::vec3(0.0f), shininess, 0.0f
                );
            }
        } 
        else if (type == 'd') {  
            float x, y, z, mode;
            iss >> x >> y >> z >> mode;  
            glm::vec3 direction = glm::vec3(x, y, z);  

            if (mode == 0.0f) {  
                scene.addLight(std::make_shared<DirectionalLight>(
                    direction, glm::vec3(1.0f)  
                ));
            } else {  // Spotlight
                float cutoff = 0.6f;  
                scene.addLight(std::make_shared<SpotLight>(
                    direction, direction, cutoff, glm::vec3(1.0f)  
                ));
                spotlightCounter++;  
            }
        } 
        else if (type == 'p') {  
            float x, y, z, cutoff;
            iss >> x >> y >> z >> cutoff;

            // Ensure the last added light is a spotlight
            if (spotlightCounter > 0 && spotlightCounter <= scene.lights.size()) {
                auto spotlight = std::dynamic_pointer_cast<SpotLight>(scene.lights[spotlightCounter - 1]);  
                if (spotlight) {
                    spotlight->position = glm::vec3(x, y, z);  
                    spotlight->cutoff = cutoff; 
                }
            }
        } 
        else if (type == 'i') {  
            float r, g, b, mode;
            iss >> r >> g >> b >> mode;
            if (lightCounter <= scene.lights.size()) {
                scene.lights[lightCounter++]->intensity = glm::vec3(r, g, b);  
            }
        }
    }

    file.close();
}
