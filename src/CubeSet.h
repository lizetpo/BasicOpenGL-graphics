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
        // Cubes data structure:
        std::vector<Cube*> Cubes;
        std::vector<std::vector<std::vector<int>>> CubeToLocation;
        std::queue<int> WallRotations;

        // Model matrices
        glm::mat4 CubeSetTranslation;
        glm::mat4 CubeSetRotation;
        glm::mat4 CubeSetScale;

        // Locks & rotations
        std::vector<int> accumulatedRotation; 
        bool Clockwise = true;
        int RotateDegree = 2;
        float CurrentRotationAngle = 0.0f;
        std::vector<int> WallIndices; // 0 = L, 1 = R, 2 = D, 3 = U, 4 = F, 5 = B 
        int ActiveRotations;
        double LastFrameTime;
        bool ColorPicking = false;

        bool CanRotate(int wall);
        void ApplyRotation(int wall);
        void UpdateCubePositions(int wall);
        std::vector<int> GetWallCubeIndices(int wall);
        void InitializeWallIndices(int num_of_cubes);


    public:
        CubeSet(int numCubes, Shader* shader, Texture* texture, VertexArray* vertexArray);
        CubeSet() = default;

        // WALL ROTATIONS
        void RotateWall(int wall);        
        void SwapRotationAxis(char dir);
        void SetClockWise();
        
        void RotateLeft();
        void RotateRight();
        void RotateDown();
        void RotateUp();
        void RotateFront();
        void RotateBack();


        // MIX & SOLVE

        void Mix();
        
        // VIEW
        void Rotate(float angle, glm::vec3 axis);

        // RENDER
        void Render(glm::mat4 view, glm::mat4 proj, double frame_time);

        // PICKING
        void Translate(int cube_id, glm::vec3 trans_vec);
        void RotationCube(int cube_id, float angle, glm::vec3 axis);
        void TogglePicking(bool mode);
        bool IdentifyPick();
        void Restart();
        void Buffer(glm::mat4 view, glm::mat4 proj);
        glm::vec3 GetCubePosition(int cubeID);
        
};