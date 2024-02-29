//
// Created by Nemo li on 2024/2/29.
//

#ifndef OGL_ASS_MODEL_H
#define OGL_ASS_MODEL_H

class AssLoader;

class AssModel {
private:
    AssLoader *assLoader;

public:
    AssModel(const char *path);
};


#endif //OGL_ASS_MODEL_H
