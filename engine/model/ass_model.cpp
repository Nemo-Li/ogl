//
// Created by Nemo li on 2024/2/29.
//

#include "ass_model.h"
#include "ass_loader.h"

AssModel::AssModel(const char *path) {
    assLoader = new AssLoader(path);
}
