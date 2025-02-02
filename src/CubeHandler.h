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

class CubeHandler
{
    public:

        glm::mat4 CubeTranslationMatrix;
        glm::mat4 CubeScaleMatrix;
        std::vector<int> totalRotation; 
        int Clockwise = 1;
        float CurrentAngle = 0.0f;
        std::vector<int> Indices;
        int ActiveRotations;
		int Angle = 2;
		int GetAxis(int wall);
		int GetLayer(int wall);
		glm::vec3 GetRotationAxis(int wall);
		void UpdateRotationAngle();
		void RotateCubes(const std::vector<int>& cubes, glm::vec3 rotationAxis);
		bool RotationTreshold();
		void ResetRotation(int wall);
		void ExecutePendingRotation();
        bool CanRotate(int axis);
        void UpdateCubePositions(int axis, int wallIndex);
        std::vector<int> GetWall(int axis, int wallIndex);
        void ProcessCubeRotation(int wall);
		Cube* Cubes[27];   // 3×3×3 = 27 cubes
        int CubePositions[3][3][3];
        std::queue<int> PendingRotations;
        glm::mat4 CubeRotationMatrix;
        CubeHandler(Shader* shader, Texture* texture, VertexArray* vertexArray);
        CubeHandler() = default;
		bool EnableColorPicking = false;
        void QueueWallRotation(int wall);        
        void SetClockWise();        
        void RotateLeft();
        void RotateRight();
        void RotateDown();
        void RotateUp();
        void RotateFront();
        void RotateBack();		        
        void Render(glm::mat4 view, glm::mat4 proj);
        void RotationCube(int cube_id, float angle, glm::vec3 axis);
        void Restart();
        void ZBuffer(glm::mat4 view, glm::mat4 proj);
        glm::vec3 GetCubePosition(int cubeID);
        void Mix();
};