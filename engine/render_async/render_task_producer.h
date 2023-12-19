//
// Created by Nemo li on 2023/12/12.
//

#ifndef OGL_RENDER_TASK_PRODUCER_H
#define OGL_RENDER_TASK_PRODUCER_H

#include <glm/glm.hpp>

/// 渲染任务生产者
class RenderTaskProducer {

public:
    /// 发出任务：更新游戏窗口尺寸
    static void ProduceRenderTaskUpdateScreenSize();

    /// 发出任务：编译Shader
    /// \param vertex_shader_source 顶点shader源码
    /// \param fragment_shader_source 片段shader源码
    /// \param shader_program_handle Shader程序句柄
    static void ProduceRenderTaskCompileShader(const char *vertex_shader_source, const char *fragment_shader_source,
                                               unsigned int shader_program_handle);

    /// 发出任务：使用Shader程序
    static void ProduceRenderTaskUseShaderProgram(unsigned int shader_program_handle);

    /// 发出任务：创建纹理
    /// \param texture_handle
    /// \param width
    /// \param height
    /// \param gl_texture_format
    /// \param client_format
    /// \param data_type
    /// \param data_size
    /// \param data
    static void ProduceRenderTaskCreateTexImage2D(unsigned int texture_handle, int width, int height,
                                                  unsigned int gl_texture_format, unsigned int client_format,
                                                  unsigned int data_type, unsigned int data_size, unsigned char *data);

    /// 发出任务：删除一个或多个Texture
    /// \param size
    /// \param texture_handle_array 指定要删除的Texture的句柄数组，注意函数里是拷贝内存块。
    static void ProduceRenderTaskDeleteTextures(int size, unsigned int *texture_handle_array);

    /// 发出任务：局部更新Texture
    /// \param texture_handle 纹理句柄
    /// \param x
    /// \param y
    /// \param width
    /// \param height
    /// \param client_format
    /// \param data_type
    /// \param data 指定要更新的数据，注意函数里是拷贝内存块。
    /// \param data_size
    static void
    ProduceRenderTaskUpdateTextureSubImage2D(unsigned int texture_handle, int x, int y, int width, int height,
                                             unsigned int client_format, unsigned int data_type,
                                             unsigned char *data, unsigned int data_size);

    /// 发出任务：创建VAO
    /// \param shader_program_handle
    /// \param vao_handle
    /// \param vertex_data_size
    /// \param vertex_data_stride
    /// \param vertex_data
    /// \param vertex_index_data_size
    /// \param vertex_index_data
    static void
    ProduceRenderTaskCreateVAO(unsigned int shader_program_handle, unsigned int vao_handle, unsigned int vbo_handle,
                               unsigned int vertex_data_size, unsigned int vertex_data_stride, void *vertex_data,
                               unsigned int vertex_index_data_size, void *vertex_index_data);

    /// 发出任务：更新VBO
    /// \param vbo_handle
    /// \param vertex_data_size
    /// \param vertex_data
    static void
    ProduceRenderTaskUpdateVBOSubData(unsigned int vbo_handle, unsigned int vertex_data_size, void *vertex_data);

    /// 发出任务：设置状态,开启或关闭
    static void ProduceRenderTaskSetEnableState(unsigned int state, bool enable);

    /// 发出任务：设置混合函数
    /// \param source_blending_factor 源混合因子
    /// \param destination_blending_factor 目标混合因子
    static void
    ProduceRenderTaskSetBlenderFunc(unsigned int source_blending_factor, unsigned int destination_blending_factor);

    /// 发出任务：设置4x4矩阵
    /// \param shader_program_handle
    /// \param uniform_name
    /// \param transpose
    /// \param value
    /// \param value_data_size
    static void
    ProduceRenderTaskSetUniformMatrix4fv(unsigned int shader_program_handle, const char *uniform_name, bool transpose,
                                         glm::mat4 &matrix);

    /// 激活并绑定纹理
    /// \param texture_uint
    /// \param texture_handle
    static void ProduceRenderTaskActiveAndBindTexture(unsigned int texture_uint, unsigned int texture_handle);

    /// 上传1个int值
    /// \param shader_program_handle
    /// \param uniform_name
    /// \param value
    static void ProduceRenderTaskSetUniform1i(unsigned int shader_program_handle, const char *uniform_name, int value);

    /// 绑定VAO并绘制
    /// \param vao_handle
    /// \param inex_count
    static void ProduceRenderTaskBindVAOAndDrawElements(unsigned int vao_handle, unsigned int vertex_index_num);

    static void ProduceRenderTaskBindVAOAndDrawElementsStrip(unsigned int vao_handle, int numTrisPerStrip, int numStrips);

    /// 设置clear_flag并且清除颜色缓冲
    /// \param clear_flag
    /// \param clear_color_r
    /// \param clear_color_g
    /// \param clear_color_b
    /// \param clear_color_a
    static void
    ProduceRenderTaskSetClearFlagAndClearColorBuffer(unsigned int clear_flag, float clear_color_r, float clear_color_g,
                                                     float clear_color_b, float clear_color_a);

    /// 设置模板测试函数
    /// \param stencil_func
    /// \param stencil_ref
    /// \param stencil_mask
    static void ProduceRenderTaskSetStencilFunc(unsigned int stencil_func, int stencil_ref, unsigned int stencil_mask);

    /// 设置模板操作
    /// \param fail_op_
    /// \param z_test_fail_op_
    /// \param z_test_pass_op_
    static void
    ProduceRenderTaskSetStencilOp(unsigned int fail_op_, unsigned int z_test_fail_op_, unsigned int z_test_pass_op_);

    /// 设置清除模板缓冲值
    static void ProduceRenderTaskSetStencilBufferClearValue(int clear_value);


    /// 发出特殊任务：渲染结束
    static void ProduceRenderTaskEndFrame();
};


#endif //OGL_RENDER_TASK_PRODUCER_H
