#include <Camera.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

// Track mouse button states
bool isLeftMousePressed = false;
bool isRightMousePressed = false;

void Camera::SetOrthographic(float near, float far)
{
    m_Near = near;
    m_Far = far;

    m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, near, far);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

void Camera::setPerspective(float near, float far) {
    m_Near = near;
    m_Far = far;

    m_Projection = glm::perspective(glm::radians(45.0f), 1.0f, near, far);

    m_Position = glm::vec3(0.0f, 0.0f, 20.0f);
    m_View = glm::lookAt(
        m_Position, // Adjust z-value if needed
        m_Position+m_Orientation, // Look at the origin
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
    );
}


/////////////////////
// Input Callbacks //
/////////////////////

void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_UP:
                std::cout << "UP Pressed" << std::endl;
                break;
            case GLFW_KEY_DOWN:
                std::cout << "DOWN Pressed" << std::endl;
                break;
            case GLFW_KEY_LEFT:
                std::cout << "LEFT Pressed" << std::endl;
                break;
            case GLFW_KEY_RIGHT:
                std::cout << "RIGHT Pressed" << std::endl;
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        isLeftMousePressed = (action == GLFW_PRESS);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        isRightMousePressed = (action == GLFW_PRESS);
    }
}

void CursorPosCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! CursorPosCallback is skipped" << std::endl;
        return;
    }


    static double lastMouseX = currMouseX;
    static double lastMouseY = currMouseY;

    double deltaX = currMouseX - lastMouseX;
    double deltaY = currMouseY - lastMouseY;

    lastMouseX = currMouseX;
    lastMouseY = currMouseY;

    const float sensitivity = 0.3f; // Adjust sensitivity for smoother movement
    
    
    if (isLeftMousePressed)
    {

        std::cout << "Position: " << glm::to_string(camera->GetPosition()) << std::endl;
         // Rotate around the global Y-axis (horizontal movement)
    const float sensitivity = 0.1f; // Adjust for smooth control

    // Horizontal rotation: Rotate around the global Y-axis
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians((float)-deltaX * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

    // Vertical rotation: Rotate around the camera's right vector
    glm::vec3 right = glm::normalize(glm::cross(camera->GetOrientation(), camera->GetUpVector()));
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians((float)-deltaY * sensitivity), right);

    // Combine rotations
    glm::mat4 combinedRotation = rotationY * rotationX;

    // Update orientation
    glm::vec3 newOrientation = glm::normalize(glm::mat3(combinedRotation) * camera->GetOrientation());

    // Update up vector (recalculate to stay perpendicular to orientation)
    glm::vec3 newUp = glm::normalize(glm::cross(glm::cross(newOrientation, camera->GetUpVector()), newOrientation));

    // Set updated orientation and up vector
    camera->SetOrientation(newOrientation);
    camera->SetUpVector(newUp);

    // Update the view matrix
    camera->SetViewMatrix(glm::lookAt(
        camera->GetPosition(),
        camera->GetPosition() + camera->GetOrientation(),
        camera->GetUpVector()
    ));
    }
    else if (isRightMousePressed)
    {
          // Get the camera's orientation vectors
        glm::vec3 forward = glm::normalize(camera->GetOrientation());
        glm::vec3 up = glm::normalize(camera->GetUpVector());
        glm::vec3 right = glm::normalize(glm::cross(forward, up));

        // Move the camera left/right based on horizontal mouse movement
        if (std::abs(deltaX) > std::abs(deltaY)) {
            glm::vec3 deltaPosition = right * (float)(-deltaX * sensitivity); // Horizontal movement (left-right)
            glm::vec3 newPosition = camera->GetPosition() + deltaPosition;
            camera->SetPosition(newPosition);
        }
        // Move the camera up/down based on vertical mouse movement
        else {
            glm::vec3 deltaPosition = up * (float)(deltaY * sensitivity); // Vertical movement (up-down)
            glm::vec3 newPosition = camera->GetPosition() + deltaPosition;
            camera->SetPosition(newPosition);
        }

        // Update the view matrix based on the new position
        camera->SetViewMatrix(glm::lookAt(camera->GetPosition(),
                                          camera->GetPosition() + camera->GetOrientation(),
                                          camera->GetUpVector()));
    }
}


void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY)
{
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
        return;
    }

    glm::vec3 newPosition = camera->GetPosition() + glm::vec3(0.0f, 0.0f, (float)-scrollOffsetY * 0.5f);
    camera->SetPosition(newPosition);
    camera->SetViewMatrix(glm::lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetOrientation(), camera->GetUpVector()));
}
void Camera::EnableInputs(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}
