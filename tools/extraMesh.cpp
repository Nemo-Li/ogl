//
// Created by Nemo li on 2023/12/5.
//
#include <cstdio>
#include "fbxsdk.h"
#include "extra_mesh/Debug.h"
#include "extra_mesh/Common/Common.h"

std::string src_file_path;

int LogSceneCheckError(FbxImporter *mImporter, FbxArray<FbxString *> &details);

int main(int argc, char **argv) {
    std::printf("hello nemo!");
    Debug::init();

    src_file_path = "../../res/models/juese_daiji.fbx";

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