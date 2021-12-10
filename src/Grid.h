//
// Created by Nemo li on 2021/12/9.
//

#ifndef OGL_GRID_H
#define OGL_GRID_H

#include <glad/glad.h>
#include "Shader.h"
#include "Model.h"

class Grid : Model {
public:
    unsigned int lineVAO;
    float lineVertices[6]{
            0.0f, 0.0f, -2.0f,
            0.0f, 0.0f, 2.0f
    };

    Shader *shader = nullptr;

    glm::mat4 *modelMatrix;

    Grid(Shader *shader, glm::mat4 *modelMatrix);

    void initVAO();

    void draw();
};


#endif //OGL_GRID_H
