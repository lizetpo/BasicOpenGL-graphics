#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>



class Cube
{
    public:
        int index;
        glm::mat4 rotationMatrix;
        glm::mat4 transformMatrix;        

        //color picking
        Shader* shader;
        Texture* cubeTexture;
        VertexArray* vertexArray;
    
        Cube(int id, Shader* shader, Texture* tex, glm::mat4 trans, VertexArray* va);

        Cube(int id)
        {
            index = id;
        }

        Cube() {
            index = 0;
        }


        void transformCube(glm::mat4 trans, glm::mat4 rot);
        void rotateWorld(float angle, glm::vec3 axis);
        void rotateCube(float angle, glm::vec3 axis);
        void translateCube(glm::vec3 trans_vec);
        void setTransform(glm::mat4 trans);
        inline glm::mat4 GetTransform() const { return transformMatrix; }

        void render(glm::mat4 mvp);
        void z_buffer(glm::mat4 mvp);


};