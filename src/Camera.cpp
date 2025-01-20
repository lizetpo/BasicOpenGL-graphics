#include <Camera.h>

void Camera::SetPerspective( float near, float far)
{
    m_Aspect = (float) m_Width / (float) m_Height;
    m_FOVdegree = 45.0f;
    m_Near = near;
    m_Far = far;

    // Rest Projection and View matrices
    m_Projection = glm::perspective(glm::radians(m_FOVdegree), m_Aspect, near, far);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}


void Camera::SetOrthographic(float near, float far)
{
    m_Near = near;
    m_Far = far;

    // Rest Projection and View matrices
    m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, near, far);
    m_View = lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

void Camera::TranslateProjection(glm::vec3 trans)
{
    m_Projection = glm::translate(m_Projection, trans);
}

void Camera::TranslateView(glm::vec3 trans)
{
    m_View = glm::translate(m_View, trans);
}

void Camera::RotateProjection(float angle, glm::vec3 rot)
{
    m_Projection = glm::rotate(m_Projection, angle, rot);
}

void Camera::RotateView(float angle, glm::vec3 rot)
{
    m_View = glm::rotate(m_View, angle, rot);
}

/////////////////////
// Input Callbacks //
/////////////////////

void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    float sensitivity = camera->m_KeySensitivity;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_UP:
                camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
                break;
            case GLFW_KEY_DOWN:
                camera->TranslateView(glm::vec3(0.0f, sensitivity, 0.0f));
                break;
            case GLFW_KEY_LEFT:
                camera->TranslateView(glm::vec3(sensitivity, 0.0f, 0.0f));
                break;
            case GLFW_KEY_RIGHT:
                camera->TranslateView(glm::vec3(-sensitivity, 0.0f, 0.0f));
                break;
            default:
                break;
        }
    }
}

void CursorPosCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    float sensitivity = camera->m_MouseSensitivity;

    camera->m_NewMouseX = camera->m_OldMouseX - currMouseX;
    camera->m_NewMouseY = camera->m_OldMouseY - currMouseY;
    camera->m_OldMouseX = currMouseX;
    camera->m_OldMouseY = currMouseY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glm::mat3 viewRotateInverse = glm::transpose(glm::mat3(camera->GetViewMatrix()));

        camera->TranslateView(viewRotateInverse * glm::vec3((float) camera->m_NewMouseX * sensitivity, (float) -camera->m_NewMouseY * sensitivity, 0.0f));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glm::mat3 viewRotateInverse = glm::transpose(glm::mat3(camera->GetViewMatrix()));
        camera->RotateView((float) camera->m_NewMouseX * sensitivity, viewRotateInverse * glm::vec3(0.0f, -1.0f, 0.0f));
        camera->RotateView((float) camera->m_NewMouseY * sensitivity, viewRotateInverse * glm::vec3(-1.0f, 0.0f, 0.0f));
    }
}

void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY)
{
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
        return;
    }

    float sensitivity = camera->m_ScrollSensitivity;

    camera->TranslateView(glm::vec3(0.0f, 0.0f, scrollOffsetY * sensitivity));
}

void Camera::EnableInputs(GLFWwindow* window)
{
    // Set camera as the user pointer for the window
    glfwSetWindowUserPointer(window, this);

    // Handle key inputs
    glfwSetKeyCallback(window, (void(*)(GLFWwindow *, int, int, int, int)) KeyCallback);

    // Handle cursor postion and inputs
    glfwSetCursorPosCallback(window , (void(*)(GLFWwindow *, double, double)) CursorPosCallback);

    // Handle scroll inputs
    glfwSetScrollCallback(window, (void(*)(GLFWwindow *, double, double)) ScrollCallback);
}