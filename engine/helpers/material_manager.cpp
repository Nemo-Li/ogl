//
// Created by Nemo li on 2024/2/28.
//

#include "material_manager.h"

MaterialManager *MaterialManager::instance = nullptr;

void MaterialManager::Init() {
    if (!MaterialManager::instance)
        MaterialManager::instance = new MaterialManager();
}

void MaterialManager::Release() {
    if (MaterialManager::instance)
        delete MaterialManager::instance;
    MaterialManager::instance = nullptr;
}

unsigned int MaterialManager::add(Material *material) {
    materialList.push_back(material);
    materialMap[material->name()] = material;
    int mid = materialList.size() - 1; // Start from 0
    material->id_ = mid;
    return mid;
}

void MaterialManager::remove(unsigned int i) {
    if (materialList.size() < i + 1) return;
    int oldMid = materialList[i]->id_;
    std::string oldName = materialList[i]->name_;
    delete materialList[i];
    Material *mtlEmp = new Material();
    mtlEmp->name_ = oldName.c_str();
    mtlEmp->id_ = oldMid;
    materialList[i] = mtlEmp;
    materialMap[oldName] = mtlEmp;
}

Material *MaterialManager::find(unsigned int i) {
    if (materialList.size() < i + 1) return NULL;
    return materialList[i];
}

int MaterialManager::find(std::string name) {
    if (materialMap.count(name) <= 0) return 0;
    return materialMap[name]->id_;
}

unsigned int MaterialManager::size() {
    return materialList.size();
}
