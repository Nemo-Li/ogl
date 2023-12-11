//
// Created by Nemo li on 2023/12/11.
//

#define GLFW_INCLUDE_NONE

#include <filesystem>
#include "fbxsdk.h"
#include "Common/Common.h"
#include "helpers/debug.h"
#include "glm/glm.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

namespace Engine {
    //顶点
    struct Vertex {
        glm::vec3 position_;
        glm::vec4 color_;
        glm::vec2 uv_;
    };

    //Mesh文件头
    struct MeshFileHead {
        char type_[4];//文件类型文件头
        char name_[32];//名字
        unsigned short vertex_num_;//顶点个数
        unsigned short vertex_index_num_;//索引个数
    };

    //Mesh文件
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
            if (index_ != nullptr) {
                delete[] index_;
                index_ = nullptr;
            }
        }

        // 写入文件
        void Write(const char *filePath) {
            std::ofstream file(filePath, std::ios::binary);
            if (file.is_open()) {
                file.write(reinterpret_cast<char *>(&head_), sizeof(head_));
                file.write(reinterpret_cast<char *>(vertex_), sizeof(Vertex) * head_.vertex_num_);
                file.write(reinterpret_cast<char *>(index_), sizeof(unsigned short) * head_.vertex_index_num_);
                file.close();
            }
        }
    };

    class Animation {
    public:
        std::string name_;
        unsigned short frame_count_;//帧数
        unsigned short frame_per_second_;//帧率

        // 存储骨骼名字 [bone_name , bone_name , ...]
        std::vector<std::string> bone_name_vec_;

        // 存储骨骼关键帧数据，外面数组下标表示第几帧，里面数组下表表示第几个Bone [["Bone.Matrix","Bone.001.Matrix"] , ["Bone.Matrix","Bone.001.Matrix"]]
        std::vector<std::vector<glm::mat4>> frame_bones_matrix_vec_;
    public:
        Animation() {
            frame_count_ = 0;
            frame_per_second_ = 0;
        }

        /// 获取骨骼index
        int get_bone_index(std::string bone_name) {
            for (int i = 0; i < bone_name_vec_.size(); ++i) {
                if (bone_name_vec_[i] == bone_name) {
                    return i;
                }
            }
            return -1;
        }

        std::string ReplaceChar(std::string file_name, char old_char, char new_char) {
            if (file_name.find(old_char) != std::string::npos) {
                DEBUG_LOG_WARN("Animation::Write file_name:{} contains invalid char,will replace with _",
                               file_name.c_str());
                std::replace(file_name.begin(), file_name.end(), old_char, new_char);
            }
            return file_name;
        }

        // 写入文件
        void Write(std::string filePath) {
            // 路径转小写
            std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
            // 替换文件名中的空格为_
            boost::filesystem::path path(filePath);
            std::string file_name = path.filename().string();
            // 判断文件名中是否存在空格、|，替换为_
            file_name = ReplaceChar(file_name, ' ', '_');
            file_name = ReplaceChar(file_name, '|', '_');
            path.remove_filename().append(file_name);
            filePath = path.string();

            // 创建文件夹
            if (boost::filesystem::exists(path) == false) {
                DEBUG_LOG_INFO("{} not exist,will create.", filePath);
                if (path.has_filename() == false) {
                    DEBUG_LOG_ERROR("{} is not correct file path.", filePath);
                    assert(false);
                }
                auto dir_path = path.parent_path();
                if (boost::filesystem::exists(dir_path) == false) {
                    boost::system::error_code error_cord;
                    if (boost::filesystem::create_directories(dir_path, error_cord) == false) {
                        DEBUG_LOG_ERROR("{} not exist,create failed.", filePath);
                        assert(false);
                    }
                }
            }
            // 写入文件
            std::ofstream file(filePath, std::ios::binary);
            if (file.is_open() == false) {
                DEBUG_LOG_ERROR("file open failed:{}", filePath);
                assert(false);
                return;
            }
            // 写入文件头 skeleton_anim
            std::string type = "skeleton_anim";
            file.write(type.c_str(), type.size());
            // 写入名字长度
            unsigned short name_len = name_.size();
            file.write(reinterpret_cast<char *>(&name_len), sizeof(unsigned short));
            // 写入名字
            file.write(name_.c_str(), name_.size());
            // 写入帧数
            file.write(reinterpret_cast<char *>(&frame_count_), sizeof(frame_count_));
            // 写入帧率
            file.write(reinterpret_cast<char *>(&frame_per_second_), sizeof(frame_per_second_));
            // 写入骨骼数量
            unsigned short bone_num = bone_name_vec_.size();
            file.write(reinterpret_cast<char *>(&bone_num), sizeof(unsigned short));
            // 写入骨骼名字
            for (int i = 0; i < bone_name_vec_.size(); ++i) {
                // 写入长度
                unsigned short bone_name_len = bone_name_vec_[i].size();
                file.write(reinterpret_cast<char *>(&bone_name_len), sizeof(unsigned short));
                // 写入名字
                file.write(bone_name_vec_[i].c_str(), bone_name_vec_[i].size());
            }
            // 写入骨骼关键帧数据
            for (int frame_index = 0; frame_index < frame_bones_matrix_vec_.size(); ++frame_index) {
                for (int bone_index = 0; bone_index < frame_bones_matrix_vec_[frame_index].size(); ++bone_index) {
                    glm::mat4 bone_matrix = frame_bones_matrix_vec_[frame_index][bone_index];
                    file.write(reinterpret_cast<char *>(&bone_matrix), sizeof(glm::mat4));
                }
            }
            file.close();
        }
    };
}

int InitFbxSDK(const char *pFileName);

/// transform FBxAMatrix to glm::mat4
glm::mat4 FbxMatrixToGlmMat4(FbxAMatrix &fbxAMatrix) {
    glm::mat4 glm_matrix;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            glm_matrix[i][j] = fbxAMatrix[i][j];
        }
    }
    return glm_matrix;
}

void ComputeClusterDeformation(FbxAMatrix &pGlobalPosition, FbxMesh *pMesh, FbxCluster *pCluster,
                               FbxAMatrix &pVertexTransformMatrix, FbxTime pTime);

void ParseNodeRecursive(FbxNode *pNode);

FbxManager *mSdkManager;
FbxScene *mScene;
FbxImporter *mImporter;
FbxAnimStack *mCurrentAnimationStack;
FbxTime mCurrentTime;
FbxTime mStart, mStop;//动画片段开始结束时间。

std::string src_file_path;

int main(int argc, char **argv) {
    Debug::Init();

    src_file_path = "../../res/model/fbx_extra.fbx";

    if (argc > 1) {
        src_file_path = argv[1];
    }
    DEBUG_LOG_INFO("src_file_name:{}", src_file_path);

    InitFbxSDK(src_file_path.c_str());

    // 获取所有的动画片段
    FbxArray<FbxString *> mAnimStackNameArray;
    mScene->FillAnimStackNameArray(mAnimStackNameArray);
    // 动画片段个数
//    const int lAnimStackCount = mAnimStackNameArray.GetCount();

    // 选择一个动画片段。
    int pIndex = 0;
    mCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
    if (mCurrentAnimationStack == NULL) {
        DEBUG_LOG_ERROR("No animation stack found,name:{}", mAnimStackNameArray[pIndex]->Buffer());
        return -1;
    }
    // 设置当前Animation Stack
    mScene->SetCurrentAnimationStack(mCurrentAnimationStack);
    // 获取动画片段的时间范围
    FbxTakeInfo *lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
    if (lCurrentTakeInfo) {
        mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    }
    FbxAMatrix lDummyGlobalPosition;
    ParseNodeRecursive(mScene->GetRootNode());

    DEBUG_LOG_INFO("extra animation success.press any key exit.");

    getchar();

    return 0;
}

int InitFbxSDK(const char *pFileName) {
    // 初始化FBX SDKManager，并创建一个Scene，用来容纳从FBX中解析的所有对象
    InitializeSdkObjects(mSdkManager, mScene);
    if (!mSdkManager) {
        DEBUG_LOG_ERROR("Failed to create FBX SDK manager.");
        return -1;
    }
    // 创建一个Importer，用来解析FBX文件
    int lFileFormat = -1;
    mImporter = FbxImporter::Create(mSdkManager, "");
    if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(pFileName, lFileFormat)) {
        // 未能识别文件格式
        DEBUG_LOG_ERROR("Unrecognizable file format.");
        return -1;
    }

    // 初始化Importer，设置文件路径
    if (mImporter->Initialize(pFileName, lFileFormat) == false) {
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
        DEBUG_LOG_ERROR("\n");
        DEBUG_LOG_ERROR("********************************************************************************\n");
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

        DEBUG_LOG_ERROR("********************************************************************************\n");
        DEBUG_LOG_ERROR("\n");
        return -1;
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
    } catch (std::runtime_error) {
        DEBUG_LOG_ERROR("Scene integrity verification failed.\n");
        return -1;
    }
    return 0;
}

/// 从Mesh节点获取蒙皮修改器，获取所有Cluster。遍历帧数，获取每一帧每个Cluster的移动旋转矩阵数据。
/// \param pNode Mesh节点
void ParseNodeRecursive(FbxNode *pNode) {
    auto name = pNode->GetName();
    DEBUG_LOG_INFO("node name: {}", name);
    FbxNodeAttribute *lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute) {
        // 获取pNode相对于锚点的offset
        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix lGeometryOffset = FbxAMatrix(lT, lR, lS);

        DEBUG_LOG_INFO(" node translation: ({},{},{},{})", lT[0], lT[1], lT[2], lT[3]);
        DEBUG_LOG_INFO(" node rotation: ({},{},{},{})", lR[0], lR[1], lR[2], lR[3]);
        DEBUG_LOG_INFO(" node scaling: ({},{},{},{})", lS[0], lS[1], lS[2], lS[3]);

        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
            FbxMesh *pMesh = pNode->GetMesh();
            // 获取蒙皮数量，一般来说一个Mesh对应一个蒙皮修改器。
//            int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
            Engine::Animation animation;
            animation.name_ = mCurrentAnimationStack->GetName();

            FbxTime::EMode lTimeMode = mScene->GetGlobalSettings().GetTimeMode();
            animation.frame_per_second_ = fbxsdk::FbxTime::GetFrameRate(lTimeMode);

            // 获取蒙皮修改器
            int lSkinIndex = 0;
            FbxSkin *lSkinDeformer = (FbxSkin *) pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
            // 获取蒙皮修改器上的顶点组数量，一般来说就是骨骼数量，绑定的时候，一般是以一个骨骼作为一个顶点组。
            int lClusterCount = lSkinDeformer->GetClusterCount();
            // 遍历骨骼
            for (int lClusterIndex = 0; lClusterIndex < lClusterCount; ++lClusterIndex) {
                // 获取骨骼的顶点组
                FbxCluster *lCluster = lSkinDeformer->GetCluster(lClusterIndex);
                animation.bone_name_vec_.push_back(lCluster->GetName());
            }

            // 每一帧的时间
            FbxTime mFrameTime;
            mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());
            for (FbxTime pTime = mStart; pTime < mStop; pTime += mFrameTime) {
                // 首先获取当前节点的全局坐标
                FbxAMatrix lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
                FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;//相乘获得pNode在当前时间相对原点的坐标。

                std::vector<glm::mat4> one_frame_bone_matrix_vec;//一帧的所有骨骼变换矩阵
                for (int lClusterIndex = 0; lClusterIndex < lClusterCount; ++lClusterIndex) {
                    // 获取骨骼的顶点组
                    FbxCluster *lCluster = lSkinDeformer->GetCluster(lClusterIndex);
                    // 计算这个骨骼的形变，前面pNode是指计算到Mesh节点的形变，而这是是计算骨骼节点，后面会作用到顶点。
                    FbxAMatrix lVertexTransformMatrix;
                    ComputeClusterDeformation(lGlobalOffPosition, pMesh, lCluster, lVertexTransformMatrix, pTime);

                    glm::mat4 bone_matrix = FbxMatrixToGlmMat4(lVertexTransformMatrix);
                    one_frame_bone_matrix_vec.push_back(bone_matrix);
                }
                animation.frame_bones_matrix_vec_.push_back(one_frame_bone_matrix_vec);
            }//lClusterCount

            animation.frame_count_ = animation.frame_bones_matrix_vec_.size();

            // 写入文件
            boost::filesystem::path path(src_file_path);
            std::string src_file_name = path.filename().stem().string();
            std::string dst_file_name = fmt::format("{}_{}.skeleton_anim", src_file_name, animation.name_);
            path.remove_filename().append(dst_file_name);
            animation.Write(path.string());
        }
    }
    // 遍历子节点，递归
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        ParseNodeRecursive(pNode->GetChild(lChildIndex));
    }
}

/// Compute the transform matrix that the cluster will transform the vertex.
/// \param pGlobalPosition 节点在当前帧的全局坐标 几何矩阵
/// \param pMesh Mesh
/// \param pCluster 顶点组，一般是一个 Bone一个顶点组，就当成是一个Bone吧。
/// \param pVertexTransformMatrix 存储顶点形变矩阵
/// \param pTime 当前帧时间
/// \param pPose 当前选择的Pose，应该是指动画片段
void ComputeClusterDeformation(FbxAMatrix &pGlobalPosition, FbxMesh *pMesh, FbxCluster *pCluster,
                               FbxAMatrix &pVertexTransformMatrix, FbxTime pTime) {
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

    if (lClusterMode == FbxCluster::eNormalize) {
        FbxAMatrix lReferenceGlobalInitPosition;
        FbxAMatrix lAssociateGlobalInitPosition;
        FbxAMatrix lAssociateGlobalCurrentPosition;
        FbxAMatrix lClusterGlobalInitPosition;
        FbxAMatrix lClusterGlobalCurrentPosition;

        FbxAMatrix lAssociateGeometry;
        FbxAMatrix lClusterGeometry;

        FbxAMatrix lClusterRelativeInitPosition;
        FbxAMatrix lClusterRelativeCurrentPositionInverse;
        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
        // Multiply lReferenceGlobalInitPosition by Geometric Transformation
        const FbxVector4 lT = pMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
        FbxAMatrix lReferenceGeometry = FbxAMatrix(lT, lR, lS);

        lReferenceGlobalInitPosition *= lReferenceGeometry;

        // Get the link initial global position and the link current global position.
        pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

        lClusterGlobalCurrentPosition = pCluster->GetLink()->EvaluateGlobalTransform(pTime);

        // Compute the initial position of the link relative to the reference.
        lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

        // Compute the current position of the link relative to the reference.
        lClusterRelativeCurrentPositionInverse = pGlobalPosition.Inverse() * lClusterGlobalCurrentPosition;

        // Compute the shift of the link relative to the reference.
        pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
    }
}