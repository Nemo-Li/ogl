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

    void Render();

    void LoadTerrainTexture(const std::string &terrainTex);

    //设置Material
    void SetMaterial(Material *material) { this->material_ = material; };

    Material *material() { return material_; }

private:
    Material *material_;

    int numStrips;
    int numTrisPerStrip;

    std::vector<float> vertices;
    std::vector<unsigned> indices;
    int width, height, nrChannels;

    //顶点缓冲区对象句柄
    unsigned int vertex_buffer_object_handle_ = 0;
    //顶点数组对象句柄
    unsigned int vertex_array_object_handle_ = 0;

RTTR_ENABLE();
};


#endif //OGL_TERRAIN_H
