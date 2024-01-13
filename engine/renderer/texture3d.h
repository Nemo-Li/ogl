//
// Created by Nemo li on 2024/1/12.
//

#ifndef OGL_TEXTURE3D_H
#define OGL_TEXTURE3D_H

#include <string>
#include <map>
#include <vector>

class Texture3D {
private:
    const char *path = nullptr;
    std::map<std::string, int> imageIdMap;

public:
    Texture3D();

    virtual ~Texture3D();

    void use(int slot);

    void addImage(const char *name);

    int findImage(const char *name);

    void initTexture3D(std::string &dir);

    void releaseTexture3D();

    unsigned int textureArrayId;
    std::vector<std::string> imageNames;
};


#endif //OGL_TEXTURE3D_H
