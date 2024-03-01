//
// Created by Nemo li on 2024/2/27.
//

#ifndef OGL_ASS_ANIMATION_H
#define OGL_ASS_ANIMATION_H

#include <cstdlib>
#include <string>
#include <vector>
#include <renderer/material.h>
#include <renderer/texture3d.h>
#include "glm/glm.hpp"

struct Frame {
    int boneCount;
    float *data;

    Frame(int bc) {
        boneCount = bc;
        data = (float *) malloc(boneCount * 16 * sizeof(float));
    }

    ~Frame() {
        free(data);
    }
};

struct AnimFrame {
    std::string name;
    float duration, ticksPerSecond;
    std::vector<Frame *> frames;

    AnimFrame(const char *n) {
        name = n;
        duration = 0.0;
        ticksPerSecond = 0.0;
        frames.clear();
    }

    ~AnimFrame() {
        for (unsigned int i = 0; i < frames.size(); i++)
            delete frames[i];
        frames.clear();
    }

    std::string getName() {
        return name;
    }

    void setDuration(float d) {
        duration = d;
    }

    void setTicksPerSecond(float t) {
        ticksPerSecond = t;
    }
};

class AssAnimation {
public:
    std::string name;
    bool inverseYZ;
    Texture3D *texture3D;
public:
    int faceCount = 0, vertCount = 0, boneCount = 0;
    std::vector<glm::vec3> aVertices;
    std::vector<glm::vec3> aNormals;
    std::vector<glm::vec3> aTangents;
    std::vector<glm::vec2> aTexcoords;
    std::vector<unsigned int> aMids;
    std::vector<Material *> aTextures;
    std::vector<glm::vec3> aAmbients;
    std::vector<glm::vec3> aDiffuses;
    std::vector<glm::vec3> aSpeculars;
    std::vector<unsigned int> aIndices;
    std::vector<glm::uvec4> aBoneids;
    std::vector<glm::vec4> aWeights;
    std::vector<AnimFrame *> datasToExport;
public:
    AssAnimation();

    virtual ~AssAnimation();

public:
    float getBoneFrame(AnimFrame *animation, float time, bool &end);

    std::string getName() { return name; }

    void setName(std::string value) { name = value; }

    std::string convertTexPath(const std::string &path);

    unsigned int getExportSize() { return datasToExport.size(); }

    void exportAnims(std::string path);

private:
    void clearExportData();
};


#endif //OGL_ASS_ANIMATION_H
