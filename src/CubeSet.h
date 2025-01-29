#pragma once

#include <vector>
#include <Cube.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <cmath>
#include <queue>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CubeSet
{
    private:
    
        std::vector<Cube*> Cubes;
        std::vector<std::vector<std::vector<int>>> CubeToLocation;
        std::queue<int> WallRotations;

        glm::mat4 CubeTranslationMatrix;
        glm::mat4 CubeRotationMatrix;
        glm::mat4 CubeScaleMatrix;

        std::vector<int> totalRotation; 
        bool Clockwise = true;
        int RotationAngle = 2;
        float CurrentRotationAngle = 0.0f;
        std::vector<int> WallIndices;
        int ActiveRotations;
        double LastFrameTime;
        bool EnableColorPicking = false;

        bool CanRotate(int wall);
        void ApplyRotation(int wall);
        void UpdateCubePositions(int wall);
        std::vector<int> GetWallCubeIndices(int wall);
        void InitializeWallIndices(int num_of_cubes);


    public:
        CubeSet(int numCubes, Shader* shader, Texture* texture, VertexArray* vertexArray);
        CubeSet() = default;

        void RotateWall(int wall);        
        void SwapRotationAxis(char dir);
        void SetClockWise();        
        void RotateLeft();
        void RotateRight();
        void RotateDown();
        void RotateUp();
        void RotateFront();
        void RotateBack();
        void Rotate(float angle, glm::vec3 axis);

        void Mix();
            
        void Render(glm::mat4 view, glm::mat4 proj, double frame_time);

        // void GetCubePosition(int cubeID);
        void Translate(int cube_id, glm::vec3 trans_vec);
        void RotationCube(int cube_id, float angle, glm::vec3 axis);
        void TogglePicking(bool mode);
        bool IdentifyPick();
        void Restart();
        void Buffer(glm::mat4 view, glm::mat4 proj);
        glm::vec3 GetCubePosition(int cubeID);
        
};