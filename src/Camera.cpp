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
    if(m_ColorPicking){
        if (m_PickedCube >= 0) // Check if an object is picked
        {
            m_Cubes->rotate_cube(m_PickedCube, (float)-m_NewMouseX * m_MouseSensitivity * 0.5f, m_Up);
            m_Cubes->rotate_cube(m_PickedCube, (float)-m_NewMouseY * m_MouseSensitivity * 0.5f, glm::normalize(glm::cross(m_Orientation, m_Up)));

        }
    }else{
        m_View = glm::rotate(m_View, angle, rot);
    }
}

void Camera::ToggleColorPicking()
{
    m_PickedCube = -1;
    m_ColorPicking = !m_ColorPicking;
    m_Cubes->swap_picking(m_ColorPicking);
}

void Camera::PickCube(double x, double y) //TODO
{

    unsigned char color_picked[]{ 0, 0, 0, 0 };
    glReadPixels(x, 800 - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color_picked);
    int color_id = color_picked[0];
    if(color_id > 27) m_PickedCube = -1;
    else m_PickedCube = color_id;
    printf("\ncube id: %d\n", color_id);
}

void Camera::ColorPick() 
{       
    printf("picked cube: %d\n", m_PickedCube);
    if (m_PickedCube >= 0) 
    {
        float depth_factor = m_Far + m_Depth * (m_Near - m_Far);
        glm::vec3 screen_pos = glm::vec3(m_NewMouseX, m_NewMouseY, depth_factor);
        glm::vec3 world_pos = glm::unProject(screen_pos, m_View, m_Projection, glm::vec4(0.0f, 0.0f, m_Width, m_Height));
        
        glm::vec3 translation_vector = world_pos - m_Position;
        m_Cubes->translate_cube(m_PickedCube, translation_vector);
    }
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
                camera->m_Cubes->change_center_of_rotation('^');
                break;
            case GLFW_KEY_DOWN:
                camera->TranslateView(glm::vec3(0.0f, sensitivity, 0.0f));
                camera->m_Cubes->change_center_of_rotation('v');
                break;
            case GLFW_KEY_LEFT:
                camera->TranslateView(glm::vec3(sensitivity, 0.0f, 0.0f));
                camera->m_Cubes->change_center_of_rotation('<');
                break;
            case GLFW_KEY_RIGHT:
                camera->TranslateView(glm::vec3(-sensitivity, 0.0f, 0.0f));
                camera->m_Cubes->change_center_of_rotation('>');
                break;
            case GLFW_KEY_I:
                camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
                camera->m_Cubes->change_center_of_rotation('I');
                break;
            case GLFW_KEY_O:
                camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
                camera->m_Cubes->change_center_of_rotation('O');
                break;
            case GLFW_KEY_R:
                camera->m_Cubes->rotate_wall(1); // Right
                break;
            case GLFW_KEY_L:
                camera->m_Cubes->rotate_wall(0); // Left
                break;
            case GLFW_KEY_U:
                camera->m_Cubes->rotate_wall(5); // Up
                break;
            case GLFW_KEY_D:
                camera->m_Cubes->rotate_wall(4); // Down
                break;
            case GLFW_KEY_B:
                camera->m_Cubes->rotate_wall(2); // Back
                break;
            case GLFW_KEY_F:
                camera->m_Cubes->rotate_wall(3); // Front
                break;
            // case GLFW_KEY_SPACE:
            //     camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
            //     camera->swap_clockwise();
            //     break;
            // case GLFW_KEY_Z:
            //     camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
            //     camera->set_degree(0.5f);
            //     break;
            // case GLFW_KEY_A:
            //     camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
            //     camera->set_degree(2.0f);
            //     break;
            // case GLFW_KEY_M:
            //     camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
            //     camera->mix();
            //     break;
            case GLFW_KEY_P:
                camera->ToggleColorPicking();
                break;
            // case GLFW_KEY_Q:
            //     camera->TranslateView(glm::vec3(0.0f, -sensitivity, 0.0f));
            //     camera->clean_start();
            //     break;
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
        printf("%d\n", camera->m_ColorPicking);
        if(camera->m_ColorPicking == false)
        {
            camera->ColorPick();
        }else{
        glm::mat3 viewRotateInverse = glm::transpose(glm::mat3(camera->GetViewMatrix()));

        camera->TranslateView(viewRotateInverse * glm::vec3((float) camera->m_NewMouseX * sensitivity, (float) -camera->m_NewMouseY * sensitivity, 0.0f));
        }
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

void MouseButtonCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    printf("mouse button");
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if(camera->m_ColorPicking == true)
        {
                printf("picking left");

            camera->PickCube(camera->m_OldMouseX, camera->m_OldMouseY);
        } 
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if(camera->m_ColorPicking == true)
        {
                printf("picking right");

            glReadPixels(currMouseX, 800 - currMouseY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &camera->m_Depth);
            camera->PickCube(camera->m_OldMouseX, camera->m_OldMouseY);
        }
    }
}

void Camera::EnableInputs(GLFWwindow* window)
{
    // Set camera as the user pointer for the window
    glfwSetWindowUserPointer(window, this);

    // Handle key inputs
    glfwSetKeyCallback(window, (void(*)(GLFWwindow *, int, int, int, int)) KeyCallback);

    glfwSetMouseButtonCallback(window, (void(*)(GLFWwindow *, int, int, int)) MouseButtonCallback);

    // Handle cursor postion and inputs
    glfwSetCursorPosCallback(window , (void(*)(GLFWwindow *, double, double)) CursorPosCallback);

    // Handle scroll inputs
    glfwSetScrollCallback(window, (void(*)(GLFWwindow *, double, double)) ScrollCallback);


}