//
// Created by Nemo li on 2023/12/5.
//
#include <cstdio>
#include <boost/filesystem.hpp>
#include <fstream>
#include "fbxsdk.h"
#include "../engine/helpers/debug.h"
#include "extra_mesh/Common/Common.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

namespace Engine {
    //顶点
    struct Vertex {
        glm::vec3 position_;
        glm::vec3 color_;
        glm::vec2 uv_;
    };

    //Mesh文件头
    struct MeshFileHead {
        //文件类型文件头
        char type_[4];
        //名字
        char name_[32];
        //顶点个数
        unsigned short vertex_num_;
        //索引个数
        unsigned short vertex_index_num_;
    };

    struct MeshFile {
        MeshFileHead head_;
        Vertex *vertex_;
        unsigned short *index_;

        MeshFile() {
            vertex_ = nullptr;
            index_ = nullptr;
        }

        ~MeshFile() {
            if (vertex_ != nullptr) {
                delete[] vertex_;
                vertex_ = nullptr;
            }
            if (index_ == nullptr) {
                delete[] index_;
            }
        }

        void write(std::string filePath) {
            std::ofstream file(filePath, std::ios::binary);
            if (file.is_open()) {
                file.write(reinterpret_cast<char *>(&head_), sizeof(head_));
                file.write(reinterpret_cast<char *>(vertex_), sizeof(Vertex) * head_.vertex_num_);
                file.write(reinterpret_cast<char *>(index_), sizeof(unsigned short) * head_.vertex_index_num_);
                file.close();
            }
        }
    };
}

void ParseNode(FbxNode *pNode);

void ParseMesh(const FbxMesh *pMesh);

std::string src_file_path;

int LogSceneCheckError(FbxImporter *mImporter, FbxArray<FbxString *> &details);

int main(int argc, char **argv) {
    std::printf("hello nemo!");
    Debug::Init();

    src_file_path = "../../res/model/juese_daiji.fbx";

    if (argc > 1) {
        src_file_path = argv[1];
    }
    DEBUG_LOG_INFO("src_file_name:{}", src_file_path);

    FbxManager *mSdkManager;
    FbxScene *mScene;
    FbxImporter *mImporter;

    // 初始化FBX SDKManager，并创建一个Scene，用来容纳从FBX中解析的所有对象
    InitializeSdkObjects(mSdkManager, mScene);
    if (!mSdkManager) {
        DEBUG_LOG_ERROR("Failed to create FBX SDK manager.");
        return -1;
    }
    // 创建一个Importer，用来解析FBX文件
    int lFileFormat = -1;
    mImporter = FbxImporter::Create(mSdkManager, "");
    if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(src_file_path.c_str(), lFileFormat)) {
        // 未能识别文件格式
        DEBUG_LOG_ERROR("Unrecognizable file format.");
        return -1;
    }

    // 初始化Importer，设置文件路径
    if (!mImporter->Initialize(src_file_path.c_str(), lFileFormat)) {
        DEBUG_LOG_ERROR("Call to FbxImporter::Initialize() failed.Error reported: {}",
                        mImporter->GetStatus().GetErrorString());
        return -1;
    }

    // 将FBX文件解析导入到Scene中
    bool lResult = mImporter->Import(mScene);
    if (!lResult) {
        DEBUG_LOG_ERROR("Call to FbxImporter::Import() failed.Error reported: {}",
                        mImporter->GetStatus().GetErrorString());
    }

    // 检查Scene完整性
    FbxStatus status;
    FbxArray<FbxString *> details;
    FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(mScene), &status, &details);
    bool lNotify = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) ||
                   (mImporter->GetStatus().GetCode() != FbxStatus::eSuccess);
    //输出错误信息
    if (lNotify) {
        return LogSceneCheckError(mImporter, details);
    }

    // 转换坐标系为右手坐标系。
    FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    if (SceneAxisSystem != OurAxisSystem) {
        OurAxisSystem.ConvertScene(mScene);
    }

    // 转换单元长度
    FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
    if (SceneSystemUnit.GetScaleFactor() != 1.0) {
        // 例子中用的是厘米，所以这里也要转换
        FbxSystemUnit::cm.ConvertScene(mScene);
    }

    // 转换曲面到三角形
    FbxGeometryConverter lGeomConverter(mSdkManager);
    try {
        lGeomConverter.Triangulate(mScene, /*replace*/true);
    } catch (std::runtime_error &error) {
        DEBUG_LOG_ERROR("Scene integrity verification failed.\n");
        return -1;
    }

    // 递归解析节点
    ParseNode(mScene->GetRootNode());

    DEBUG_LOG_INFO("extra mesh success.press any key exit.");

    getchar();
    return 0;
}

int LogSceneCheckError(FbxImporter *mImporter, FbxArray<FbxString *> &details) {
    ("\n");
    ("********************************************************************************\n");
    if (details.GetCount()) {
        DEBUG_LOG_ERROR("Scene integrity verification failed with the following errors:\n");

        for (int i = 0; i < details.GetCount(); i++)
            DEBUG_LOG_ERROR ("   %s\n", details[i]->Buffer());

        FbxArrayDelete<FbxString *>(details);
    }

    if (mImporter->GetStatus().GetCode() != FbxStatus::eSuccess) {
        DEBUG_LOG_ERROR("\n");
        DEBUG_LOG_ERROR("WARNING:\n");
        DEBUG_LOG_ERROR("   The importer was able to read the file but with errors.\n");
        DEBUG_LOG_ERROR("   Loaded scene may be incomplete.\n\n");
        DEBUG_LOG_ERROR("   Last error message:'%s'\n", mImporter->GetStatus().GetErrorString());
    }

    ("********************************************************************************\n");
    ("\n");
    return -1;
}

/// 递归解析节点
/// @param pNode 节点
void ParseNode(FbxNode *pNode) {
    FbxNodeAttribute *lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute) {
        //节点是Mesh
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
            FbxMesh *lMesh = pNode->GetMesh();
            if (lMesh && !lMesh->GetUserDataPtr()) {
                ParseMesh(lMesh);
            }
        }
    }

    int count = pNode->GetChildCount();
    for (int i = 0; i < count; ++i) {
        ParseNode(pNode->GetChild(i));
    }
}

/// 解析Mesh
/// @param pMesh Mesh 对象
void ParseMesh(const FbxMesh *pMesh) {
    FbxNode *lNode = pMesh->GetNode();
    if (!lNode) {
        DEBUG_LOG_ERROR("Mesh has no node.");
        return;
    }
    DEBUG_LOG_INFO("Mesh name: {}", lNode->GetName());

    // 获取Mesh多边形个数，对游戏来说就是三角形面数。
    const int lPolygonCount = pMesh->GetPolygonCount();
    // 是否有UV数据？
    bool mHasUV = pMesh->GetElementUVCount() > 0;
    bool mAllByControlPoint = true;
    FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
    if (mHasUV) {
        lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
        if (lUVMappingMode == FbxGeometryElement::eNone) {
            mHasUV = false;
        }
        if (mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint) {
            mAllByControlPoint = false;
        }
    }

    // 最终顶点个数到底是多少？
    // 如果只有一套UV，即UV映射方式是按实际顶点个数(FbxGeometryElement::eByControlPoint)，那么就是实际顶点个数。
    // 如果有多套UV，那么一个顶点在不同的多边形里，会对应不同的UV坐标，即UV映射方式是按多边形(eByPolygonVertex)，那么顶点个数是多边形数*3.
    int lPolygonVertexCount = mAllByControlPoint?pMesh->GetControlPointsCount():lPolygonCount * 3;
    // 创建数组存放所有顶点坐标。
    float * lVertices = new float[lPolygonVertexCount * 3];
    // 创建数组存放索引数据，数组长度=面数*3.
    unsigned short * lIndices = new unsigned short[lPolygonCount * 3];
    // 获取多套UV名字
    float * lUVs = nullptr;
    FbxStringList lUVNames;
    pMesh->GetUVSetNames(lUVNames);
    const char * lUVName = nullptr;
    if (mHasUV && lUVNames.GetCount()) {
        // 创建数组存放UV数据
        lUVs = new float[lPolygonVertexCount * 2];
        // 暂时只使用第一套UV。
        lUVName = lUVNames[0];
    }
    FbxVector4 *lControlPoints = pMesh->GetControlPoints();

    // 遍历所有三角面，遍历每个面的三个顶点，解析顶点坐标、UV坐标数据。
    int lVertexCount = 0;
    for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
        // 三角面，3个顶点
        for (int lVertexIndex = 0; lVertexIndex < 3; ++lVertexIndex) {
            // 传入面索引，以及当前面的第几个顶点，获取顶点索引。
            const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVertexIndex);
            if (lControlPointIndex >= 0) {
                // 因为设定一个顶点有多套UV，所以每个三角面与其他面相邻的共享的顶点，尽管实际上是同一个点(ControlPoint),因为有不同的UV，所以还是算不同的顶点。
                lIndices[lVertexCount] = static_cast<unsigned short>(lVertexCount);
                // 获取当前顶点索引对应的实际顶点。
                FbxVector4 lCurrentVertex = lControlPoints[lControlPointIndex];
                // 将顶点坐标从FbxVector4转为float数组
                lVertices[lVertexCount * 3] = static_cast<float>(lCurrentVertex[0]);
                lVertices[lVertexCount * 3 + 1] = static_cast<float>(lCurrentVertex[1]);
                lVertices[lVertexCount * 3 + 2] = static_cast<float>(lCurrentVertex[2]);

                if (mHasUV) {
                    // 获取当前顶点在指定UV层的UV坐标，前面说过，一个顶点可能有多套UV。
                    bool lUnmappedUV;
                    FbxVector2 lCurrentUV;
                    pMesh->GetPolygonVertexUV(lPolygonIndex, lVertexIndex, lUVName, lCurrentUV, lUnmappedUV);
                    // 将UV坐标从FbxVector2转为float数组
                    lUVs[lVertexCount * 2] = static_cast<float>(lCurrentUV[0]);
                    lUVs[lVertexCount * 2 + 1] = static_cast<float>(lCurrentUV[1]);
                }
            }
            ++lVertexCount;
        }
    }


    // 创建引擎Mesh文件，从FBX中解析数据填充到里面。
    Engine::MeshFile meshFile;
    // 构造引擎Mesh结构，设置文件头
    strcpy(meshFile.head_.type_, "Mesh");
    strcpy(meshFile.head_.name_, lNode->GetName());
    meshFile.head_.vertex_num_ = lVertexCount;
    meshFile.head_.vertex_index_num_ = lVertexCount;
    meshFile.vertex_ = new Engine::Vertex[meshFile.head_.vertex_num_];
    meshFile.index_ = new unsigned short[meshFile.head_.vertex_index_num_];
    // 填充顶点坐标、color、UV坐标。
    for (int i = 0; i < lVertexCount; ++i) {
        meshFile.vertex_[i].position_ = glm::vec3(lVertices[i * 3], lVertices[i * 3 + 1], lVertices[i * 3 + 2]);
        meshFile.vertex_[i].color_ = glm::vec4(1.0f);
        meshFile.vertex_[i].uv_ = glm::vec2(lUVs[i * 2], lUVs[i * 2 + 1]);
    }
    // 填充索引
    meshFile.index_ = lIndices;

    // 写入文件
    boost::filesystem::path path(src_file_path);
    std::string src_file_name = path.filename().stem().string();
    std::string dst_file_name = fmt::format("{}_{}.mesh", src_file_name, meshFile.head_.name_);
    path = path.remove_filename().append(dst_file_name);
    DEBUG_LOG_INFO("mesh path {}", path.string());
    meshFile.write(path.string());
}