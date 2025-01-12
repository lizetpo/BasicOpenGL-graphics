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
glm::mat4 globalRotationMatrix = glm::mat4(1.0f);


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
glm::mat4 globalRotation = glm::mat4(1.0f); // Global rotation matrix for the entire cube


// inline void normalize_rotation_matrix(glm::mat4& matrix) {
//     glm::vec3 xAxis = glm::normalize(glm::vec3(matrix[0]));
//     glm::vec3 yAxis = glm::normalize(glm::vec3(matrix[1]));
//     glm::vec3 zAxis = glm::normalize(glm::vec3(matrix[2]));

//     yAxis = glm::normalize(glm::cross(zAxis, xAxis));
//     zAxis = glm::normalize(glm::cross(xAxis, yAxis));

//     matrix[0] = glm::vec4(xAxis, 0.0f);
//     matrix[1] = glm::vec4(yAxis, 0.0f);
//     matrix[2] = glm::vec4(zAxis, 0.0f);
// }

// inline void rotate_face(const int cubes[], const glm::vec3& localAxis, const float rot_angle) {
//     inMovement = true;
//     animation = 0;

//     // Transform the local axis using the global rotation matrix

//     glm::mat4 rotate_face_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(rot_angle), localAxis);

//      for (int i = 0; i < CUBE_FACE_SIZE; i++) {
//          int cubeIndex = cubesIndex[cubes[i]];

//         // Update the oldRotMatrix to store the current rotMatrix before applying the new rotation
		
//         allCubes[cubeIndex].oldRotMatrix = allCubes[cubeIndex].rotMatrix;

//         // Apply the new rotation based on the previous rotation
//         allCubes[cubeIndex].rotMatrix = rotate_face_matrix*allCubes[cubeIndex].rotMatrix ;

//         // Normalize the matrix to avoid precision errors over time
//         normalize_rotation_matrix(allCubes[cubeIndex].rotMatrix);
//     }
// }


inline void rotate_face(const int cubes[], const int direction, const float rot_angle) {
	 inMovement = true;
	 animation = 0;
	 switch (direction) {
	 case 1:
		 for (int i = 0; i < CUBE_FACE_SIZE; i++) {
			 const glm::mat4 rotate_face_cube = glm::rotate(rot_angle, glm::vec3(allCubes[cubesIndex[cubes[i]]].transMatrix[0][0], allCubes[cubesIndex[cubes[i]]].transMatrix[0][1], allCubes[cubesIndex[cubes[i]]].transMatrix[0][2]));
		     allCubes[cubesIndex[cubes[i]]].oldRotMatrix = allCubes[cubesIndex[cubes[i]]].rotMatrix;
			 allCubes[cubesIndex[cubes[i]]].rotMatrix = rotate_face_cube*allCubes[cubesIndex[cubes[i]]].rotMatrix;
		 }

		 break;
	 case 2:
		 for (int i = 0; i < CUBE_FACE_SIZE; i++) {
			 glm::mat4 rotate_face_cube = glm::rotate(rot_angle, glm::vec3(allCubes[cubesIndex[cubes[i]]].transMatrix[1][0],
			                                               allCubes[cubesIndex[cubes[i]]].transMatrix[1][1],
			                                               allCubes[cubesIndex[cubes[i]]].transMatrix[1][2]));
			 allCubes[cubesIndex[cubes[i]]].oldRotMatrix = allCubes[cubesIndex[cubes[i]]].rotMatrix;
			 allCubes[cubesIndex[cubes[i]]].rotMatrix = rotate_face_cube * allCubes[cubesIndex[cubes[i]]].rotMatrix;
		 }
		 break;
	 case 3:
		 for (int i = 0; i < CUBE_FACE_SIZE; i++) {
			 glm::mat4 rotate_face_cube =glm::rotate(rot_angle, glm::vec3(allCubes[cubesIndex[cubes[i]]].transMatrix[2][0], allCubes[cubesIndex[cubes[i]]].transMatrix[2][1], allCubes[cubesIndex[cubes[i]]].transMatrix[2][2]));
				 allCubes[cubesIndex[cubes[i]]].oldRotMatrix = allCubes[cubesIndex[cubes[i]]].rotMatrix;
				 allCubes[cubesIndex[cubes[i]]].rotMatrix = rotate_face_cube*allCubes[cubesIndex[cubes[i]]].rotMatrix;
		 }
		 break;
	 default:
		 break;
	 }
 };

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

// inline void rotation_checker(int before[], int after[], const glm::vec3& localAxis, int dir, int face) {
//     if ((FaceMoving == 0) || (FaceMoving == face)) {
// 		if(!inMovement){
//  			FaceMoving = face;
// 		}
//          // Transform the local axis using the global rotation matrix
//         //glm::vec3 transformedAxis = glm::vec3(globalRotationMatrix * glm::vec4(localAxis, 0.0f));

//         // Rotate the selected face
//         rotate_face(before, localAxis, dir * clockwise * rotationAngle);

//         totalAngle += clockwise * rotationAngle;

//         if (compare_floats(fabs(totalAngle), 90.0f)) {
//             totalAngle = 0.0f;
//             FaceMoving = 0;
//             change_array_index(before, after);
//             for (int i = 0; i < CUBE_FACE_SIZE; i++) {
//                 normalize_rotation_matrix(allCubes[cubesIndex[before[i]]].rotMatrix);
//             }
//             inMovement = false;
//         }
// 		else{
// 			inMovement = true;
// 		}
//     }
// }



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
// inline void rotate_u() { // Rotate around Y-axis (Up face)
//     switch (size) {
//         case 2: {
//             int before[4] = {2, 3, 6, 7};
//             int after[4] = {3, 7, 2, 6};
//             rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 1);
//         } break;
//         case 3: {
//             int before[9] = {6, 7, 8, 15, 16, 17, 24, 25, 26};
//             int after[9] = {8, 17, 26, 7, 16, 25, 6, 15, 24};
//             rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 1);
//         } break;
//         default:
//             break;
//     }
// }

// inline void rotate_d() { // Rotate around Y-axis (Down face)
//     switch (size) {
//         case 2: {
//             int before[4] = {0, 1, 4, 5};
//             int after[4] = {1, 5, 0, 4};
//             rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 2);
//         } break;
//         case 3: {
//             int before[9] = {0, 1, 2, 9, 10, 11, 18, 19, 20};
//             int after[9] = {2, 11, 20, 1, 10, 19, 0, 9, 18};
//             rotation_checker(before, after, glm::vec3(0, 1, 0), -1, 2);
//         } break;
//         default:
//             break;
//     }
// }

// inline void rotate_f() { // Rotate around Z-axis (Front face)
//     switch (size) {
//         case 2: {
//             int before[4] = {0, 1, 2, 3};
//             int after[4] = {1, 3, 0, 2};
//             rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 3);
//         } break;
//         case 3: {
//             int before[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
//             int after[9] = {2, 5, 8, 1, 4, 7, 0, 3, 6};
//             rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 3);
//         } break;
//         default:
//             break;
//     }
// }

// inline void rotate_b() { // Rotate around Z-axis (Back face)
//     switch (size) {
//         case 2: {
//             int before[4] = {4, 5, 6, 7};
//             int after[4] = {5, 7, 4, 6};
//             rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 4);
//         } break;
//         case 3: {
//             int before[9] = {18, 19, 20, 21, 22, 23, 24, 25, 26};
//             int after[9] = {20, 23, 26, 19, 22, 25, 18, 21, 24};
//             rotation_checker(before, after, glm::vec3(0, 0, 1), 1, 4);
//         } break;
//         default:
//             break;
//     }
// }

// inline void rotate_r() { // Rotate around X-axis (Right face)
//     switch (size) {
//         case 2: {
//             int before[4] = {1, 5, 3, 7};
//             int after[4] = {5, 7, 1, 3};
//             rotation_checker(before, after, glm::vec3(1, 0, 0), 1, 5);
//         } break;
//         case 3: {
//             int before[9] = {18, 9, 0, 21, 12, 3, 24, 15, 6};
//             int after[9] = {0, 3, 6, 9, 12, 15, 18, 21, 24};
//             rotation_checker(before, after, glm::vec3(1, 0, 0), 1, 6);
//         } break;
//         default:
//             break;
//     }
// }

// inline void rotate_l() { // Rotate around X-axis (Left face)
//     switch (size) {
//         case 2: {
//             int before[4] = {4, 0, 6, 2};
//             int after[4] = {0, 2, 4, 6};
//             rotation_checker(before, after, glm::vec3(1, 0, 0), -1, 6);
//         } break;
//         case 3: {
           
//              int before[9] = {2, 11, 20, 5, 14, 23, 8, 17, 26};
//             int after[9] = {20, 23, 26, 11, 14, 17, 2, 5, 8};
//             rotation_checker(before, after, glm::vec3(1, 0, 0), -1, 5);
//         } break;
//         default:
//             break;
//     }
// }

// void updateIndicesAfterGlobalRotation() {
//     // Temporary array to store the new indices
//     int newCubesIndex[CUBE_SIZE];

//     // Loop through each cube
//     for (int i = 0; i < CUBE_SIZE; i++) {
//         // Get the original 3D position of the cube
//         int x = i % size;                 // X-coordinate
//         int y = (i / size) % size;        // Y-coordinate
//         int z = i / (size * size);        // Z-coordinate

//         // Center the cube positions for rotation
//         glm::vec3 originalPos = glm::vec3(x, y, z) - glm::vec3(size - 1) * 0.5f;

//         // Apply the global rotation to the position
//         glm::vec3 rotatedPos = glm::vec3(globalRotationMatrix * glm::vec4(originalPos, 1.0f));

//         // Snap the rotated positions to discrete values: -1, 0, or 1
//         rotatedPos.x = glm::round(rotatedPos.x);
//         rotatedPos.y = glm::round(rotatedPos.y);
//         rotatedPos.z = glm::round(rotatedPos.z);

//         // Ensure the values are clamped within the range [-1, 1]
//         // rotatedPos = glm::clamp(rotatedPos, glm::vec3(-1.0f), glm::vec3(1.0f));
        

//         // Recalculate the new grid position
//         int newX = static_cast<int>(rotatedPos.x + 0.5f * (size - 1));
//         int newY = static_cast<int>(rotatedPos.y + 0.5f * (size - 1));
//         int newZ = static_cast<int>(rotatedPos.z + 0.5f * (size - 1));

//         // Compute the new index in the 1D array
//         int newIndex = newZ * size * size + newY * size + newX;

//         // Update the newCubesIndex array
//         newCubesIndex[newIndex] = cubesIndex[i];
//     }

//     // Copy the updated indices back to cubesIndex
//     for (int i = 0; i < CUBE_SIZE; i++) {
//         cubesIndex[i] = newCubesIndex[i];
//     }

//     // Print the state for debugging
//     print_state();
// }
// inline void help(glm::mat4 rotationMatrix){
//     globalRotationMatrix = rotationMatrix * globalRotationMatrix;

//         for (int i = 0; i < CUBE_SIZE; i++) {
//             allCubes[i].rotMatrix = rotationMatrix * allCubes[i].rotMatrix;
//             allCubes[i].transMatrix = rotationMatrix * allCubes[i].transMatrix;

//             // Snap positions to -1, 0, or 1
//             glm::vec3 position = glm::vec3(allCubes[i].transMatrix[3]);
//             position.x = glm::round(position.x);
//             position.y = glm::round(position.y);
//             position.z = glm::round(position.z);

//             allCubes[i].transMatrix[3] = glm::vec4(position, 1.0f);
//         }


//         // Update indices and snap positions
//         updateIndicesAfterGlobalRotation();
           
// }

// inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// 	{
// 		        glm::mat4 rotationMatrix = glm::mat4(1.0f);

// 		if (!inMovement)
// 		{
// 			switch (key)
// 			{
// 			case GLFW_KEY_ESCAPE:
// 				if (action == GLFW_PRESS)
// 					glfwSetWindowShouldClose(window, GLFW_TRUE);
// 				break;
//             global = true;
//             case GLFW_KEY_UP:
//                 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//                 help(rotationMatrix);
//                 break;
//             case GLFW_KEY_DOWN:
//                 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//                                 help(rotationMatrix);

//                 break;
//             case GLFW_KEY_LEFT:
//                 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//                                 help(rotationMatrix);

//                 break;
//             case GLFW_KEY_RIGHT:
//                 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//                                 help(rotationMatrix);

//                 break;
// 			case GLFW_KEY_R:
// 				if (action == GLFW_PRESS)
// 				{
//                     global = false;
// 					rotate_r();
// 				}
// 				break;
// 			case GLFW_KEY_L:
// 				if (action == GLFW_PRESS)
// 				{global = false;
// 					rotate_l();
// 				}
// 				break;
// 			case GLFW_KEY_U:
// 				if (action == GLFW_PRESS)
// 				{
// global = false;
// 					rotate_u();
// 				}
// 				break;
// 			case GLFW_KEY_D:
// 				if (action == GLFW_PRESS)
// 				{global = false;
// 					rotate_d();
// 				}
// 				break;
// 			case GLFW_KEY_B:
// 				if (action == GLFW_PRESS)
// 				{global = false;
// 					rotate_b();
// 				}
// 				break;
// 			case GLFW_KEY_F:
// 				if (action == GLFW_PRESS)
// 				{global = false;
// 					rotate_f();
// 				}
// 				break;
// 			case GLFW_KEY_SPACE:
// 				if (action == GLFW_PRESS)
// 					clockwise = clockwise * -1;
// 				break;
// 			case GLFW_KEY_Z:
// 				if (action == GLFW_PRESS)
// 					rotationAngle = rotationAngle / 2;
// 				break;
// 			case GLFW_KEY_A:
// 				if (action == GLFW_PRESS)
// 					if (rotationAngle <= 90)
// 					{
// 						rotationAngle = rotationAngle * 2;
// 					}
// 				break;
// 			// case GLFW_KEY_M:
// 			// 	if (action == GLFW_PRESS)
// 			// 	{
// 			// 		mixer();
// 			// 	}
// 			// 	break;
// 			case GLFW_KEY_S:
// 				if (action == GLFW_PRESS)
// 				{
// 				}
// 				break;
// 			case GLFW_KEY_I:

// 				inside = 1;
// 				break;

// 			case GLFW_KEY_O:

// 				inside = 0;
// 				break;

// 			default:
// 				break;
// 			}

        
// 		}
// 	}

inline void rotation_checker(int before1[], int after1[], int axis1, int dir1, int face) {

	 if ((FaceMoving == 0 )|| (FaceMoving == face)) {

		 FaceMoving = face;
		 rotate_face(before1, axis1, dir1*clockwise*rotationAngle);

		 totalAngle = totalAngle + clockwise*rotationAngle;
		 if (compare_floats(fabs(totalAngle), 90.0f)) {
			 FaceMoving = 0;
			 totalAngle = 0.0f;
			 change_array_index(before1, after1);
		 }
		 else if (compare_floats(fabs(totalAngle), 180.0f)) {
			 FaceMoving = 0;
			 totalAngle = 0.0f;
			 change_array_index(before1, after1);
			 change_array_index(before1, after1);
		 }
		 else if (totalAngle > 180.0f) {
			 totalAngle = -rotationAngle;
			 rotate_face(before1, 2, dir1*clockwise*-rotationAngle);
		 }
	 }
 }


inline void rotate_u() { // BY Y
	switch (size) {
	case 2: {
		int before[4] = { 2,3,6,7 };
		int after[4] = { 3,7,2,6 };
		rotation_checker(before, after, 2, -1, 1);
	}
			break;
	case 3: {
		int before[9] = { 6,7,8,15,16,17,24,25,26 };
		int after[9] = { 8,17,26,7,16,25,6,15,24 };
		rotation_checker(before, after, 2, -1, 1);
	}
			break;
	case 4: {
		if (inside == 0) {
			int before[16] = {12,13,14,15,28,29,30,31,44,45,46,47,60,61,62,63 };
			int after[16] = {15,31,47,63,14,30,46,62,13,29,45,61,12,28,44,60 };
			rotation_checker(before, after, 2, -1, 1);
		}
		else {
			int before[16] = {8,9,10,11,24,25,26,27,40,41,42,43,56,57,58,59 };
			int after[16] = {11,27,43,59,10,26,42,58,9,25,41,57,8,24,40,56 };
			rotation_checker(before, after, 2, -1, 1);
		}
	}
			break;
	case 5: {
		if (inside == 0) {
			int before[25] = {20,21,22,23,24,45,46,47,48,49,70,71,72,73,74,95,96,97,98,99,120,121,122,123,124 };
			int after[25] = {24,49,74,99,124,23,48,73,98,123,22,47,72,97,122,21,46,71,96,121,20,45,70,95,120 };
			rotation_checker(before, after, 2, -1, 1);
		}
		else {
			int before[25] = {15,16,17,18,19,40,41,42,43,44,65,66,67,68,69,90,91,92,93,94,115,116,117,118,119 };
			int after[25] = {19,44,69,94,119,18,43,68,93,118,17,42,67,92,117,16,41,66,91,116,15,40,65,90,115 };
			rotation_checker(before, after, 2, -1, 1);
		}
	}
			break;
	default:
		break;
	}
}

inline void rotate_d() { // BY Y
	switch (size) {
	case 2: {
		int before[4] = { 0,1,4,5 };
		int after[4] = { 1,5,0,4 };
		rotation_checker(before, after, 2, -1, 2);
	}
			break;
	case 3: {
		int before[9] = { 0,1,2,9,10,11,18,19,20 };
		int after[9] = { 2,11,20,1,10,19,0,9,18 };
		rotation_checker(before, after, 2, -1, 2);
	}
			break;
	case 4: {
		if (inside == 0) {
			int before[16] = {0,1,2,3,16,17,18,19,32,33,34,35,48,49,50,51 };
			int after[16] = {3,19,35,51,2,18,34,50,1,17,33,49,0,16,32,48 };
			rotation_checker(before, after, 2, -1, 2);
		}
		else {
			int before[16] = {4,5,6,7,20,21,22,23,36,37,38,39,52,53,54,55 };
			int after[16] = {7,23,39,55,6,22,38,54,5,21,37,53,4,20,36,52 };
			rotation_checker(before, after, 2, -1, 2);
		}
	}
			break;
	case 5: {
		if (inside == 0) {
			int before[25] = {0,1,2,3,4,25,26,27,28,29,50,51,52,53,54,75,76,77,78,79,100,101,102,103,104 };
			int after[25] = {4,29,54,79,104,3,28,53,78,103,2,27,52,77,102,1,26,51,76,101,0,25,50,75,100 };
			rotation_checker(before, after, 2, -1, 2);
		}
		else {
			int before[25] = {5,6,7,8,9,30,31,32,33,34,55,56,57,58,59,80,81,82,83,84,105,106,107,108,109 };
			int after[25] = {9,34,59,84,109,8,33,58,83,108,7,32,57,82,107,6,31,56,81,106,5,30,55,80,105 };
			rotation_checker(before, after, 2, -1, 2);
		}
	}
			break;
	default:
		break;

	}
}

inline void rotate_f() { // BY Z
	switch (size) {
	case 2: {
		int before[4] = { 0,1,2,3 };
		int after[4] = { 1,3,0,2 };
		rotation_checker(before, after, 3, 1, 3);
	}
			break;
	case 3: {
		int before[9] = { 0,1,2,3,4,5,6,7,8 };
		int after[9] = { 2,5,8,1,4,7,0,3,6 };
		rotation_checker(before, after, 3, 1, 3);
	}
			break;
	case 4: {
		if (inside == 0) {
			int before[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
			int after[16] = {3,7,11,15,2,6,10,14,1,5,9,13,0,4,8,12 };
			rotation_checker(before, after, 3, 1, 3);
		}
		else {
			int before[16] = {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 };
			int after[16] = {19,23,27,31,18,22,26,30,17,21,25,29,16,20,24,28 };
			rotation_checker(before, after, 3, 1, 3);
		}
	}
			break;
	case 5: {
		if (inside == 0) {
			int before[25] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
			int after[25] = {4,9,14,19,24,3,8,13,18,23,2,7,12,17,22,1,6,11,16,21,0,5,10,15,20 };
			rotation_checker(before, after, 3, 1, 3);
		}
		else {
			int before[25] = {25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,	48,49 };
			int after[25] = {29,34,39,44,49,28,33,38,43,48,27,32,37,42,47,26,31,36,41,46,25,30,35,40,45 };
			rotation_checker(before, after, 3, 1, 3);
		}
	}
			break;
	default:
		break;
	}

}

inline void rotate_b() { // BY Z
	switch (size) {
	case 2: {
		int before[4] = { 4,5,6,7 };
		int after[4] = { 5,7,4,6 };
		rotation_checker(before, after, 3, 1, 4);
	}
			break;
	case 3: {
		int before[9] = { 18,19,20,21,22,23,24,25,26 };
		int after[9] = { 20,23,26,19,22,25,18,21,24 };
		rotation_checker(before, after, 3, 1, 4);
	}

			break;

	case 4: {
		if (inside == 0) {
			int before[16] = {48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63 };
			int after[16] = {51,55,59,63,50,54,58,62,49,53,57,61,48,52,56,60 };
			rotation_checker(before, after, 3, 1, 4);
		}
		else {
			int before[16] = {32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 };
			int after[16] = {35,39,43,47,34,38,42,46,33,37,41,45,32,36,40,44 };
			rotation_checker(before, after, 3, 1, 4);
		}
	}
			break;
	case 5: {
		if (inside == 0) {
			int before[25] = {100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124 };
			int after[25] = {104,109,114,119,124,103,108,113,118,123,102,107,112,117,122,101,106,111,116,121,100,105,110,115,120 };
			rotation_checker(before, after, 3, 1, 4);
		}
		else {
			int before[25] = {75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99 };
			int after[25] = {79,84,89,94,99,78,83,88,93,98,77,82,87,92,97,76,81,86,91,96,75,80,85,90,95 };
			rotation_checker(before, after, 3, 1, 4);
		}
	}
			break;
	default:
		break;
	}
}

inline void rotate_r() { // BY X
	switch (size) {

	case 2: {
		int before[4] = { 1,5,3,7 };
		int after[4] = { 5,7,1,3 };
		rotation_checker(before, after, 1, 1, 5);
	}
			break;
	case 3: {
		int before[9] = { 2,11,20,5,14,23,8,17,26 };
		int after[9] = { 20,23,26,11,14,17,2,5,8 };
		rotation_checker(before, after, 1, 1, 5);
	}
			break;
	case 4: {
		if (inside == 0) {
			int before[16] = {3,19,35,51,7,23,39,55,11,27,43,59,15,31,47,63 };
			int after[16] = {51,55,59,63,35,39,43,47,19,23,27,31,3,7,11,15 };
			rotation_checker(before, after, 1, 1, 5);
		}
		else {
			int before[16] = {2,18,34,50,6,22,38,54,10,26,42,58,14,30,46,62 };
			int after[16] = {50,54,58,62,34,38,42,46,18,22,26,30,2,6,10,14 };
			rotation_checker(before, after, 1, 1, 5);
		}
	}
			break;
	case 5: {
		if (inside == 0) {
			int before[25] = {4,29,54,79,104,9,34,59,84,109,14,39,64,89,114,19,44,69,94,119,24,49,74,99,124 };
			int after[25] = {104,109,114,119,124,79,84,89,94,99,54,59,64,69,74,29,34,39,44,49,4,9,14,19,24 };
			rotation_checker(before, after, 1, 1, 5);
		}
		else {
			int before[25] = {3,28,53,78,103,8,33,58,83,108,13,38,63,88,113,18,43,68,93,118,23,48,73,98,123 };
			int after[25] = {103,108,113,118,123,78,83,88,93,98,53,58,63,68,73,28,33,38,43,48,3,8,13,18,23 };
			rotation_checker(before, after, 1, 1, 5);
		}
	}
			break;
	default:
		break;
	}
}

inline void rotate_l() { // BY X
	switch (size) {
	case 2: {
		int before[4] = { 4,0,6,2 };
		int after[4] = { 0,2,4,6 };
		rotation_checker(before, after, 1, -1, 6);
	}
			break;
	case 3: {
		int before[9] = { 18,9,0,21,12,3,24,15,6 };
		int after[9] = { 0,3,6,9,12,15,18,21,24 };
		rotation_checker(before, after, 1, -1, 6);
	}

			break;

	case 4: {
		if (inside == 0) {
			int before[16] = {48,32,16,0,52,36,20,4,56,40,24,8,60,44,28,12 };
			int after[16] = {0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60 };
			rotation_checker(before, after, 1, -1, 6);
		}
		else {
			int before[16] = {49,33,17,1,53,37,21,5,57,41,25,9,61,45,29,13 };
			int after[16] = {1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61 };
			rotation_checker(before, after, 1, -1, 6);
		}
	}
			break;
	case 5: {
		if (inside == 0) {
			int before[25] = {100,75,50,25,0,105,80,55,30,5,110,85,60,35,10,115,90,65,40,15,120,95,70,45,20 };
			int after[25] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120 };
			rotation_checker(before, after, 1, -1, 6);
		}
		else {
			int before[25] = {101,76,51,26,1,106,81,56,31,6,111,86,61,36,11,116,91,66,41,16,121,96,71,46,21 };
			int after[25] = {1,6,11,16,21,26,31,36,41,46,51,56,61,66,71,76,81,86,91,96,101,106,111,116,121 };
			rotation_checker(before, after, 1, -1, 6);
		}
	}
			break;
	default:
		break;
	}
}

inline void mixer()
{
	
	char func_name[6] = {'U','D','R','L','F','B'};
	for (auto i=0;i<10;i++){
	const auto function = rand() % 6; // random 0-5
	std::cout << func_name[function] << " ";
	std::cout << "\n";
	switch (function)
		{
		case 0:
			rotate_u();
			break;
		case 1:
			rotate_d();
			break;
		case 2:
			rotate_r();
			break;
		case 3:
			rotate_l();
			break;
		case 4:
			rotate_f();
			break;
		case 5:
			rotate_b();
			break;
		default:
			break;
		}
	}
}

inline void solver() { // empty function
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
			case GLFW_KEY_UP:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate1 = glm::rotate(7.0f, glm::vec3(1, 0, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate1 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate1 * allCubes[i].transMatrix;
				}
				break;
			case GLFW_KEY_DOWN:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate1 = glm::rotate(-7.0f, glm::vec3(1, 0, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate1 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate1 * allCubes[i].transMatrix;
				}
				break;

			case GLFW_KEY_RIGHT:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate2 =glm::rotate(7.0f, glm::vec3(0, 1, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate2 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate2 * allCubes[i].transMatrix;
				}
				break;
			case GLFW_KEY_LEFT:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate2 = glm::rotate(-7.0f,glm:: vec3(0, 1, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate2 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate2 * allCubes[i].transMatrix;
				}
				break;
			case GLFW_KEY_R:
				if (action == GLFW_PRESS)
				{
					rotate_r();
				}
				break;
			case GLFW_KEY_L:
				if (action == GLFW_PRESS)
				{
					rotate_l();
				}
				break;
			case GLFW_KEY_U:
				if (action == GLFW_PRESS)
				{
					rotate_u();
				}
				break;
			case GLFW_KEY_D:
				if (action == GLFW_PRESS)
				{
					rotate_d();
				}
				break;
			case GLFW_KEY_B:
				if (action == GLFW_PRESS)
				{
					rotate_b();
				}
				break;
			case GLFW_KEY_F:
				if (action == GLFW_PRESS)
				{
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
			case GLFW_KEY_M:
				if (action == GLFW_PRESS)
				{
					mixer();
				}
				break;
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