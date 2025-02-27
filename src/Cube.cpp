#include <Cube.h>



Cube::Cube(int id, Shader* shader, Texture* tex, glm::mat4 trans, VertexArray* va) : 
    index(id),
    shader(shader),
    cubeTexture(tex),
    transformMatrix(trans),
    rotationMatrix(glm::mat4(1.0f)),
    vertexArray(va)
{
}


void Cube::render(glm::mat4 mvp)
{
    glm::mat4 cube_model = rotationMatrix * transformMatrix;   
    glm::mat4 draw_mvp = mvp * cube_model;
    glm::vec4 color;


    color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);

    cubeTexture->Bind(0.0);
    shader->Bind();
    shader->SetUniform4f("u_Color", color); 
    shader->SetUniformMat4f("u_MVP", draw_mvp);
    shader->SetUniform1i("u_Texture", 0);
    vertexArray->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
}

void Cube::z_buffer(glm::mat4 mvp)
{

    glm::vec4 color = glm::vec4(static_cast<float>(index) / 255.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 cube_model = rotationMatrix * transformMatrix;   
    glm::mat4 draw_mvp = mvp * cube_model;
    cubeTexture->Bind(0);
    shader->Bind();
    shader->SetUniform4f("u_Color", color);
    shader->SetUniformMat4f("u_MVP", draw_mvp);
    vertexArray->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
}



void Cube::translateCube(glm::vec3 trans_vec)
{
    glm::mat3 inverse = glm::transpose(rotationMatrix * transformMatrix);
    transformMatrix = glm::translate(transformMatrix, inverse * trans_vec);
}

void Cube::transformCube(glm::mat4 trans, glm::mat4 rot)
{
    transformMatrix = trans;
    rotationMatrix = rot;
}

void Cube::rotateWorld(float angle, glm::vec3 axis)
{
    glm::mat3 inverse = glm::transpose(rotationMatrix * transformMatrix);
    transformMatrix = transformMatrix * glm::rotate(glm::mat4(1.0f), angle, inverse * axis);
}

void Cube::rotateCube(float angle, glm::vec3 axis)
{
    rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis) * rotationMatrix;
}

void Cube::setTransform(glm::mat4 trans)
{
    transformMatrix = trans;
}

