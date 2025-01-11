#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>

class Cube {
public:
    int index;
    glm::mat4 rotMatrix;
    glm::mat4 transMatrix;
    glm::mat4 oldRotMatrix;

    Cube(int ind) {
        index = ind;
        rotMatrix = glm::mat4(1);
        transMatrix = glm::mat4(1);
        oldRotMatrix = glm::mat4(1);
    }

    Cube() {
        index = 0;
    }
};

static const int size = 3;
static const int CUBE_SIZE = size * size * size;
static const int CUBE_FACE_SIZE = size * size;

int FaceMoving = 0;
float totalAngle = 0.0f;
bool inMovement = false;
int clockwise = 1;
float rotationAngle = 90.0f; // Renamed variable
Cube allCubes[CUBE_SIZE];
int cubesIndex[CUBE_SIZE];
float angleSum = 0.0f;
int animation;
int inside = 0;
bool global = false;

inline void normalize_rotation_matrix(glm::mat4& matrix) {
    glm::vec3 xAxis = glm::normalize(glm::vec3(matrix[0]));
    glm::vec3 yAxis = glm::normalize(glm::vec3(matrix[1]));
    glm::vec3 zAxis = glm::normalize(glm::vec3(matrix[2]));

    yAxis = glm::normalize(glm::cross(zAxis, xAxis));
    zAxis = glm::normalize(glm::cross(xAxis, yAxis));

    matrix[0] = glm::vec4(xAxis, 0.0f);
    matrix[1] = glm::vec4(yAxis, 0.0f);
    matrix[2] = glm::vec4(zAxis, 0.0f);
}

inline void rotate_face(const int cubes[], const glm::vec3& localAxis, const float rot_angle) {
    inMovement = true;
    animation = 0;

    // Transform the local axis using the global rotation matrix

    glm::mat4 rotate_face_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(rot_angle), localAxis);

     for (int i = 0; i < CUBE_FACE_SIZE; i++) {
         int cubeIndex = cubesIndex[cubes[i]];

        // Update the oldRotMatrix to store the current rotMatrix before applying the new rotation
		
        allCubes[cubeIndex].oldRotMatrix = allCubes[cubeIndex].rotMatrix;

        // Apply the new rotation based on the previous rotation
        allCubes[cubeIndex].rotMatrix = rotate_face_matrix*allCubes[cubeIndex].rotMatrix ;

        // Normalize the matrix to avoid precision errors over time
        normalize_rotation_matrix(allCubes[cubeIndex].rotMatrix);
    }
}

inline void print_state() {
    for (int i = 0; i < CUBE_SIZE; i++) {
    glm::vec3 cubePos = glm::vec3(allCubes[i].transMatrix[3]); // Extract position from transformation matrix
    std::cout << "Cube " << i << ": Position = " << cubePos.x << ", " << cubePos.y << ", " << cubePos.z << std::endl;
}
    std::cout << "\n";
}

inline bool compare_floats(float A, float B) {
    const float epsilon = 0.01f; // Adjusted epsilon for better precision
    return (fabs(A - B) < epsilon);
}

inline void change_array_index(int indexArray[CUBE_FACE_SIZE], int paramArray[CUBE_FACE_SIZE]) {
    int temp[CUBE_SIZE];
    for (int i = 0; i < CUBE_SIZE; i++) {
        temp[i] = cubesIndex[i];
    }

    for (int i = 0; i < CUBE_FACE_SIZE; i++) {
        if (clockwise == 1) {
            cubesIndex[indexArray[i]] = temp[paramArray[i]];
        } else {
            cubesIndex[paramArray[i]] = temp[indexArray[i]];
        }
    }
    print_state();  // Debugging: Print cube state after index update
}

inline void rotation_checker(int before[], int after[], const glm::vec3& localAxis, int dir, int face) {
    if ((FaceMoving == 0) || (FaceMoving == face)) {
		if(!inMovement){
 			FaceMoving = face;
		}
       
        rotate_face(before, localAxis, dir * clockwise * rotationAngle);

        totalAngle += clockwise * rotationAngle;

        if (compare_floats(fabs(totalAngle), 90.0f)) {
            totalAngle = 0.0f;
            FaceMoving = 0;
            change_array_index(before, after);
            for (int i = 0; i < CUBE_FACE_SIZE; i++) {
                normalize_rotation_matrix(allCubes[cubesIndex[before[i]]].rotMatrix);
            }
            inMovement = false;
        }
		else{
			inMovement = true;
		}
    }
}



inline void reset_transformations() {
    for (int i = 0; i < CUBE_SIZE; i++) {
        int x = i % size;                 // X-coordinate
        int y = (i / size) % size;        // Y-coordinate
        int z = i / (size * size);        // Z-coordinate

        // Center the cube in the grid
        glm::vec3 centerOffset = glm::vec3(size - 1) * 0.5f;
        allCubes[i].transMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) - centerOffset);
        allCubes[i].rotMatrix = glm::mat4(1.0f); // Reset rotation matrix
    }
}

// Key callback example
inline void rotate_u() { // Rotate around Y-axis (Up face)
    switch (size) {
        case 2: {
            int before[4] = {2, 3, 6, 7};
            int after[4] = {3, 7, 2, 6};
            rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 1);
        } break;
        case 3: {
            int before[9] = {6, 7, 8, 15, 16, 17, 24, 25, 26};
            int after[9] = {8, 17, 26, 7, 16, 25, 6, 15, 24};
            rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 1);
        } break;
        default:
            break;
    }
}

inline void rotate_d() { // Rotate around Y-axis (Down face)
    switch (size) {
        case 2: {
            int before[4] = {0, 1, 4, 5};
            int after[4] = {1, 5, 0, 4};
            rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 2);
        } break;
        case 3: {
            int before[9] = {0, 1, 2, 9, 10, 11, 18, 19, 20};
            int after[9] = {2, 11, 20, 1, 10, 19, 0, 9, 18};
            rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 2);
        } break;
        default:
            break;
    }
}

inline void rotate_f() { // Rotate around Z-axis (Front face)
    switch (size) {
        case 2: {
            int before[4] = {0, 1, 2, 3};
            int after[4] = {1, 3, 0, 2};
            rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 3);
        } break;
        case 3: {
            int before[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
            int after[9] = {2, 5, 8, 1, 4, 7, 0, 3, 6};
            rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 3);
        } break;
        default:
            break;
    }
}

inline void rotate_b() { // Rotate around Z-axis (Back face)
    switch (size) {
        case 2: {
            int before[4] = {4, 5, 6, 7};
            int after[4] = {5, 7, 4, 6};
            rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 4);
        } break;
        case 3: {
            int before[9] = {18, 19, 20, 21, 22, 23, 24, 25, 26};
            int after[9] = {20, 23, 26, 19, 22, 25, 18, 21, 24};
            rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 4);
        } break;
        default:
            break;
    }
}

inline void rotate_r() { // Rotate around X-axis (Right face)
    switch (size) {
        case 2: {
            int before[4] = {1, 5, 3, 7};
            int after[4] = {5, 7, 1, 3};
            rotation_checker(before, after, glm::vec3(1, 0, 0), 1, 5);
        } break;
        case 3: {
            int before[9] = {2, 11, 20, 5, 14, 23, 8, 17, 26};
            int after[9] = {20, 23, 26, 11, 14, 17, 2, 5, 8};
            rotation_checker(before, after, glm::vec3(1, 0, 0), 1, 5);
        } break;
        default:
            break;
    }
}

inline void rotate_l() { // Rotate around X-axis (Left face)
    switch (size) {
        case 2: {
            int before[4] = {4, 0, 6, 2};
            int after[4] = {0, 2, 4, 6};
            rotation_checker(before, after, glm::vec3(1, 0, 0), -1, 6);
        } break;
        case 3: {
            int before[9] = {18, 9, 0, 21, 12, 3, 24, 15, 6};
            int after[9] = {0, 3, 6, 9, 12, 15, 18, 21, 24};
            rotation_checker(before, after, glm::vec3(1, 0, 0), -1, 6);
        } break;
        default:
            break;
    }
}



inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		
		if (!inMovement)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				if (action == GLFW_PRESS)
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			// case GLFW_KEY_UP:
			// 	for (int i = 0; i < CUBE_SIZE; i++)
			// 	{
			// 		const glm::mat4 rotate1 = rotate(7.0f, glm::vec3(1, 0, 0));
			// 		allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
			// 		allCubes[i].rotMatrix = rotate1 * allCubes[i].rotMatrix;
			// 		allCubes[i].transMatrix = rotate1 * allCubes[i].transMatrix;
            //         global = true;
                   
			// 	}
            //     updateIndicesAfterGlobalRotation();
			// 	break;
			// case GLFW_KEY_DOWN:
			// 	for (int i = 0; i < CUBE_SIZE; i++)
			// 	{
			// 		const glm::mat4 rotate1 = rotate(-7.0f, glm::vec3(1, 0, 0));
			// 		allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
			// 		allCubes[i].rotMatrix = rotate1 * allCubes[i].rotMatrix;
			// 		allCubes[i].transMatrix = rotate1 * allCubes[i].transMatrix;
            //         global = true;
                    

			// 	}
            //     updateIndicesAfterGlobalRotation();
			// 	break;

			// case GLFW_KEY_RIGHT:
			// 	for (int i = 0; i < CUBE_SIZE; i++)
			// 	{
			// 		const glm::mat4 rotate2 = rotate(7.0f, glm::vec3(0, 1, 0));
			// 		allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
			// 		allCubes[i].rotMatrix = rotate2 * allCubes[i].rotMatrix;
			// 		allCubes[i].transMatrix = rotate2 * allCubes[i].transMatrix;
            //         global = true;

			// 	}
            //     updateIndicesAfterGlobalRotation();
			// 	break;
			// case GLFW_KEY_LEFT:
			// 	for (int i = 0; i < CUBE_SIZE; i++)
			// 	{
			// 		const glm::mat4 rotate2 = rotate(-7.0f, glm::vec3(0, 1, 0));
			// 		allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
			// 		allCubes[i].rotMatrix = rotate2 * allCubes[i].rotMatrix;
			// 		allCubes[i].transMatrix = rotate2 * allCubes[i].transMatrix;
            //         global = true;
                           

			// 	}
            //     updateIndicesAfterGlobalRotation();
			// 	break;
			case GLFW_KEY_R:
				if (action == GLFW_PRESS)
				{
                    global = false;
					rotate_r();
				}
				break;
			case GLFW_KEY_L:
				if (action == GLFW_PRESS)
				{global = false;
					rotate_l();
				}
				break;
			case GLFW_KEY_U:
				if (action == GLFW_PRESS)
				{
global = false;
					rotate_u();
				}
				break;
			case GLFW_KEY_D:
				if (action == GLFW_PRESS)
				{global = false;
					rotate_d();
				}
				break;
			case GLFW_KEY_B:
				if (action == GLFW_PRESS)
				{global = false;
					rotate_b();
				}
				break;
			case GLFW_KEY_F:
				if (action == GLFW_PRESS)
				{global = false;
					rotate_f();
				}
				break;
			case GLFW_KEY_SPACE:
				if (action == GLFW_PRESS)
					clockwise = clockwise * -1;
				break;
			case GLFW_KEY_Z:
				if (action == GLFW_PRESS)
					rotationAngle = rotationAngle / 2;
				break;
			case GLFW_KEY_A:
				if (action == GLFW_PRESS)
					if (rotationAngle <= 90)
					{
						rotationAngle = rotationAngle * 2;
					}
				break;
			// case GLFW_KEY_M:
			// 	if (action == GLFW_PRESS)
			// 	{
			// 		mixer();
			// 	}
			// 	break;
			case GLFW_KEY_S:
				if (action == GLFW_PRESS)
				{
				}
				break;
			case GLFW_KEY_I:

				inside = 1;
				break;

			case GLFW_KEY_O:

				inside = 0;
				break;

			default:
				break;
			}
		}
	}

