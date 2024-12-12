
#include <iostream>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Renderer.h"
#include "Loader.h"

int main() {
    // 1. Scene Configuration
    Scene scene(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 5.0f));

    // 2. Load Objects and Lights from Scene File
std::string sceneFile = "C:/dev/BasicOpenGL-graphics/src/scene1.txt";
    Loader::loadScene(sceneFile, scene); // Use your Loader class to parse the scene

    // 3. Renderer Configuration
    int imageWidth = 800;
    int imageHeight = 800;
    int maxRecursionDepth = 5; // Maximum depth for reflections and transparency
    Renderer renderer(scene, imageWidth, imageHeight, maxRecursionDepth);

    // 4. Render Image
    std::vector<glm::vec3> imageBuffer(imageWidth * imageHeight); // Buffer to store pixel colors
    renderer.render(imageBuffer);

    // 5. Save Image
    std::ofstream imageFile("output.ppm");
    if (imageFile.is_open()) {
        imageFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
        for (const auto &color : imageBuffer) {
            int r = static_cast<int>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
            int g = static_cast<int>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
            int b = static_cast<int>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
            imageFile << r << " " << g << " " << b << "\n";
        }
        imageFile.close();
        std::cout << "Image saved as output.ppm\n";
    } else {
        std::cerr << "Failed to save the image.\n";
    }

    return 0;
}
