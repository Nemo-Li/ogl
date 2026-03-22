//
// Created by Nemo li on 2024/2/4.
//

#ifndef OGL_GRASS_H
#define OGL_GRASS_H

#include <string>
#include "component/component.h"
#include "material.h"

class Grass : public Component {
public:
    Grass();

    void Render() override;

    void LoadTerrainTexture(const std::string &terrainTex);

    //设置Material
    void SetMaterial(Material *material) { this->material_ = material; };

    Material *material() { return material_; }

private:
    const unsigned int NUM_PATCH_PTS = 4;
    unsigned rez = 20;
    Material *material_;
    unsigned int texture;

    std::vector<float> vertices;
    int width, height, nrChannels;

    //顶点数组句柄
    unsigned int vao_ = 0;

RTTR_ENABLE();
};


#endif //OGL_GRASS_H
