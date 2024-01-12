//
// Created by Nemo li on 2024/1/11.
//

#include "mtl_loader.h"
#include <fstream>
#include <renderer/material.h>
#include <sstream>

using std::ifstream;
using std::string;
using std::istringstream;

MtlLoader::MtlLoader(const char *mtlPath) {
    mtlFilePath = mtlPath;
    mtlCount = 0;
    texture3D = new Texture3D;
    readMtlInfo();
    readMtlFile();
}

MtlLoader::~MtlLoader() {
    obj_mtl_map_.clear();
    if (texture3D) {
        delete texture3D;
        texture3D = nullptr;
    }
}

void MtlLoader::readMtlInfo() {
    ifstream infile(mtlFilePath);
    string l_line;
    while (getline(infile, l_line)) {
        //newmtl
        if (l_line[0] == 'n' && l_line[1] == 'e')
            mtlCount++;
    }
    infile.close();
}

void MtlLoader::readMtlFile() {
    ifstream infile(mtlFilePath);
    string l_line;
    int n = 0, t = 0, d = 0, a = 0, s = 0;

    string value, name, image;
    float red = 0, green = 0, blue = 0;
    Material *mtl = nullptr;
    while (getline(infile, l_line)) {
        if (!l_line.empty()) {
            istringstream ins(l_line);
            ins >> value;
            if (value == "newmtl") {
                ins >> name;
                mtl = new Material();
                mtl->name_ = name;
                obj_mtl_map_[name] = mtl;
                n++;
            } else if (value == "map_Kd") {
                ins >> image;
                if (mtl) {
                    if (texture3D->findImage(image.c_str()) < 0)
                        texture3D->addImage(image.c_str());
                    mtl->textureId.x = texture3D->findImage(image.c_str());
                }
                t++;
            } else if (value == "Kd") {
                ins >> red >> green >> blue;
                if (mtl) {
                    mtl->lightEffect->diffuse.x = red;
                    mtl->lightEffect->diffuse.y = green;
                    mtl->lightEffect->diffuse.z = blue;
                }
                d++;
            } else if (value == "Ka") {
                ins >> red >> green >> blue;
                if (mtl) {
                    mtl->lightEffect->ambient.x = red;
                    mtl->lightEffect->ambient.y = green;
                    mtl->lightEffect->ambient.z = blue;
                }
                a++;
            } else if (value == "Ks") {
                ins >> red >> green >> blue;
                if (mtl) {
                    mtl->lightEffect->specular.x = red;
                    mtl->lightEffect->specular.y = green;
                    mtl->lightEffect->specular.z = blue;
                }
                s++;
            }
        }
    }
    infile.close();
}
