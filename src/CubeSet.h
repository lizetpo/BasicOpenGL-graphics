#pragma once

#include <vector>
#include <Cube.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <cmath>
#include <queue>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CubeSet
{
    private:
        // Cubes data structure:
        std::vector<Cube*> cubes;
        std::vector<std::vector<std::vector<int>>> cube_to_location;
        std::queue<int> wall_rotations_q;

        // Model matrices
        glm::mat4 rubiks_trans;
        glm::mat4 rubiks_rot;
        glm::mat4 rubiks_scl;

        // Locks & rotations
        std::vector<int> acc_rotation; 
        bool clockwise = true;
        int rotate_degree = 2;
        float curr_degree_of_wall = 0.0f;
        std::vector<int> char_to_wall; // 0 = L, 1 = R, 2 = D, 3 = U, 4 = F, 5 = B 
        int is_rotating;
        double last_time;
        bool picking_mode = false;

        glm::mat4 translate_by(int x, int y, int z);
        bool can_rotate(int wall);
        void apply_rotation(int wall);
        void update_cube_locations(int wall);
        std::vector<int> get_cube_indices(int wall);
        glm::vec3 get_rot_axis(int wall);
        void set_char_to_wall(int num_of_cubes);

    public:
        CubeSet(int num_of_cubes, Shader* shader, Texture* tex, VertexArray* va);
        ~CubeSet();

        // WALL ROTATIONS
        void rotate_wall(int wall);
        void change_center_of_rotation(char dir);
        void set_rotate_degree(float mult);
        void set_clockwise();
        
        // MIX & SOLVE

        void rand_mix();
        
        // VIEW
        void model_rotate(float angle, glm::vec3 axis);

        // RENDER
        void render_rubik(glm::mat4 view, glm::mat4 proj, double frame_time);

        // PICKING
        void translate_cube(int cube_id, glm::vec3 trans_vec);
        void rotate_cube(int cube_id, float angle, glm::vec3 axis);
        void swap_picking(bool mode);
        bool should_pick();
        void restart();
        void draw_back_buffer(glm::mat4 view, glm::mat4 proj);
        
};