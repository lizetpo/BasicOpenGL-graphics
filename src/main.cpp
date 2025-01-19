
#define GLM_ENABLE_EXPERIMENTAL


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

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

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
const float near = 0.1f;
const float far = 100.0f;
const float fov = 45.0f;  


/* Full cube vertices with positions, colors, and texture coordinates */
float vertices[] = {
    // positions          // colors         // texCoords
    -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Back face
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

    -1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Front face
     1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Left face
    -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

     1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Right face
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

    -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom face
    -1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,

    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,  // Top face
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
};


/* Indices for vertices order */
unsigned int indices[] = { 0, 1, 2,
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

    for (int z = 0; z <CUBE_SIZE; z++) {
				allCubes[z] = Cube(z);
				cubesIndex[z] = z;
	}
    
    reset_transformations();


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
        glm::mat4 scaleS = scale(glm::mat4(1), glm::vec3(0.98f, 0.98f, 0.98f));
        
        glfwSetKeyCallback(window, key_callback);


    while (!glfwWindowShouldClose(window)) {

    for (animation = 0; animation <= 40; animation++) {
        // Delay for smooth animation
        std::this_thread::sleep_for(std::chrono::milliseconds(25)); // Adjust speed by changing delay
    GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    inMovement=false;
    //
    int index = 0;
    for (int z = -size + 1; z < size; z = z + 2) {
        for (int y = -size + 1; y < size; y = y + 2) {
            for (int x = size - 1; x >= -size +1; x = x - 2, index++) {
                glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f); // Default color

                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                float frame_prog = glm::clamp((float)animation / 40.0f, 0.0f, 1.0f);
                    glm::mat4 animated_rotation = glm::interpolate(
                        
                        allCubes[index].oldRotMatrix, // Starting rotation
                        allCubes[index].rotMatrix,    // Target rotation
                        frame_prog                          // Progress ratio
                    );
               
                glm::mat4 model = animated_rotation *trans* scaleS;

                glm::mat4 mvp = camera.GetProjectionMatrix() * camera.GetViewMatrix() * model;

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
    
    // Normalize rotation matrices for cubes that were rotated

        for (int i = 0; i < CUBE_SIZE; i++) { 
            allCubes[i].oldRotMatrix = allCubes[i].rotMatrix; // Store final state
        }


    global = false;
    inMovement = false; // Allow new movement
    
}
        
    }
    
    glfwTerminate();
    return 0;
}
