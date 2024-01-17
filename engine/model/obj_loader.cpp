//
// Created by Nemo li on 2024/1/11.
//

#include <fstream>
#include <string>
#include <sstream>
#include "obj_loader.h"

using std::string;
using std::ifstream;
using std::istringstream;

ObjLoader::ObjLoader(const char *objPath, const char *mtlPath, int vtNum) {
    objFilePath = objPath;
    mtlFilePath = mtlPath;
    vtNumber = vtNum;
    vCount = 0;
    vnCount = 0;
    vtCount = 0;
    faceCount = 0;
    readObjInfo();
    readObjFile();
    mtlLoader = new MtlLoader(mtlFilePath);
}

void ObjLoader::readObjInfo() {
    std::ifstream infile(objFilePath);
    std::string l_line;

    //一行一行读取
    while (getline(infile, l_line)) {
        if (l_line[0] == 'v') {
            if (l_line[1] == 'n')
                vnCount++;
            else if (l_line[1] == 't')
                vtCount++;
            else
                vCount++;
        }
        if (l_line[0] == 'f') {
            faceCount++;
        }
        //use mtl
        if (l_line[0] == 'u' && l_line[1] == 's') {
            mtCount++;
        }
    }
    infile.close();
    printf("vCount %d \n", vCount);
    printf("faceCount %d \n", faceCount);
}

void ObjLoader::readObjFile() {
    vArr = new float *[vCount];
    for (int i = 0; i < vCount; i++)
        vArr[i] = new float[3];

    vnArr = new float *[vnCount];
    for (int i = 0; i < vnCount; i++)
        vnArr[i] = new float[3];

    vtArr = new float *[vtCount];
    for (int i = 0; i < vtCount; i++)
        vtArr[i] = new float[vtNumber];

    fvArr = new int *[faceCount];
    ftArr = new int *[faceCount];
    fnArr = new int *[faceCount];
    for (int i = 0; i < faceCount; i++) {
        fvArr[i] = new int[3];
        ftArr[i] = new int[3];
        fnArr[i] = new int[3];
    }
    mtArr = new string[faceCount];

    ifstream infile(objFilePath);
    string l_line;
    int ii = 0, tt = 0, jj = 0, kk = 0;

    string s1;
    float f2, f3, f4;
    string mtl("");

    while (getline(infile, l_line)) {
        if (l_line[0] == 'v') {
            //vn
            if (l_line[1] == 'n') {
                istringstream ins(l_line);
                ins >> s1 >> f2 >> f3 >> f4;
                vnArr[ii][0] = f2;
                vnArr[ii][1] = f3;
                vnArr[ii][2] = f4;
                ii++;
            } else if (l_line[1] == 't') {//vt
                istringstream ins(l_line);
                if (vtNumber == 3) {
                    ins >> s1 >> f2 >> f3 >> f4;
                    vtArr[tt][0] = f2;
                    vtArr[tt][1] = f3;
                    vtArr[tt][2] = f4;
                } else {
                    ins >> s1 >> f2 >> f3;
                    vtArr[tt][0] = f2;
                    vtArr[tt][1] = f3;
                }
                tt++;
            } else {//v
                istringstream ins(l_line);
                ins >> s1 >> f2 >> f3 >> f4;
                vArr[jj][0] = f2;
                vArr[jj][1] = f3;
                vArr[jj][2] = f4;
                jj++;
            }
        } else if (l_line[0] == 'f') { //f
            istringstream ins(l_line);
            float a;
            //开头为f 字符为f
            ins >> s1;
            int i, k;
            for (i = 0; i < 3; i++) {
                ins >> s1;
                //读取面 分三段
                a = 0;
                //读取数字 多位数字则高位*10 迭代相加 字符转数字 -48
                for (k = 0; s1[k] != '/'; k++)
                    a = a * 10 + (s1[k] - 48);
                fvArr[kk][i] = a;

                a = 0;
                for (k = k + 1; s1[k] != '/'; k++)
                    a = a * 10 + (s1[k] - 48);
                ftArr[kk][i] = a;

                a = 0;
                for (k = k + 1; s1[k]; k++)
                    a = a * 10 + (s1[k] - 48);
                fnArr[kk][i] = a;
            }
            mtArr[kk] = mtl;
            kk++;
        }
            //usemtl
        else if (l_line[0] == 'u' && l_line[1] == 's') {
            istringstream ins(l_line);
            ins >> s1 >> mtl;
        }
    }
    infile.close();
}

ObjLoader::~ObjLoader() {
    printf("ObjLoader release\n");
    for (int i = 0; i < vCount; i++)
        delete[] vArr[i];
    for (int i = 0; i < vnCount; i++)
        delete[] vnArr[i];
    for (int i = 0; i < vtCount; i++)
        delete[] vtArr[i];
    for (int i = 0; i < faceCount; i++) {
        delete[] fvArr[i];
        delete[] ftArr[i];
        delete[] fnArr[i];
    }
    delete[] vArr;
    delete[] vnArr;
    delete[] vtArr;
    delete[] fvArr;
    delete[] ftArr;
    delete[] fnArr;
    delete[] mtArr;

    delete mtlLoader;
    mtlLoader = nullptr;
}

