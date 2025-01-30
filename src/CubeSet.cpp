#include "CubeSet.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

CubeSet::CubeSet(Shader* shader, Texture* texture, VertexArray* vertexArray) 
    : CubeTranslationMatrix(glm::mat4(1.0f)),
      CubeRotationMatrix(glm::mat4(1.0f)),
      CubeScaleMatrix(glm::mat4(0.5f)),
      ActiveRotations(-1),
      LastFrameTime(0.0)
{
    int cube_id = 0;
    
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            for(int k = 0; k < 3; ++k) {
                glm::vec3 position(2.0f * (k - 3 / 2), 2.0f * (j - 3 / 2), 2.0f * (i - 3 / 2));
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f));
                Cubes[cube_id] = new Cube(cube_id, shader, texture, translation, vertexArray);
                CubeToLocation[i][j][k] = cube_id++;
            }
        }
    }
    
    totalRotation.resize(3 * 3, 0);
    for(int d = 0; d < 3; ++d) {
        WallIndices.push_back(d * 3);
        WallIndices.push_back(d * 3 + 3 - 1);
    }
}

std::vector<int> CubeSet::GetWallCubeIndices(int axis, int wallIndex) {
    
    std::vector<int> wallIndices;
    wallIndices.reserve(3 * 3);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            wallIndices.push_back(
                (axis == 0) ? CubeToLocation[i][j][wallIndex] :
                (axis == 1) ? CubeToLocation[wallIndex][i][j] :
                              CubeToLocation[j][wallIndex][i]
            );
        }
    }
    return wallIndices;
}

bool CubeSet::CanRotate(int axis) {
    
    for(int d = 0; d < 3; ++d) {
        if(d == axis) continue;
        for(int i = 0; i < 3; ++i) {
            if(totalRotation[d * 3 + i] % 2 == 1) return false;
        }
    }
    return true;
}

void CubeSet::ApplyRotation(int wall) {
    if(ActiveRotations != -1) {
        std::vector<int> indices = GetWallCubeIndices(wall/3, wall%3);
        CurrentRotationAngle += (Clockwise ? 1.0f : -1.0f);

        glm::vec3 rotationAxis = (wall < 3) ? glm::vec3(1, 0, 0) :
                                 (wall < 2 * 3) ? glm::vec3(0, 0, 1) :
                                 glm::vec3(0, 1, 0);
        
        for(int i : indices) {
            Cubes[i]->set_tot_rot(glm::radians(Clockwise ? 1.0f : -1.0f), rotationAxis);
        }

        if (glm::abs(CurrentRotationAngle) >= 45.0f) {
            ActiveRotations = -1;
            CurrentRotationAngle = 0.0f;
            if(totalRotation[wall] % 2 == 0) {
                UpdateCubePositions(wall/3, wall%3);
                totalRotation[wall] = 0; 
            }
        }
    }
    
    if(ActiveRotations == -1 && !WallRotations.empty()) {
        if(CanRotate(WallRotations.front()/3)) {
            ActiveRotations = WallRotations.front();
            totalRotation[ActiveRotations]++;
        }
        WallRotations.pop();
    }
}

void CubeSet::UpdateCubePositions(int axis, int wallIndex) {

    auto getIndex = [&](int i, int j) -> int& {
        switch (axis) {
            case 0: return CubeToLocation[i][j][wallIndex]; // X-axis
            case 1: return CubeToLocation[wallIndex][i][j]; // Y-axis
            case 2: return CubeToLocation[j][wallIndex][i]; // Z-axis
            default: throw std::runtime_error("Invalid axis in UpdateCubePositions");
        }
    };

    // **In-place rotation using 4-way swaps**
    for (int i = 0; i < 3 / 2; ++i) {
        for (int j = i; j < 3 - i - 1; ++j) {
            int temp = getIndex(i, j);

            if (Clockwise) {
                getIndex(i, j) = getIndex(2 - j, i);
                getIndex(2 - j, i) = getIndex(2 - i, 2 - j);
                getIndex(2 - i, 2 - j) = getIndex(j, 2 - i);
                getIndex(j, 2 - i) = temp;
            } else {
                getIndex(i, j) = getIndex(j, 2 - i);
                getIndex(j, 2 - i) = getIndex(2 - i, 2 - j);
                getIndex(2 - i, 2 - j) = getIndex(2 - j, i);
                getIndex(2 - j, i) = temp;
            }
        }
    }
}



void CubeSet::RotateWall(int wall)
{
    wall = WallIndices[wall];

    if (CanRotate(wall/3))
    {
        for(int i = 0; i < RotationAngle; i++)
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

void CubeSet::SwapRotationAxis(char dir) { //i think its for the bonus we dont have
    int axis = (dir == '^' || dir == 'v') ? 0 : (dir == '<' || dir == '>') ? 1 : 2;
    int start = axis * 2;
    int end = start + 1;

    if ((dir == '^' || dir == '<' || dir == 'I') && WallIndices[start] % 3 > 0 && WallIndices[end] % 3 < 3 - 1) {
        WallIndices[start]--;
        WallIndices[end]++;
    } else if ((dir == 'v' || dir == '>' || dir == 'O') && WallIndices[end] - WallIndices[start] > 1) {
        WallIndices[start]++;
        WallIndices[end]--;
    }
}


void CubeSet::SetClockWise()
{
    if(ActiveRotations == -1) Clockwise = !Clockwise; 
}

void CubeSet::Render(glm::mat4 view, glm::mat4 proj)
{
    
    // Change to "if" to enable motion
    while (ActiveRotations != -1 || !WallRotations.empty()) 
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


void CubeSet::Translate(int cube_id, glm::vec3 trans_vec)
{
    glm::mat3 inverse = glm::transpose(CubeRotationMatrix);
    Cubes[cube_id]->add_translate(inverse * trans_vec);
}

void CubeSet::RotationCube(int cube_id, float angle, glm::vec3 axis)
{
    glm::mat3 rot_inverse = glm::transpose(CubeRotationMatrix);
    Cubes[cube_id]->set_rotate(angle, rot_inverse * axis);
}

void CubeSet::TogglePicking(bool mode)
{
    EnableColorPicking = mode;
}

void CubeSet::Restart()
{
    CubeTranslationMatrix = glm::mat4(1.0f);
    CubeScaleMatrix = glm::mat4(1.0f);
    ActiveRotations = -1;
    
    int z = -3 / 2;
    int cube_id = 0;

    for(int i = 0; i<3; i++)
    {
        int y = -3 / 2;

        for(int j = 0; j<3; j++)
        {
            int x = -3 / 2;

            for(int k = 0; k<3; k++)
            {
                glm::vec3 offset = 3 % 2 == 1 ? glm::vec3(0.0f) : glm::vec3(0.5f);
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
    glm::mat4 model = CubeRotationMatrix * CubeTranslationMatrix * CubeScaleMatrix;
    glm::mat4 mvp = proj * view * model;

    for (Cube* c : Cubes) 
    {
        c->render_back_buffer(mvp);
    }

}

glm::vec3 CubeSet::GetCubePosition(int cubeID) {
    if (cubeID < 0 || cubeID >= 27) return glm::vec3(0.0f);

    // Extract position from transformation matrix
    return glm::vec3(Cubes[cubeID]->GetTransform()[3]); 
}


bool CubeSet::IdentifyPick()
{
    return EnableColorPicking;
}