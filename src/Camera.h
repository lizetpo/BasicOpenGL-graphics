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

class Camera
{
    private:
        // View and Projection
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);

        // View matrix paramters
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 20.0f);;
        glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Projection matrix parameters
        float m_Near = 0.1f; 
        float m_Far = 100.0f;
        int m_Width;
        int m_Height;

        // Orthographic Projection parameters
        float m_Left = -1.0f;
        float m_Right = 1.0f;
        float m_Bottom = -1.0f; 
        float m_Top = 1.0f;
    public:
        // Prevent the camera from jumping around when first clicking left click
        double m_OldMouseX = 0.0;
        double m_OldMouseY = 0.0;
        double m_NewMouseX = 0.0;
        double m_NewMouseY = 0.0;
    public:
        Camera(int width, int height)
            : m_Width(width), m_Height(height) {};

        // Update Projection matrix for Orthographic mode
        void SetOrthographic(float near, float far);
        void setPerspective(float near, float far);

        // Handle camera inputs
        void EnableInputs(GLFWwindow* window);

        inline glm::mat4 GetViewMatrix() const { return m_View; }
        inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }
        

        // Add getters and setters for other private members
        glm::vec3 GetPosition() const { return m_Position; }
        glm::vec3 GetOrientation() const { return m_Orientation; }
        glm::vec3 GetUpVector() const { return m_Up; }

        void SetViewMatrix(const glm::mat4& view) { m_View = view; }
        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetOrientation(const glm::vec3& orientation) { m_Orientation = orientation; }
        void SetUpVector(const glm::vec3& up) { m_Up = up; }
        void SetProjection(const glm::mat4& proj) { m_Projection = proj; }
        void setRotationAroundAxis(const glm::vec3& axis, float angle);
};