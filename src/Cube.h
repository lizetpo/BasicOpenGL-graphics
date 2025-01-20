#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <thread>
#include <chrono>
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
static const int CUBE_SIZE = 27;
static const int CUBE_FACE_SIZE = 9;
bool pickingMode = false; // Declare globally or inside relevant class


int FaceMoving = 0;
float totalAngle = 0.0f;
bool inMovement = false;
int clockwise = 1;
float rotationAngle = 90.0f; // Renamed variable
Cube allCubes[CUBE_SIZE];
int cubesIndex[CUBE_SIZE];
int animation;



inline void normalize_positions() {
    for (int i = 0; i < CUBE_SIZE; i++) {
        glm::vec3 position = glm::vec3(allCubes[i].transMatrix[3]);
        position.x = glm::round(position.x);
        position.y = glm::round(position.y);
        position.z = glm::round(position.z);
        allCubes[i].transMatrix[3] = glm::vec4(position, 1.0f);
    }
}

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

inline void finalize_rotation(){
	for (int i = 0; i < CUBE_SIZE; i++) { 
                allCubes[i].oldRotMatrix = allCubes[i].rotMatrix; // Store final state
    }
}


inline void rotate_cube_side(const int cubes[], const int direction, const float rot_angle) {
	 inMovement = true;
	 animation = 0;

	 switch (direction) {
	 case 1:
		 for (int i = 0; i < CUBE_FACE_SIZE; i++) {
            int cubeIndex = cubesIndex[cubes[i]];
			 const glm::mat4 rotate_face_cube = glm::rotate( glm::radians(rot_angle), glm::vec3(allCubes[cubesIndex[cubes[i]]].transMatrix[0][0], allCubes[cubesIndex[cubes[i]]].transMatrix[0][1], allCubes[cubesIndex[cubes[i]]].transMatrix[0][2]));
		     allCubes[cubesIndex[cubes[i]]].oldRotMatrix = allCubes[cubesIndex[cubes[i]]].rotMatrix;
			 allCubes[cubesIndex[cubes[i]]].rotMatrix = rotate_face_cube*allCubes[cubesIndex[cubes[i]]].rotMatrix;
             normalize_rotation_matrix(allCubes[cubeIndex].rotMatrix);

		 }

		 break;
	 case 2:
		 for (int i = 0; i < CUBE_FACE_SIZE; i++) {
            int cubeIndex = cubesIndex[cubes[i]];
			 glm::mat4 rotate_face_cube = glm::rotate(glm::radians(rot_angle), glm::vec3(allCubes[cubesIndex[cubes[i]]].transMatrix[1][0],
			                                               allCubes[cubesIndex[cubes[i]]].transMatrix[1][1],
			                                               allCubes[cubesIndex[cubes[i]]].transMatrix[1][2]));
			 allCubes[cubesIndex[cubes[i]]].oldRotMatrix = allCubes[cubesIndex[cubes[i]]].rotMatrix;
			 allCubes[cubesIndex[cubes[i]]].rotMatrix = rotate_face_cube * allCubes[cubesIndex[cubes[i]]].rotMatrix;
              normalize_rotation_matrix(allCubes[cubeIndex].rotMatrix);

		 }
		 break;
	 case 3:
		 for (int i = 0; i < CUBE_FACE_SIZE; i++) {
            int cubeIndex = cubesIndex[cubes[i]];
			 glm::mat4 rotate_face_cube =glm::rotate(glm::radians(rot_angle), glm::vec3(allCubes[cubesIndex[cubes[i]]].transMatrix[2][0], allCubes[cubesIndex[cubes[i]]].transMatrix[2][1], allCubes[cubesIndex[cubes[i]]].transMatrix[2][2]));
				 allCubes[cubesIndex[cubes[i]]].oldRotMatrix = allCubes[cubesIndex[cubes[i]]].rotMatrix;
				 allCubes[cubesIndex[cubes[i]]].rotMatrix = rotate_face_cube*allCubes[cubesIndex[cubes[i]]].rotMatrix;
                normalize_rotation_matrix(allCubes[cubeIndex].rotMatrix);
		 }
		 break;
	 default:
		 break;
	 }
         

    
 };



inline bool compare_floats(float A, float B) {
    const float epsilon = 0.01f; // Adjusted epsilon for better precision
    return (fabs(A - B) < epsilon);
}

inline void normalize_index(int indexArray[CUBE_FACE_SIZE], int paramArray[CUBE_FACE_SIZE]) {
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


inline void rotation_checker(int before1[], int after1[], int axis1, int dir1, int face) {

	 if ((FaceMoving == 0 )|| (FaceMoving == face)) {

		 FaceMoving = face;
         std::cout << rotationAngle;
         std::cout << "\n";

		 rotate_cube_side(before1, axis1, dir1*clockwise*rotationAngle);
        
		 totalAngle = totalAngle + clockwise*rotationAngle;
		 if (compare_floats(fabs(totalAngle), 90.0f)) {
			 FaceMoving = 0;
			 totalAngle = 0.0f;
			 normalize_index(before1, after1);
         
		 }
		 else if (compare_floats(fabs(totalAngle), 180.0f)) {
			 FaceMoving = 0;
			 totalAngle = 0.0f;
			 normalize_index(before1, after1);
			 normalize_index(before1, after1); //notsure
		 }
		 else if (totalAngle > 180.0f) {
			 totalAngle = -rotationAngle;
			 rotate_cube_side(before1, 2, dir1*clockwise*-rotationAngle);
        
		 }
	 }
 }


inline void rotate_up() { // BY Y
	int before[9] = { 6,7,8,15,16,17,24,25,26 };
	int after[9] = { 8,17,26,7,16,25,6,15,24 };
	rotation_checker(before, after, 2, -1, 1);
	}

inline void rotate_down() { // BY Y
	int before[9] = { 0,1,2,9,10,11,18,19,20 };
	int after[9] = { 2,11,20,1,10,19,0,9,18 };
	rotation_checker(before, after, 2, -1, 2);

}

inline void rotate_front() { // Rotate around Z-axis (Clockwise)
    int before[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    int after[9] = { 2, 5, 8, 1, 4, 7, 0, 3, 6 }; // Indices for clockwise rotation
    rotation_checker(before, after, 3, -1, 3); // Axis: Z, Direction: Clockwise
}

inline void rotate_back() { // Rotate around Z-axis (Counter-Clockwise)
    int before[9] = { 18, 19, 20, 21, 22, 23, 24, 25, 26 };
    int after[9] = { 20, 23, 26, 19, 22, 25, 18, 21, 24 }; // Indices for counter-clockwise rotation
    rotation_checker(before, after, 3, 1, 4); // Axis: Z, Direction: Counter-Clockwise
}

inline void rotate_right() { // Rotate around X-axis (Clockwise)
    int before[9] = { 2, 11, 20, 5, 14, 23, 8, 17, 26 };
    int after[9] = { 20, 23, 26, 11, 14, 17, 2, 5, 8 }; // Indices for clockwise rotation
    rotation_checker(before, after, 1, -1, 5); // Axis: X, Direction: Clockwise
}

inline void rotate_left() { // Rotate around X-axis (Counter-Clockwise)
    int before[9] = { 18, 9, 0, 21, 12, 3, 24, 15, 6 };
    int after[9] = { 0, 3, 6, 9, 12, 15, 18, 21, 24 }; // Indices for counter-clockwise rotation
    rotation_checker(before, after, 1, 1, 6); // Axis: X, Direction: Counter-Clockwise
}

void randomMixer(int numMoves) {
    std::ofstream mixerFile("mixer.txt");
    if (!mixerFile.is_open()) {
        std::cerr << "Error opening mixer.txt" << std::endl;
        return;
    }

    // Define possible moves and directions
    const char* moves[] = {"U", "D", "F", "B", "L", "R"};
    const char* directions[] = {"", "'"}; // Normal and inverse moves

    // Perform random moves
    for (int i = 0; i < numMoves; i++) {
        int move = rand() % 6;          // Random move index
        int direction = rand() % 2;    // Random direction index

        // Construct move string for logging
        std::string moveStr = moves[move] + std::string(directions[direction]);
        mixerFile << moveStr << " ";   // Log the move

    
        // Perform the move
        if (moveStr == "U") {
            rotate_up();
        } else if (moveStr == "D") {
            rotate_down();
        } else if (moveStr == "F") {
            rotate_front();
        } else if (moveStr == "B") {
            rotate_back();
        } else if (moveStr == "L") {
            rotate_left();
        } else if (moveStr == "R") {
            rotate_right();
        }

       
    }

    mixerFile.close();
}


inline void initializeIndicesOnly()
{
    for (int i = 0; i < CUBE_SIZE; i++)
    {
        cubesIndex[i] = i; // Reset the indices to their default order
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
			case GLFW_KEY_UP:
			
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate1 = glm::rotate(glm::radians(12.0f), glm::vec3(1, 0, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate1 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate1 * allCubes[i].transMatrix;
					 
				}
				break;
			case GLFW_KEY_DOWN:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate1 = glm::rotate(glm::radians(-12.0f), glm::vec3(1, 0, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate1 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate1 * allCubes[i].transMatrix; 
				}
				break;

			case GLFW_KEY_RIGHT:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate2 =glm::rotate(glm::radians(12.0f), glm::vec3(0, 1, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate2 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate2 * allCubes[i].transMatrix; 
				}
				break;
			case GLFW_KEY_LEFT:
				for (int i = 0; i < CUBE_SIZE; i++)
				{
					const glm::mat4 rotate2 = glm::rotate(glm::radians(-12.0f),glm:: vec3(0, 1, 0));
					allCubes[i].oldRotMatrix = allCubes[i].rotMatrix;
					allCubes[i].rotMatrix = rotate2 * allCubes[i].rotMatrix;
					allCubes[i].transMatrix = rotate2 * allCubes[i].transMatrix; 
				}
				break;
			case GLFW_KEY_R:
				if (action == GLFW_PRESS)
				{
					rotate_right();
				}
				break;
			case GLFW_KEY_L:
				if (action == GLFW_PRESS)
				{
					rotate_left();
				}
				break;
			case GLFW_KEY_U:
				if (action == GLFW_PRESS)
				{
					rotate_up();
				}
				break;
			case GLFW_KEY_D:
				if (action == GLFW_PRESS)
				{
					rotate_down();
				}
				break;
			case GLFW_KEY_B:
				if (action == GLFW_PRESS)
				{
					rotate_back();
				}
				break;
			case GLFW_KEY_F:
				if (action == GLFW_PRESS)
				{
					rotate_front();
				}
				break;
			case GLFW_KEY_M:
				if (action == GLFW_PRESS)
					{
						randomMixer(20);
					}
					break;
			case GLFW_KEY_SPACE:
				if (action == GLFW_PRESS)
					clockwise = clockwise * -1;
				break;
			case GLFW_KEY_Z:
				if (action == GLFW_PRESS)
					if(rotationAngle> 45){
						rotationAngle = rotationAngle / 2;
					}
					
				break;
			case GLFW_KEY_P:
				if (action == GLFW_PRESS)
					pickingMode = !pickingMode; 
				break;
			case GLFW_KEY_A:
				if (action == GLFW_PRESS)
					if (rotationAngle <= 90)
					{
						rotationAngle = rotationAngle * 2;
					}
				break;
	
		

			default:
				break;
			}
		}
       
	}