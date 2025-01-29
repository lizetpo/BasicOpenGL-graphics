#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Debugger.h>
#include <Shader.h>
#include "CubeSet.h"

class Camera
{
    private:
        // View and Projection
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);

        // View matrix paramters
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Projection matrix parameters
        float m_Near = 0.1f; 
        float m_Far = 100.0f;
        int m_Width;
        int m_Height;

        // Perspective Projection parameters
        float m_FOVdegree = 45.0f;
        float m_Aspect = 1.0f;

        // Orthographic Projection parameters
        float m_Left = -1.0f * 2.0f;
        float m_Right = 1.0f * 2.0f;
        float m_Bottom = -1.0f * 2.0f; 
        float m_Top = 1.0f * 2.0f;
    public:
        //color picking
        bool m_ColorPicking = false;
        CubeSet* m_Cubes;
        int m_PickedCube = -1;
        float m_Depth = -1.0f;

        // Prevent the camera from jumping around when first clicking left click
        double m_OldMouseX = 0.0;
        double m_OldMouseY = 0.0;
        double m_NewMouseX = 0.0;
        double m_NewMouseY = 0.0;

        // Adjust the speed of the camera and it's sensitivity when looking around
        float m_KeySensitivity = 0.4f;
        float m_MouseSensitivity = 0.05f;
        float m_ScrollSensitivity = 1.0f;
    public:
        Camera(int width, int height, CubeSet* r_cube)
            : m_Width(width), m_Height(height), 
                m_Cubes(r_cube){};

        // Update Projection matrix for Perspective mode
        void SetPerspective(float near, float far);

        // Update Projection matrix for Orthographic mode
        void SetOrthographic(float near, float far);

        void TranslateProjection(glm::vec3 trans);
        void TranslateView(glm::vec3 trans);

        void RotateProjection(float angle, glm::vec3 rot);
        void RotateView(float angle, glm::vec3 rot);

        // Handle camera inputs
        void EnableInputs(GLFWwindow* window);

        inline glm::mat4 GetViewMatrix() const { return m_View; }
        inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }

        void ToggleColorPicking();
        void PickCube(double x, double y);
        void ColorPick();

};