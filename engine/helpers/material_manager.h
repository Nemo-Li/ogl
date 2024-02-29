//
// Created by Nemo li on 2024/2/28.
//

#ifndef OGL_MATERIAL_MANAGER_H
#define OGL_MATERIAL_MANAGER_H

#include <cstdlib>
#include <map>
#include <string>
#include "renderer/material.h"

class MaterialManager {
public:
    static MaterialManager *instance;

public:
    static void Init();

    static void Release();

    std::vector<Material *> materialList;
    std::map<std::string, Material *> materialMap;

    unsigned int add(Material *material);

    void remove(unsigned int i);

    Material *find(unsigned int i);

    int find(std::string name);

    unsigned int size();
};


#endif //OGL_MATERIAL_MANAGER_H
