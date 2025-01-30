#include "CubeHandler.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

CubeHandler::CubeHandler(Shader* shader, Texture* texture, VertexArray* vertexArray) 
    : CubeTranslationMatrix(glm::mat4(1.0f)),
      CubeRotationMatrix(glm::mat4(1.0f)),
      CubeScaleMatrix(glm::mat4(0.5f)),
      ActiveRotations(-1)
{
    int cube_id = 0;
    
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            for(int k = 0; k < 3; ++k) {
                glm::vec3 position(2.0f * (k - 3 / 2), 2.0f * (j - 3 / 2), 2.0f * (i - 3 / 2));
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f));
                Cubes[cube_id] = new Cube(cube_id, shader, texture, translation, vertexArray);
                CubePositions[i][j][k] = cube_id++;
            }
        }
    }
    
    totalRotation.resize(3 * 3, 0);
    for(int d = 0; d < 3; ++d) {
        Indices.push_back(d * 3);
        Indices.push_back(d * 3 + 3 - 1);
    }
}

std::vector<int> CubeHandler::GetWall(int axis, int layer) {
    
    std::vector<int> wallIndices;
    wallIndices.reserve(3 * 3);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            wallIndices.push_back(
                (axis == 0) ? CubePositions[i][j][layer] :
                (axis == 1) ? CubePositions[layer][i][j] :
                              CubePositions[j][layer][i]
            );
        }
    }
    return wallIndices;
}

bool CubeHandler::CanRotate(int axis) {
    
    for(int d = 0; d < 3; ++d) {
        if(d == axis) continue;
        for(int i = 0; i < 3; ++i) {
            if(totalRotation[d * 3 + i] % 2 == 1) return false;
        }
    }
    return true;
}

void CubeHandler::ApplyRotation(int wall) {
    if (ActiveRotations != -1) {
        // Get affected cube indices
        std::vector<int> affectedCubes = GetWall(GetAxis(wall), GetLayer(wall));

        // Increment rotation angle
        UpdateRotationAngle();

        // Get rotation axis based on wall index
        glm::vec3 rotationAxis = GetRotationAxis(wall);

        // Apply rotation to each cube in the affected layer
        RotateCubes(affectedCubes, rotationAxis);

        // Check if the rotation is complete
        if (RotationTreshold()) {
            ResetRotation(wall);
        }
    }

    // Start the next queued rotation (if any)
    ExecutePendingRotation();
}


void CubeHandler::UpdateCubePositions(int axis, int layer) {

    auto getIndex = [&](int i, int j) -> int& {
        switch (axis) {
            case 0: return CubePositions[i][j][layer]; // X-axis
            case 1: return CubePositions[layer][i][j]; // Y-axis
            case 2: return CubePositions[j][layer][i]; // Z-axis
            default: throw std::runtime_error("Invalid axis in UpdateCubePositions");
        }
    };

    // **In-place rotation using 4-way swaps**
    for (int i = 0; i < 3 / 2; ++i) {
        for (int j = i; j < 2 - i; ++j) {
            int currentIndex = getIndex(i, j);

            if (Clockwise == 1) {
                getIndex(i, j) = getIndex(2 - j, i);
                getIndex(2 - j, i) = getIndex(2 - i, 2 - j);
                getIndex(2 - i, 2 - j) = getIndex(j, 2 - i);
                getIndex(j, 2 - i) = currentIndex;
            } else {
                getIndex(i, j) = getIndex(j, 2 - i);
                getIndex(j, 2 - i) = getIndex(2 - i, 2 - j);
                getIndex(2 - i, 2 - j) = getIndex(2 - j, i);
                getIndex(2 - j, i) = currentIndex;
            }
        }
    }
}



void CubeHandler::QueueWallRotation(int wall)
{
        wall = Indices[wall];
        
        switch (Angle)
        {
        case 4:
            PendingRotations.push(wall);    
            PendingRotations.push(wall);        
            PendingRotations.push(wall);        
            PendingRotations.push(wall);        

            break;  
        case 3:
            PendingRotations.push(wall);    
            PendingRotations.push(wall);        
            PendingRotations.push(wall);        

            break;            
        case 2:
            PendingRotations.push(wall);    
            PendingRotations.push(wall);        

            break;
        case 1:
            PendingRotations.push(wall);    

        default:
            break;
        }
}



// void CubeHandler::Mix()
// {
//     std::vector<int> wall_indices({0, 1, 2, 3, 4, 5});
//     std::vector<std::string> wall_names({"L", "R", "D", "U", "F", "B"});

//     std::ofstream file("mixer.txt");
//     if (!file.is_open())
//     {
//         std::cerr << "Error: Unable to open output file.\n";
//         return;
//     }


//     for(int i = 0; i < 30; i++)
//     {
//         int wall_i = std::rand() % wall_indices.size();
//         RotateWall(wall_indices[wall_i]);

//         file << "(" << wall_names[wall_indices[wall_i]] << ")\n";
//     }

//     file.close();
// }

void CubeHandler::SetClockWise()
{
    if(ActiveRotations == -1){
        if(Clockwise == 1)
        {
            Clockwise = -1;
        }else{
            Clockwise = 1;
        }
    }
}

void CubeHandler::Render(glm::mat4 view, glm::mat4 proj)
{
    
    // Change to "if" to enable motion
    if (ActiveRotations != -1 || !PendingRotations.empty()) 
    {
        ApplyRotation(ActiveRotations);
    }

    glm::mat4 model = CubeRotationMatrix * CubeTranslationMatrix * CubeScaleMatrix;

    glm::mat4 mvp = proj * view * model;

    for (Cube* c : Cubes) 
    {
        c->render(mvp);
    }


}

void CubeHandler::RotationCube(int index, float angle, glm::vec3 axis)
{
    glm::mat3 inverse = glm::transpose(CubeRotationMatrix);
    Cubes[index]->rotateWorld(angle, inverse * axis);
}

void CubeHandler::Restart()
{
    CubeTranslationMatrix = glm::mat4(1.0f);
    CubeScaleMatrix = glm::mat4(1.0f);
    ActiveRotations = -1;
    
    int z = -1;
    int index = 0;

    for(int i = 0; i<3; i++)
    {
        int y = -1;

        for(int j = 0; j<3; j++)
        {
            int x = -1;

            for(int k = 0; k<3; k++)
            {
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * x, 2.0f * y, 2.0f * z) + glm::vec3(0.0f));  
                Cubes[index]->transformCube(translation, glm::mat4(1.0f));
                CubePositions[i][j][k] = index;
                
                x++;
                index++;
            }
            y++;
        }
        z++;
    }  
}

void CubeHandler::ZBuffer(glm::mat4 view, glm::mat4 proj)
{
    glm::mat4 model = CubeRotationMatrix * CubeTranslationMatrix * CubeScaleMatrix;
    glm::mat4 mvp = proj * view * model;

    for (Cube* c : Cubes) 
    {
        c->z_buffer(mvp);
    }

}

glm::vec3 CubeHandler::GetCubePosition(int cubeID) {
    if (cubeID < 0 || cubeID >= 27) return glm::vec3(0.0f);

    // Extract position from transformation matrix
    return glm::vec3(Cubes[cubeID]->GetTransform()[3]); 
}

void CubeHandler::RotateLeft()  { if(CanRotate(0)){QueueWallRotation(0); }}
void CubeHandler::RotateRight() { if(CanRotate(1/3)){QueueWallRotation(1); }}
void CubeHandler::RotateDown()  { if(CanRotate(4/3)){QueueWallRotation(4); }}
void CubeHandler::RotateUp()    { if(CanRotate(5/3)){QueueWallRotation(5); }}
void CubeHandler::RotateFront() { if(CanRotate(1)){QueueWallRotation(3); }}
void CubeHandler::RotateBack()  { if(CanRotate(2/3)){QueueWallRotation(2);}}

// Extracts axis (0 = X, 1 = Y, 2 = Z) from wall index
int CubeHandler::GetAxis(int wall) {
    return wall / 3;
}

// Extracts layer index (0, 1, 2) from wall index
int CubeHandler::GetLayer(int wall) {
    return wall % 3;
}

// Determines the correct rotation axis based on the wall index
glm::vec3 CubeHandler::GetRotationAxis(int wall) {
    if (wall < 3) return glm::vec3(1, 0, 0); // X-axis rotation
    if (wall < 6) return glm::vec3(0, 0, 1); // Z-axis rotation
    return glm::vec3(0, 1, 0);               // Y-axis rotation
}

// Updates the current rotation angle (clockwise or counterclockwise)
void CubeHandler::UpdateRotationAngle() {
    CurrentAngle += (Clockwise ? 1.0f : -1.0f);
}

// Rotates all affected cubes in the wall
void CubeHandler::RotateCubes(const std::vector<int>& cubes, glm::vec3 rotationAxis) {
    for (int i : cubes) {
        Cubes[i]->rotateCube(glm::radians(Clockwise), rotationAxis);
    }
}

// Checks if the rotation has reached 45 degrees (rotation complete)
bool CubeHandler::RotationTreshold() {
    return glm::abs(CurrentAngle) >= 45.0f;
}

// Finalizes the rotation by resetting variables and updating positions
void CubeHandler::ResetRotation(int wall) {
    ActiveRotations = -1;
    CurrentAngle = 0.0f;

    if (totalRotation[wall] % 2 == 0) {
        UpdateCubePositions(GetAxis(wall), GetLayer(wall));
        totalRotation[wall] = 0;
    }
}

// Checks for the next queued rotation and starts it if possible
void CubeHandler::ExecutePendingRotation() {
    if (ActiveRotations == -1 && !PendingRotations.empty()) {
        int nextRotation = PendingRotations.front();
        if (CanRotate(GetAxis(nextRotation))) {
            ActiveRotations = nextRotation;
            totalRotation[ActiveRotations]++;
        }
        PendingRotations.pop();
    }
}
