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
    size_t  objectIndex = 0;
    size_t  lightCounter = 0;  // Track the number of lights added (both directional and spotlights)
    size_t  spotlightCounter = 0;  // Track the number of spotlights for positions

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
        else if (type == 'o' || type == 'r' || type == 't') {  // Objects (Sphere or Plane)
            float x, y, z, r;
            iss >> x >> y >> z >> r;
            bool reflective, transparent = false;
            if(type == 'r'){
                reflective = true;
            }
            else if(type == 't'){
                transparent = true; 
            }
            if (r > 0) {  // Sphere
                std::shared_ptr<Object> obj = std::make_shared<Sphere>(glm::vec3(x, y, z), r, Material(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, 0.0f));

                if (type == 'r') { // Reflective
                    obj->material.specular = glm::vec3(1.0f); // Full specular reflection
                    obj->material.transparency = 0.0f;       // Not transparent
                } else if (type == 't') { // Transparent
                    obj->material.specular = glm::vec3(0.0f); // No specular
                    obj->material.transparency = 1.0f;       // Fully transparent
                }
                else{
                    obj->material.transparency = -1.0f;       // Fully transparent
                }
                
                scene.addObject(obj );

            } else {  // Plane
                std::shared_ptr<Object> obj = std::make_shared<Plane>(glm::vec3(x, y, z), r, Material(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, 0.0f));

                if (type == 'r') { // Reflective
                    obj->material.specular = glm::vec3(1.0f); // Full specular reflection
                    obj->material.transparency = 0.0f;       // Not transparent
                } else if (type == 't') { // Transparent
                    obj->material.specular = glm::vec3(0.0f); // No specular
                    obj->material.transparency = 1.0f;       // Fully transparent
                }
                else{
                    obj->material.transparency = -1.0f;       // Fully transparent
                }

                scene.addObject(
                    obj
                );
            }
        } 
        else if (type == 'c') {  // Colors for the objects
            float r, g, b, shininess;
            iss >> r >> g >> b >> shininess;
            if (objectIndex < scene.objects.size()) {
                auto obj = scene.objects[objectIndex++];
                obj->material.ambient = glm::vec3(r, g, b);
                obj->material.diffuse = glm::vec3(r, g, b);
                obj->material.shininess = shininess;

                // Reflective/transparent objects ignore material colors
                if (obj->material.transparency > 0.0f || obj->material.specular == glm::vec3(1.0f)) {
                    obj->material.ambient = glm::vec3(0.0f);
                    obj->material.diffuse = glm::vec3(0.0f);
                }
            }
        }
        else if (type == 'd') {  
            float x, y, z, mode;
            iss >> x >> y >> z >> mode;  
            glm::vec3 direction = glm::vec3(x, y, z);

            if (mode == 0.0f) {  
                // Directional Light
                scene.addLight(std::make_shared<DirectionalLight>(
                    direction, glm::vec3(1.0f)
                ));
            } else {  
                // Spotlight
                float cutoff = 0.6f;
                scene.addLight(std::make_shared<SpotLight>(
                    glm::vec3(0.0f, 0.0f, 1.0f), direction, cutoff, glm::vec3(1.0f)
                ));
                spotlightCounter++;            
            }
        } 
        else if (type == 'p') { 
            static size_t spotlightHandler = 0; 
            float x, y, z, cutoff;
            iss >> x >> y >> z >> cutoff;

            if (!scene.lights.empty() && spotlightHandler < spotlightCounter) {
                auto spotLight = std::dynamic_pointer_cast<SpotLight>(scene.lights[spotlightHandler]);
                if (spotLight) {
                    // Update spotlight properties
                    spotLight->position = glm::vec3(x, y, z);
                    spotLight->cutoff = cutoff;                

                    spotlightHandler++; 
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
