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
#include <InputManager.h>

#include <iostream>

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
const float near = 0.1f;
const float far = 100.0f;
const float fov = 45.0f;  // Field Of View in degrees

/* Full cube vertices with positions, colors, and texture coordinates */
float vertices[] = {
    // positions          // colors         // texCoords
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Back face
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Front face
     0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Left face
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Right face
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom face
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,  // Top face
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
};

/* Indices for vertices order */
unsigned int indices[] = {
    0, 1, 2,  2, 3, 0,   // Back face
    4, 5, 6,  6, 7, 4,   // Front face
    8, 9, 10, 10, 11, 8,  // Left face
    12, 13, 14, 14, 15, 12, // Right face
    16, 17, 18, 18, 19, 16, // Bottom face
    20, 21, 22, 22, 23, 20, // Top face
};



int main() {
    if (!glfwInit()) return -1;

    for (int z = 0; z <CUBE_SIZE; z++) {
				allCubes[z] = Cube(z);
				cubesIndex[z] = z;
	}

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

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        GLCall(glEnable(GL_DEPTH_TEST));

        Camera camera(width, height);
        camera.setPerspective(near,far);
        camera.EnableInputs(window);
        glm::mat4 scaleS = scale(glm::mat4(1), glm::vec3(1.0f, 1.0f, 1.0f));

        while (!glfwWindowShouldClose(window)) {

            GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
                    int cubeIndex = 0;

            for (int z = -1; z <= 1; z++) {
                for (int y = -1; y <= 1; y++) {
                    for (int x = -1; x <= 1; x++) {
                        if (x == 0 && y == 0 && z == 0) continue;

                            glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);

                            glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(x, y, z));

                            glm::mat4 model = trans * rot * scaleS;
                            glm::mat4 view = camera.GetViewMatrix();
                            glm::mat4 proj = camera.GetProjectionMatrix();
                            glm::mat4 mvp = proj * view * model;

                            shader.Bind();
                            shader.SetUniform4f("u_Color", color);

                            shader.SetUniformMat4f("u_MVP", mvp);
                            shader.SetUniform1i("u_Texture", 0);
                            va.Bind();
                            ib.Bind();
                            GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
            
                    }
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
    glfwTerminate();
    return 0;
}
