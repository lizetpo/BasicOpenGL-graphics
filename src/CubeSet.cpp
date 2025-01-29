#include "CubeSet.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

CubeSet::CubeSet(int numCubes, Shader* shader, Texture* texture, VertexArray* vertexArray) 
    : CubeSetTranslation(glm::mat4(1.0f)),
      CubeSetRotation(glm::mat4(1.0f)),
      CubeSetScale(glm::mat4(1.0f)),
      ActiveRotations(-1),
      LastFrameTime(0.0)
{
    int z = -numCubes / 2;
    CubeToLocation.resize(numCubes);
    Cubes.resize(numCubes * numCubes * numCubes);
        
    int cube_id = 0;

    for(int i = 0; i<numCubes; i++)
    {
        CubeToLocation[i].resize(numCubes);
        int y = -numCubes / 2;

        for(int j = 0; j<numCubes; j++)
        {
            CubeToLocation[i][j].resize(numCubes);
            int x = -numCubes / 2;

            for(int k = 0; k<numCubes; k++)
            {

                glm::vec3 offset = CubeToLocation.size() % 2 == 1 ? glm::vec3(0.0f) : glm::vec3(0.5f);
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * x, 2.0f * y, 2.0f * z) + offset);  
                Cubes[cube_id] = new Cube(cube_id, shader, texture, translation, vertexArray);
                CubeToLocation[i][j][k] = cube_id;
                
                x++;
                cube_id++;
            }
            y++;
        }
        z++;
    }
    
    for(int i = 0; i < numCubes * numCubes; i++) accumulatedRotation.push_back(0);
    for(int d = 0; d<3; d++)
    {
        WallIndices.push_back(d * numCubes);
        WallIndices.push_back(d * numCubes + numCubes - 1);
    }
}

// 

std::vector<int> CubeSet::GetWallCubeIndices(int wall) {
    int cubesNum = CubeToLocation.size();
    int axis = wall / cubesNum;
    int wallIndex = wall % cubesNum;

    // Pre-allocate memory for performance optimization
    std::vector<int> wallIndices;
    wallIndices.reserve(cubesNum * cubesNum);

    for (int i = 0; i < cubesNum; ++i) {
        for (int j = 0; j < cubesNum; ++j) {
            switch (axis) {
                case 0: // X-axis
                    wallIndices.push_back(CubeToLocation[i][j][wallIndex]);
                    break;
                case 1: // Y-axis
                    wallIndices.push_back(CubeToLocation[wallIndex][i][j]);
                    break;
                case 2: // Z-axis
                    wallIndices.push_back(CubeToLocation[j][wallIndex][i]);
                    break;
            }
        }
    }

    // Debug print (only enable when necessary)
    #ifdef DEBUG
        for (int i : wallIndices) {
            std::cout << i << std::endl;
        }
    #endif

    return wallIndices;
}


bool CubeSet::CanRotate(int wall)
{
    int num_of_cubes = CubeToLocation.size();
    int wall_axis = wall / num_of_cubes;
    for(int d = 0; d < 3; d++)
    {
        if(d == wall_axis) continue;
        for(int i = 0; i < num_of_cubes; i++)
        {
            if(accumulatedRotation[d * num_of_cubes + i] % 2 == 1) return false;
        }
    }  
    return true;
}

void CubeSet::ApplyRotation(int wall)
{
    if(ActiveRotations != -1) 
    {
        std::vector<int> indices = GetWallCubeIndices(wall);
        CurrentRotationAngle += Clockwise ? 1.0f : -1.0f;

        for(int i : indices)
        {
            int num_of_cubes = CubeToLocation.size();

            glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f); //Y
            if(wall >= 0 && wall < CubeToLocation.size()) rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f); //X
            if(wall >= CubeToLocation.size() && wall < CubeToLocation.size() * 2) rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f); //Z
            Cubes[i]->set_tot_rot(glm::radians(Clockwise ? 1.0f : -1.0f), rotationAxis);
        }

        if (glm::abs(CurrentRotationAngle - 45.0f) < 0.01f || glm::abs(CurrentRotationAngle + 45.0f) < 0.01f) 
        {
            CurrentRotationAngle = glm::round(CurrentRotationAngle);
        }

        if(glm::abs(CurrentRotationAngle) >= 45.0f)
        {
            ActiveRotations = -1;
            CurrentRotationAngle = 0.0f;
            if(accumulatedRotation[wall] % 2 == 0)
            {
                UpdateCubePositions(wall);
                accumulatedRotation[wall] = 0; 
            }
        }
    }

    if(ActiveRotations == -1 && !WallRotations.empty())
    {
        if(CanRotate(WallRotations.front()))
        {
            ActiveRotations = WallRotations.front();
            accumulatedRotation[ActiveRotations]++;
        }
        WallRotations.pop();
    } 
}

// void CubeSet::UpdateCubePositions(int wall)
// {
//     int num_of_cubes = CubeToLocation.size();
//     int axis = wall / num_of_cubes; 
//     int wall_index = wall % num_of_cubes; 
//     std::vector<std::vector<int>> temp_wall(num_of_cubes, std::vector<int>(num_of_cubes));

//     switch (axis)
//     {
//         case 0: // X-axis
//             for (int i = 0; i < num_of_cubes; ++i)
//                 for (int j = 0; j < num_of_cubes; ++j)
//                     temp_wall[Clockwise ? j : num_of_cubes - 1 - j][Clockwise ? num_of_cubes - 1 - i : i] = CubeToLocation[i][j][wall_index];
//             for (int i = 0; i < num_of_cubes; ++i)
//                 for (int j = 0; j < num_of_cubes; ++j)
//                     CubeToLocation[i][j][wall_index] = temp_wall[i][j];
//             break;

//         case 1: // Y-axis
//             for (int i = 0; i < num_of_cubes; ++i)
//                 for (int j = 0; j < num_of_cubes; ++j)
//                     temp_wall[Clockwise ? j : num_of_cubes - 1 - j][Clockwise ? num_of_cubes - 1 - i : i] = CubeToLocation[wall_index][i][j];
//             for (int i = 0; i < num_of_cubes; ++i)
//                 for (int j = 0; j < num_of_cubes; ++j)
//                     CubeToLocation[wall_index][i][j] = temp_wall[i][j];
//             break;

//         case 2: // Z-axis
//             for (int i = 0; i < num_of_cubes; ++i)
//                 for (int j = 0; j < num_of_cubes; ++j)
//                     temp_wall[Clockwise ? j : num_of_cubes - 1 - j][Clockwise ? num_of_cubes - 1 - i : i] = CubeToLocation[j][wall_index][i];
//             for (int i = 0; i < num_of_cubes; ++i)
//                 for (int j = 0; j < num_of_cubes; ++j)
//                     CubeToLocation[j][wall_index][i] = temp_wall[i][j];
//             break;

//         default:
//             break;
//     }
// }

// void CubeSet::UpdateCubePositions(int wall) {
//     int numCubes = CubeToLocation.size();
//     int axis = wall / numCubes;
//     int wallIndex = wall % numCubes;

//     // Temporary storage for rotated layer
//     std::vector<std::vector<int>> tempLayer(numCubes, std::vector<int>(numCubes));

//     // Lambda function to handle cube index retrieval
//     auto getIndex = [&](int i, int j) -> int& {
//         switch (axis) {
//             case 0: return CubeToLocation[i][j][wallIndex]; // X-axis
//             case 1: return CubeToLocation[wallIndex][i][j]; // Y-axis
//             case 2: return CubeToLocation[j][wallIndex][i]; // Z-axis
//             default: throw std::runtime_error("Invalid axis in UpdateCubePositions");
//         }
//     };

//     // Rotate the layer 90 degrees based on direction
//     for (int i = 0; i < numCubes; ++i) {
//         for (int j = 0; j < numCubes; ++j) {
//             int newI = Clockwise ? j : numCubes - 1 - j;
//             int newJ = Clockwise ? numCubes - 1 - i : i;
//             tempLayer[newI][newJ] = getIndex(i, j);
//         }
//     }

//     // Copy the rotated layer back
//     for (int i = 0; i < numCubes; ++i) {
//         for (int j = 0; j < numCubes; ++j) {
//             getIndex(i, j) = tempLayer[i][j];
//         }
//     }
// }
void CubeSet::UpdateCubePositions(int wall) {
    int numCubes = CubeToLocation.size();
    int axis = wall / numCubes;
    int wallIndex = wall % numCubes;

    // Lambda function to get a reference to the cube index
    auto getIndex = [&](int i, int j) -> int& {
        switch (axis) {
            case 0: return CubeToLocation[i][j][wallIndex]; // X-axis
            case 1: return CubeToLocation[wallIndex][i][j]; // Y-axis
            case 2: return CubeToLocation[j][wallIndex][i]; // Z-axis
            default: throw std::runtime_error("Invalid axis in UpdateCubePositions");
        }
    };

    // **In-place rotation using 4-way swaps**
    for (int i = 0; i < numCubes / 2; ++i) {
        for (int j = i; j < numCubes - i - 1; ++j) {
            int temp = getIndex(i, j);

            if (Clockwise) {
                getIndex(i, j) = getIndex(numCubes - 1 - j, i);
                getIndex(numCubes - 1 - j, i) = getIndex(numCubes - 1 - i, numCubes - 1 - j);
                getIndex(numCubes - 1 - i, numCubes - 1 - j) = getIndex(j, numCubes - 1 - i);
                getIndex(j, numCubes - 1 - i) = temp;
            } else {
                getIndex(i, j) = getIndex(j, numCubes - 1 - i);
                getIndex(j, numCubes - 1 - i) = getIndex(numCubes - 1 - i, numCubes - 1 - j);
                getIndex(numCubes - 1 - i, numCubes - 1 - j) = getIndex(numCubes - 1 - j, i);
                getIndex(numCubes - 1 - j, i) = temp;
            }
        }
    }
}



void CubeSet::RotateWall(int wall)
{
    wall = WallIndices[wall];

    if (CanRotate(wall))
    {
        for(int i = 0; i < RotateDegree; i++)
        {
            WallRotations.push(wall);
        }
    }
}

void CubeSet::RotateLeft()  { RotateWall(0); }
void CubeSet::RotateRight() { RotateWall(1); }
void CubeSet::RotateDown()  { RotateWall(4); }
void CubeSet::RotateUp()    { RotateWall(5); }
void CubeSet::RotateFront() { RotateWall(3); }
void CubeSet::RotateBack()  { RotateWall(2); }


void CubeSet::Mix()
{
    std::vector<int> wall_indices({0, 1, 2, 3, 4, 5});
    std::vector<std::string> wall_names({"L", "R", "D", "U", "F", "B"});

    std::ofstream file("mixer.txt");
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open output file.\n";
        return;
    }


    for(int i = 0; i < 30; i++)
    {
        int wall_i = std::rand() % wall_indices.size();
        RotateWall(wall_indices[wall_i]);

        file << "(" << wall_names[wall_indices[wall_i]] << ")\n";
    }

    file.close();
}

void CubeSet::Rotate(float angle, glm::vec3 axis)
{
    CubeSetRotation = glm::rotate(glm::mat4(1.0f), angle, axis) * CubeSetRotation;
}

void CubeSet::SwapRotationAxis(char dir)
{
    int num_of_cubes = CubeToLocation.size();

    switch(dir)
    {
        case '^': // X-axis wider
            if(WallIndices[0] % num_of_cubes > 0 && WallIndices[1] % num_of_cubes < num_of_cubes - 1)
            {
                WallIndices[0]--;
                WallIndices[1]++;
            } 
            break;
        case 'v': // X-axis narrower
            if(WallIndices[1] - WallIndices[0] > 1)
            {
                WallIndices[0]++;
                WallIndices[1]--;
            }
            break;
        case '<': // Y-axis wider
            if(WallIndices[2] % num_of_cubes > 0 && WallIndices[3] % num_of_cubes < num_of_cubes - 1)
            {
                WallIndices[2]--;
                WallIndices[3]++;
            } 
            break;
        case '>': // Y-axis narrower
            if(WallIndices[3] - WallIndices[2] > 1)
            {
                WallIndices[2]++;
                WallIndices[3]--;
            }
            break;
        case 'I': // Z-axis wider
            if(WallIndices[4] % num_of_cubes > 0 && WallIndices[5] % num_of_cubes < num_of_cubes - 1)
            {
                WallIndices[4]--;
                WallIndices[5]++;
            } 
            break;
        case 'O': // Z-axis narrower
            if(WallIndices[5] - WallIndices[4] > 1)
            {
                WallIndices[4]++;
                WallIndices[5]--;
            }
            break;
    }
}



void CubeSet::SetClockWise()
{
    if(ActiveRotations == -1) Clockwise = !Clockwise; 
}

void CubeSet::Render(glm::mat4 view, glm::mat4 proj, double frame_time)
{
    /*        Motion    - start        */
    double delta_time = frame_time - LastFrameTime;

    while (delta_time >= (1.0f / 120.0f)) 
    {
        // Change to "while" to disable motion
        if (ActiveRotations != -1 || !WallRotations.empty()) 
        {
            ApplyRotation(ActiveRotations);
        }
        delta_time -= (1.0f / 120.0f);
    }

    LastFrameTime = frame_time - delta_time; 
    /*        Motion    - end          */

    glm::mat4 model = CubeSetRotation * CubeSetTranslation * CubeSetScale;
    glm::mat4 mvp = proj * view * model;

    for (Cube* c : Cubes) 
    {
        c->render(mvp);
    }


}


void CubeSet::Translate(int cube_id, glm::vec3 trans_vec)
{
    glm::mat3 inverse = glm::transpose(CubeSetRotation);
    Cubes[cube_id]->add_translate(inverse * trans_vec);
}

void CubeSet::RotationCube(int cube_id, float angle, glm::vec3 axis)
{
    glm::mat3 rot_inverse = glm::transpose(CubeSetRotation);
    Cubes[cube_id]->set_rotate(angle, rot_inverse * axis);
}

void CubeSet::TogglePicking(bool mode)
{
    ColorPicking = mode;
}

void CubeSet::Restart()
{
    CubeSetTranslation = glm::mat4(1.0f);
    CubeSetScale = glm::mat4(1.0f);
    ActiveRotations = -1;
    
    int z = -3 / 2;
    int cube_id = 0;

    for(int i = 0; i<3; i++)
    {
        CubeToLocation[i].resize(3);
        int y = -3 / 2;

        for(int j = 0; j<3; j++)
        {
            CubeToLocation[i][j].resize(3);
            int x = -3 / 2;

            for(int k = 0; k<3; k++)
            {
                glm::vec3 offset = CubeToLocation.size() % 2 == 1 ? glm::vec3(0.0f) : glm::vec3(0.5f);
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * x, 2.0f * y, 2.0f * z) + offset);  
                Cubes[cube_id]->set_transform(translation, glm::mat4(1.0f));
                CubeToLocation[i][j][k] = cube_id;
                
                x++;
                cube_id++;
            }
            y++;
        }
        z++;
    }  
}

void CubeSet::Buffer(glm::mat4 view, glm::mat4 proj)
{
    glm::mat4 model = CubeSetRotation * CubeSetTranslation * CubeSetScale;
    glm::mat4 mvp = proj * view * model;

    for (Cube* c : Cubes) 
    {
        c->render_back_buffer(mvp);
    }

}

glm::vec3 CubeSet::GetCubePosition(int cubeID) {
    if (cubeID < 0 || cubeID >= Cubes.size()) return glm::vec3(0.0f);

    // Extract position from transformation matrix
    return glm::vec3(Cubes[cubeID]->GetTransform()[3]); 
}


bool CubeSet::IdentifyPick()
{
    return ColorPicking;
}