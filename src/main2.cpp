#include "Scene.h"      // Include the Scene class
#include "Loader.h"     // Include the Loader class
#include "Renderer.h"   // Include the Renderer class
#include <glm/glm.hpp>  // For glm::vec3
#include <vector>       // For std::vector
#include <iostream>     // For std::cout and std::cerr

// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // Include stb_image_write for PNG output

const int WIDTH = 800; // Define the image width
const int HEIGHT = 800; // Define the image height

int main2() {
    // Setup Scene
    Scene scene(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 5.0f));
    Loader::loadScene("../src/scene6.txt", scene); // Adjust the path to the scene file

    // Setup Renderer
    Renderer renderer(scene, WIDTH, HEIGHT, 5);
    std::vector<glm::vec3> imageBuffer(WIDTH * HEIGHT); // Create the image buffer
    renderer.render(imageBuffer); // Perform the rendering

    // Convert imageBuffer to unsigned char array for PNG output
    std::vector<unsigned char> pixelData(WIDTH * HEIGHT * 3);
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        pixelData[i * 3 + 0] = static_cast<unsigned char>(glm::clamp(imageBuffer[i].r, 0.0f, 1.0f) * 255.0f);
        pixelData[i * 3 + 1] = static_cast<unsigned char>(glm::clamp(imageBuffer[i].g, 0.0f, 1.0f) * 255.0f);
        pixelData[i * 3 + 2] = static_cast<unsigned char>(glm::clamp(imageBuffer[i].b, 0.0f, 1.0f) * 255.0f);
    }

    // Save as PNG
    const char* outputFilename = "output.png"; // Output filename
    if (stbi_write_png(outputFilename, WIDTH, HEIGHT, 3, pixelData.data(), WIDTH * 3)) {
        std::cout << "Image saved as " << outputFilename << std::endl;
    } else {
        std::cerr << "Failed to save image." << std::endl;
    }

    return 0;
}
