
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
#include <InputManager.h>
#include <thread>
#include <chrono>
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
    for (int z = -1; z <= 1; z++) {
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                if (x == 0 && y == 0 && z == 0) continue; // Skip center cube if necessary

                glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f); // Default color

                int index = (z + 1) * size * size + (y + 1) * size + (x + 1);
                int cubeIndex = cubesIndex[index];  // Map to the correct logical cube after rotations
                // Translate and rotate each cube based on its individual transformations
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                 float progress = glm::clamp((float)animation / 40.0f, 0.0f, 1.0f);
                    glm::mat4 rotAnim = glm::interpolate(
                        allCubes[cubeIndex].oldRotMatrix, // Starting rotation
                        allCubes[cubeIndex].rotMatrix,    // Target rotation
                        progress                          // Progress ratio
                    );
                //glm::mat4 model =trans* allCubes[cubeIndex].rotMatrix * scaleS;
                //if(global){
                    glm::mat4 model = rotAnim*trans* scaleS;
                //}
                // Compute MVP (Model-View-Projection matrix)


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
            int cubeIndex = cubesIndex[i];
            normalize_rotation_matrix(allCubes[cubeIndex].rotMatrix);      
            allCubes[i].oldRotMatrix = allCubes[i].rotMatrix; // Store final state
        }

    global = false;
        inMovement = false; // Allow new movement
    
}
        
    }
    
    glfwTerminate();
    return 0;
}
