//
// Created by Nemo li on 2023/12/13.
//

#ifndef OGL_RENDER_TASK_CONSUMER_BASE_H
#define OGL_RENDER_TASK_CONSUMER_BASE_H

#include <thread>

class RenderTaskBase;

/// 渲染任务消费端
/// 消费端代理基类
class RenderTaskConsumerBase {
public:
    RenderTaskConsumerBase() {}

    virtual ~RenderTaskConsumerBase() {}

    virtual void Init();

    /// 渲染相关的API调用需要放到渲染线程中。
    virtual void InitGraphicsLibraryFramework();

    virtual void Exit();

    virtual void GetFramebufferSize(int &width, int &height);

    virtual void SwapBuffer();

private:
    /// 线程主函数：死循环处理渲染任务
    void ProcessTask();

    /// 更新游戏画面尺寸
    /// \param task_base
    void UpdateScreenSize(RenderTaskBase *task_base);

    /// 编译、链接Shader
    /// \param task_base
    void CompileShader(RenderTaskBase *task_base);

    /// 使用Shader程序
    void UseShaderProgram(RenderTaskBase *task_base);

    /// 创建普通纹理
    /// \param task_base
    void CreateTexImage2D(RenderTaskBase *task_base);


    /// 删除Textures
    /// \param task_base
    void DeleteTextures(RenderTaskBase *task_base);

    /// 局部更新纹理
    /// \param task_base
    void UpdateTextureSubImage2D(RenderTaskBase *task_base);

    /// 创建VAO
    /// \param task_base
    void CreateVAO(RenderTaskBase *task_base);

    /// 更新VBO
    /// \param task_base
    void UpdateVBOSubData(RenderTaskBase *task_base);

    /// 设置状态,开启或关闭
    /// \param task_base
    void SetEnableState(RenderTaskBase *task_base);

    /// 设置混合函数
    /// \param task_base
    void SetBlendFunc(RenderTaskBase *task_base);

    /// 上传uniform矩阵
    /// \param task_base
    void SetUniformMatrix4fv(RenderTaskBase *task_base);

    /// 激活并绑定纹理
    /// \param task_base
    void ActiveAndBindTexture(RenderTaskBase *task_base);

    /// 上传1个int值
    /// \param task_base
    void SetUniform1i(RenderTaskBase *task_base);

    /// 上传1个float值
    /// \param task_base
    void SetUniform1f(RenderTaskBase *task_base);

    /// 上传1个vec3
    /// \param task_base
    void SetUniform3f(RenderTaskBase *task_base);

    /// 绑定VAO并绘制
    /// \param task_base
    void BindVAOAndDrawElements(RenderTaskBase *task_base);

    /// 设置clear_flag并且清除颜色缓冲
    /// \param task_base
    void SetClearFlagAndClearColorBuffer(RenderTaskBase *task_base);

    /// 设置模板测试函数
    void SetStencilFunc(RenderTaskBase *task_base);

    /// 设置模板操作
    void SetStencilOp(RenderTaskBase *task_base);

    /// 设置清除模板缓冲值
    void SetStencilBufferClearValue(RenderTaskBase *task_base);

    /// 结束一帧
    /// \param task_base
    void EndFrame(RenderTaskBase *task_base);

private:
    std::thread render_thread_;//渲染线程
    bool exit_ = false;
};

#endif //OGL_RENDER_TASK_CONSUMER_BASE_H
