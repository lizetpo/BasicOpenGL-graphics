#include <CubeSet.h>

glm::mat4 CubeSet::translate_by(int x, int y, int z)
{
    glm::vec3 trans_safe = cube_to_location.size() % 2 == 1 ? glm::vec3(0.0f) : glm::vec3(0.5f);
    return glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * x, 2.0f * y, 2.0f * z) + trans_safe);  
}

CubeSet::CubeSet(int num_of_cubes, Shader* shader, Texture* tex, VertexArray* va) : 
    rubiks_trans(glm::mat4(1.0f)),
    rubiks_rot(glm::mat4(1.0f)),
    rubiks_scl(glm::mat4(1.0f)),
    is_rotating(-1),
    last_time(0)
{

    int z = -num_of_cubes / 2;
    cube_to_location.resize(num_of_cubes);
    cubes.resize(num_of_cubes * num_of_cubes * num_of_cubes);
        
    int cube_id = 0;

    for(int i = 0; i<num_of_cubes; i++)
    {
        cube_to_location[i].resize(num_of_cubes);
        int y = -num_of_cubes / 2;

        for(int j = 0; j<num_of_cubes; j++)
        {
            cube_to_location[i][j].resize(num_of_cubes);
            int x = -num_of_cubes / 2;

            for(int k = 0; k<num_of_cubes; k++)
            {
                cubes[cube_id] = new Cube(cube_id, shader, tex, translate_by(x, y, z), va);
                cube_to_location[i][j][k] = cube_id;
                
                x++;
                cube_id++;
            }
            y++;
        }
        z++;
    }
    
    for(int i = 0; i < num_of_cubes * num_of_cubes; i++) acc_rotation.push_back(0);
    set_char_to_wall(num_of_cubes);
}

void CubeSet::set_char_to_wall(int num_of_cubes)
{
    for(int d = 0; d<3; d++)
    {
        char_to_wall.push_back(d * num_of_cubes);
        char_to_wall.push_back(d * num_of_cubes + num_of_cubes - 1);
    }

    // print char to i created:
    for(int i : char_to_wall) 
    {
        std::cout<< i <<std::endl;
    }

}



CubeSet::~CubeSet() 
{
    for(Cube* c : cubes)
    {
        delete c;
    }
}

std::vector<int> CubeSet::get_cube_indices(int wall)
{
    std::vector<int> wall_indices;
    int num_of_cubes = cube_to_location.size();

    int axis = wall / num_of_cubes; //getting the axis of wall
    int wall_index = wall % num_of_cubes; //get the correct wall in the axis (e.g. left is 0, right is 2 in 3x3x3)

    switch (axis)
    {
        case 0: // X-axis
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    wall_indices.push_back(cube_to_location[i][j][wall_index]);
            break;

        case 1: // Y-axis
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    wall_indices.push_back(cube_to_location[wall_index][i][j]);
            break;

        case 2: // Z-axis
            for (int j = 0; j < num_of_cubes; ++j)
                for (int k = 0; k < num_of_cubes; ++k)
                    wall_indices.push_back(cube_to_location[j][wall_index][k]);
            break;

        default:
            break;
    }

    // print cubes on wall that should rotate:
    for(int i : wall_indices)
    {
        std::cout<< i <<std::endl;
    }
    
    return wall_indices;

}

bool CubeSet::can_rotate(int wall)
{
    int num_of_cubes = cube_to_location.size();
    int wall_axis = wall / num_of_cubes;
    for(int d = 0; d < 3; d++)
    {
        if(d == wall_axis) continue;
        for(int i = 0; i < num_of_cubes; i++)
        {
            if(acc_rotation[d * num_of_cubes + i] % 2 == 1) return false;
        }
    }  
    return true;
}

glm::vec3 CubeSet::get_rot_axis(int wall)
{
    int num_of_cubes = cube_to_location.size();

    if(wall >= 0 && wall < num_of_cubes) return glm::vec3(1.0f, 0.0f, 0.0f); //x-wall
    if(wall >= num_of_cubes && wall < num_of_cubes * 2) return glm::vec3(0.0f, 0.0f, 1.0f); //z-wall
    return glm::vec3(0.0f, 1.0f, 0.0f); //y-wall
}


void CubeSet::apply_rotation(int wall)
{
    if(is_rotating != -1) 
    {
        std::vector<int> indices = get_cube_indices(wall);
        curr_degree_of_wall += clockwise ? 1.0f : -1.0f;

        for(int i : indices)
        {
            cubes[i]->set_tot_rot(glm::radians(clockwise ? 1.0f : -1.0f), get_rot_axis(wall));
        }

        if (glm::abs(curr_degree_of_wall - 45.0f) < 0.01f || glm::abs(curr_degree_of_wall + 45.0f) < 0.01f) 
        {
            curr_degree_of_wall = glm::round(curr_degree_of_wall);
        }

        if(glm::abs(curr_degree_of_wall) >= 45.0f)
        {
            is_rotating = -1;
            curr_degree_of_wall = 0.0f;
            if(acc_rotation[wall] % 2 == 0)
            {
                update_cube_locations(wall);
                acc_rotation[wall] = 0; 
            }
        }
    }

    if(is_rotating == -1 && !wall_rotations_q.empty())
    {
        if(can_rotate(wall_rotations_q.front()))
        {
            is_rotating = wall_rotations_q.front();
            acc_rotation[is_rotating]++;
        }
        wall_rotations_q.pop();
    } 
}

void CubeSet::update_cube_locations(int wall)
{
    int num_of_cubes = cube_to_location.size();
    int axis = wall / num_of_cubes; 
    int wall_index = wall % num_of_cubes; 
    std::vector<std::vector<int>> temp_wall(num_of_cubes, std::vector<int>(num_of_cubes));

    switch (axis)
    {
        case 0: // X-axis
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    temp_wall[clockwise ? j : num_of_cubes - 1 - j][clockwise ? num_of_cubes - 1 - i : i] = cube_to_location[i][j][wall_index];
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    cube_to_location[i][j][wall_index] = temp_wall[i][j];
            break;

        case 1: // Y-axis
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    temp_wall[clockwise ? j : num_of_cubes - 1 - j][clockwise ? num_of_cubes - 1 - i : i] = cube_to_location[wall_index][i][j];
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    cube_to_location[wall_index][i][j] = temp_wall[i][j];
            break;

        case 2: // Z-axis
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    temp_wall[clockwise ? j : num_of_cubes - 1 - j][clockwise ? num_of_cubes - 1 - i : i] = cube_to_location[j][wall_index][i];
            for (int i = 0; i < num_of_cubes; ++i)
                for (int j = 0; j < num_of_cubes; ++j)
                    cube_to_location[j][wall_index][i] = temp_wall[i][j];
            break;

        default:
            break;
    }
}


void CubeSet::rotate_wall(int wall)
{
    wall = char_to_wall[wall];

    if (can_rotate(wall))
    {
        for(int i = 0; i < rotate_degree; i++)
        {
            wall_rotations_q.push(wall);
        }
    }
}

void CubeSet::set_rotate_degree(float mult)
{ 
    rotate_degree = glm::clamp((int) (rotate_degree * mult), 1, 4); 
}

void CubeSet::rand_mix()
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
        rotate_wall(wall_indices[wall_i]);

        file << "(" << wall_names[wall_indices[wall_i]] << ")\n";
    }

    file.close();
}

void CubeSet::model_rotate(float angle, glm::vec3 axis)
{
    rubiks_rot = glm::rotate(glm::mat4(1.0f), angle, axis) * rubiks_rot;
}

void CubeSet::change_center_of_rotation(char dir)
{
    int num_of_cubes = cube_to_location.size();

    switch(dir)
    {
        case '^': // X-axis wider
            if(char_to_wall[0] % num_of_cubes > 0 && char_to_wall[1] % num_of_cubes < num_of_cubes - 1)
            {
                char_to_wall[0]--;
                char_to_wall[1]++;
            } 
            break;
        case 'v': // X-axis narrower
            if(char_to_wall[1] - char_to_wall[0] > 1)
            {
                char_to_wall[0]++;
                char_to_wall[1]--;
            }
            break;
        case '<': // Y-axis wider
            if(char_to_wall[2] % num_of_cubes > 0 && char_to_wall[3] % num_of_cubes < num_of_cubes - 1)
            {
                char_to_wall[2]--;
                char_to_wall[3]++;
            } 
            break;
        case '>': // Y-axis narrower
            if(char_to_wall[3] - char_to_wall[2] > 1)
            {
                char_to_wall[2]++;
                char_to_wall[3]--;
            }
            break;
        case 'I': // Z-axis wider
            if(char_to_wall[4] % num_of_cubes > 0 && char_to_wall[5] % num_of_cubes < num_of_cubes - 1)
            {
                char_to_wall[4]--;
                char_to_wall[5]++;
            } 
            break;
        case 'O': // Z-axis narrower
            if(char_to_wall[5] - char_to_wall[4] > 1)
            {
                char_to_wall[4]++;
                char_to_wall[5]--;
            }
            break;
    }
}



void CubeSet::set_clockwise()
{
    if(is_rotating == -1) clockwise = !clockwise; 
}

void CubeSet::render_rubik(glm::mat4 view, glm::mat4 proj, double frame_time)
{
    /*        Motion    - start        */
    double delta_time = frame_time - last_time;

    while (delta_time >= (1.0f / 120.0f)) 
    {
        // Change to "while" to disable motion
        if (is_rotating != -1 || !wall_rotations_q.empty()) 
        {
            apply_rotation(is_rotating);
        }
        delta_time -= (1.0f / 120.0f);
    }

    last_time = frame_time - delta_time; 
    /*        Motion    - end          */

    glm::mat4 model = rubiks_rot * rubiks_trans * rubiks_scl;
    glm::mat4 mvp = proj * view * model;

    for (Cube* c : cubes) 
    {
        c->render(mvp);
    }


}


void CubeSet::translate_cube(int cube_id, glm::vec3 trans_vec)
{
    glm::mat3 inverse = glm::transpose(rubiks_rot);
    cubes[cube_id]->add_translate(inverse * trans_vec);
}

void CubeSet::rotate_cube(int cube_id, float angle, glm::vec3 axis)
{
    glm::mat3 rot_inverse = glm::transpose(rubiks_rot);
    cubes[cube_id]->set_rotate(angle, rot_inverse * axis);
}

void CubeSet::swap_picking(bool mode)
{
    picking_mode = mode;
}

void CubeSet::restart()
{
    rubiks_trans = glm::mat4(1.0f);
    rubiks_scl = glm::mat4(1.0f);
    is_rotating = -1;
    
    int z = -3 / 2;
    int cube_id = 0;

    for(int i = 0; i<3; i++)
    {
        cube_to_location[i].resize(3);
        int y = -3 / 2;

        for(int j = 0; j<3; j++)
        {
            cube_to_location[i][j].resize(3);
            int x = -3 / 2;

            for(int k = 0; k<3; k++)
            {
                cubes[cube_id]->set_transform(translate_by(x, y, z), glm::mat4(1.0f));
                cube_to_location[i][j][k] = cube_id;
                
                x++;
                cube_id++;
            }
            y++;
        }
        z++;
    }  
}

void CubeSet::draw_back_buffer(glm::mat4 view, glm::mat4 proj)
{
    glm::mat4 model = rubiks_rot * rubiks_trans * rubiks_scl;
    glm::mat4 mvp = proj * view * model;

    for (Cube* c : cubes) 
    {
        c->render_back_buffer(mvp);
    }

}

bool CubeSet::should_pick()
{
    return picking_mode;
}