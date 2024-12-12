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

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'e') { // Camera position
            float x, y, z, mode;
            iss >> x >> y >> z >> mode;
            scene.cameraPosition = glm::vec3(x, y, z);
        } else if (type == 'a') { // Ambient light
            float r, g, b, mode;
            iss >> r >> g >> b >> mode;
            scene.ambientLight = glm::vec3(r, g, b);
        } else if (type == 'o') { // Objects
            float x, y, z, r;
            iss >> x >> y >> z >> r;
            if (r > 0) { // Sphere
                scene.addObject(std::make_shared<Sphere>(
                    glm::vec3(x, y, z), r, Material(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, 0.0f)
                ));
            } else { // Plane
                scene.addObject(std::make_shared<Plane>(
                    glm::vec3(x, y, z), r, Material(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, 0.0f)
                ));
            }
        } else if (type == 'c') { // Colors
            float r, g, b, shininess;
            iss >> r >> g >> b >> shininess;
            if (objectIndex < scene.objects.size()) {
                scene.objects[objectIndex++]->material = Material(
                    glm::vec3(r, g, b), glm::vec3(r, g, b), glm::vec3(0.7f), shininess, 0.0f
                );
            }
        } else if (type == 'd') { // Directional lights
            float x, y, z, mode;
            iss >> x >> y >> z >> mode;
            scene.addLight(std::make_shared<DirectionalLight>(
                glm::vec3(x, y, z), glm::vec3(1.0f)
            ));
        } else if (type == 'p') { // Spotlights
            float x, y, z, cutoff;
            iss >> x >> y >> z >> cutoff;
            glm::vec3 position(x, y, z);
            if (!scene.lights.empty()) {
                auto spotlight = std::dynamic_pointer_cast<SpotLight>(scene.lights.back());
                if (spotlight) {
                    spotlight->position = position;
                    spotlight->cutoff = cutoff;
                }
            }
        } else if (type == 'i') { // Light intensity
            float r, g, b, mode;
            iss >> r >> g >> b >> mode;
            if (!scene.lights.empty()) {
                scene.lights.back()->intensity = glm::vec3(r, g, b);
            }
        }
    }

    file.close();
}
