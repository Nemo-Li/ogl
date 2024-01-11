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
    readMtlInfo();
    readMtlFile();
}

MtlLoader::~MtlLoader() {
    obj_mtl_map_.clear();
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

}
