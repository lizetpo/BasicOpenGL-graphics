
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <Camera.h>
#include <Cube.h>
#include <thread>
#include <chrono>
#include <iostream>

#include <CubeHandler.h>

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
const float near = 0.1f;
const float far = 100.0f;
const float fov = 45.0f;  

/* Full cube vertices with positions, colors, and texture coordinates */
// float vertices[] = {
//     // positions          // colors         // texCoords
//     -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Back face
//      1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//      1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//     -1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

//     -1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Front face
//      1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
//      1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
//     -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

//     -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Left face
//     -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
//     -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
//     -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

//      1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Right face
//      1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
//      1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
//      1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

//     -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom face
//     -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
//      1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
//      1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,

//     -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,  // Top face
//     -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
//      1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
//      1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
// };

//pretty cube:
float vertices[] = {
    // Positions          // Colors (R, G, B)      // Texture Coords
    -1.0f, -1.0f, -1.0f,   1.0f, 0.75f, 0.8f,    0.0f, 0.0f,  // Baby Pink
     1.0f, -1.0f, -1.0f,   1.0f, 0.75f, 0.8f,    1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 0.75f, 0.8f,    1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 0.75f, 0.8f,    0.0f, 1.0f,

    -1.0f, -1.0f,  1.0f,   1.0f, 0.9f, 0.4f,     0.0f, 0.0f,  // Banana Yellow
     1.0f, -1.0f,  1.0f,   1.0f, 0.9f, 0.4f,     1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.9f, 0.4f,     1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   1.0f, 0.9f, 0.4f,     0.0f, 1.0f,

    -1.0f, -1.0f, -1.0f,   0.54f, 0.81f, 0.94f,  0.0f, 0.0f,  // Baby Blue
    -1.0f,  1.0f, -1.0f,   0.54f, 0.81f, 0.94f,  1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   0.54f, 0.81f, 0.94f,  1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.54f, 0.81f, 0.94f,  0.0f, 1.0f,

     1.0f, -1.0f, -1.0f,   0.78f, 0.64f, 0.92f,  0.0f, 0.0f,  // Lilac Purple
     1.0f,  1.0f, -1.0f,   0.78f, 0.64f, 0.92f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.78f, 0.64f, 0.92f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   0.78f, 0.64f, 0.92f,  0.0f, 1.0f,

    -1.0f, -1.0f, -1.0f,   0.83f, 0.83f, 0.83f,  0.0f, 0.0f,  // Light Grey
    -1.0f, -1.0f,  1.0f,   0.83f, 0.83f, 0.83f,  1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   0.83f, 0.83f, 0.83f,  1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,   0.83f, 0.83f, 0.83f,  0.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,     0.0f, 0.0f,  // White
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
};


/* Indices for vertices order */
unsigned int indices[] = { 
        0, 1, 2,
		0, 2, 3,

		6, 5, 4,
		7, 6, 4,

		10, 9, 8,
		11, 10, 8,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		22, 21, 20,
		23, 22, 20
	};

//main
int main() {
    std::cout.flush();

    if (!glfwInit()) return -1;

    



    GLFWwindow* window = glfwCreateWindow(width, height, "Rubik's Cube", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

     /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Load GLAD so it configures OpenGL */
    gladLoadGL();

    /* Control frame rate */
    glfwSwapInterval(1);

    /* Print OpenGL version after completing initialization */
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* Set scope so that on widow close the destructors will be called automatically */
    {
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(vertices, sizeof(vertices));
        IndexBuffer ib(indices, sizeof(indices));

        VertexBufferLayout layout;
        layout.Push<float>(3);  // positions
        layout.Push<float>(3);  // colors
        layout.Push<float>(2);  // texCoords
        va.AddBuffer(vb, layout);

        Texture texture("res/textures/plane.png");
        texture.Bind();
            
        Shader shader("res/shaders/basic.shader");
        shader.Bind();

        CubeHandler cubes(&shader, &texture, &va);
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        GLCall(glEnable(GL_DEPTH_TEST));
        Camera camera(width, height, &cubes);
        camera.SetPerspective(near,far);
        camera.EnableInputs(window);

    while (!glfwWindowShouldClose(window)) {
    //background black so its pretty
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            glm::mat4 view = camera.GetViewMatrix(); 
            glm::mat4 proj = camera.GetProjectionMatrix();
            
            ib.Bind();
            cubes.Render(view, proj);
            ib.Unbind();
            
            
            glfwSwapBuffers(window);

            if(cubes.EnableColorPicking) 
            {
                ib.Bind();
                GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
                cubes.ZBuffer(view, proj);
                ib.Unbind();
            }

            
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}
