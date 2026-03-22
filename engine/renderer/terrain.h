//
// Created by Nemo li on 2023/12/18.
//

#ifndef OGL_TERRAIN_H
#define OGL_TERRAIN_H

#include <string>
#include "component/component.h"
#include "material.h"

class Terrain : public Component {

public:
    Terrain();

    void Render() override;

    void LoadTerrainTexture(const std::string &terrainTex);

    //设置Material
    void SetMaterial(Material *material) { this->material_ = material; };

    Material *material() { return material_; }

    float GetElevation(int x, int z);

private:
    const unsigned int NUM_PATCH_PTS = 4;
    unsigned rez = 20;
    Material *material_;
    unsigned int texture;

    std::vector<float> vertices;
    std::vector<float> elevation_vec;
    int width, height, nrChannels;

    //顶点数组句柄
    unsigned int vao_ = 0;

RTTR_ENABLE();
};


#endif //OGL_TERRAIN_H
