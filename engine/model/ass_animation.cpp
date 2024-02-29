//
// Created by Nemo li on 2024/2/27.
//

#include "ass_animation.h"
#include <fstream>
#include <zconf.h>
#include "framework/application.h"

AssAnimation::AssAnimation() {
    vertCount = 0;
    faceCount = 0;
    boneCount = 0;
    aVertices.clear();
    aNormals.clear();
    aTangents.clear();
    aTexcoords.clear();
    aMids.clear();
    aTextures.clear();
    aAmbients.clear();
    aDiffuses.clear();
    aSpeculars.clear();
    aIndices.clear();
    aBoneids.clear();
    aWeights.clear();
    datasToExport.clear();
    inverseYZ = false;
}

AssAnimation::~AssAnimation() {
    aVertices.clear();
    aNormals.clear();
    aTangents.clear();
    aTexcoords.clear();
    aMids.clear();
    aTextures.clear();
    aAmbients.clear();
    aDiffuses.clear();
    aSpeculars.clear();
    aIndices.clear();
    aBoneids.clear();
    aWeights.clear();
}

float AssAnimation::getBoneFrame(AnimFrame *animation, float time, bool &end) {
    float ticks = time * animation->ticksPerSecond;
    float animTime = ticks;
    if (animTime > animation->duration - 0.01) {
        end = true;
        animTime = animation->duration - 0.01;
    } else end = false;

    return animTime * 100.0;
}

std::string AssAnimation::convertTexPath(const std::string &path) {
    int lc = path.find_last_of('/');
    int ld = path.find_last_of('\\');
    lc = lc > ld ? lc : ld;
    if (lc == std::string::npos) lc = 0;
    else lc += 1;
    std::string res = path.substr(lc); //Application::data_path() + "textures/" + path.substr(lc);
    printf("convert %s\n", res.data());
    return res;
}

void AssAnimation::exportAnims(std::string path) {
    for (unsigned int i = 0; i < getExportSize(); ++i) {
        AnimFrame *animation = datasToExport[i];
        float boneCount = animation->frames[0]->boneCount;
        float frameCount = animation->frames.size();
        float duration = animation->duration;
        float ticksPerSecond = animation->ticksPerSecond;

        const char *savePath = (path + "\\" + getName() + "_" + animation->getName() + ".t3a").data();
        if (access(savePath, 0) == 0) continue;

        std::ofstream of(savePath, std::ios::binary);
        of << boneCount << " " << frameCount << " " << duration << " " << ticksPerSecond << std::endl;
        for (unsigned int f = 0; f < frameCount; ++f) {
            Frame *frame = animation->frames[f];
            for (unsigned int b = 0; b < boneCount; ++b) {
                for (unsigned int inn = 0; inn < 12; ++inn)
                    of << frame->data[b * 12 + inn] << " ";
                of << std::endl;
            }
        }
        of.close();
    }
    clearExportData();
}

void AssAnimation::clearExportData() {
    for (unsigned int i = 0; i < datasToExport.size(); i++)
        delete datasToExport[i];
    datasToExport.clear();
}
