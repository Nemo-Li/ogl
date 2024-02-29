//
// Created by Nemo li on 2024/2/29.
//

#ifndef OGL_ASS_MODEL_H
#define OGL_ASS_MODEL_H

class AssLoader;

class Texture3D;

class Shader;

class AssModel {
private:
    AssLoader *assLoader;

    void loadAssModel(const char *path);

public:
    Texture3D *texture3D = nullptr;
    Shader *shader = nullptr;

public:
    AssModel(const char *path);
};


#endif //OGL_ASS_MODEL_H
