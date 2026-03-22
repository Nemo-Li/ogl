# Edu-OpenGL Engine

Edu-OpenGL 是一个基于 C++17 和 OpenGL 4.1 的轻量级、模块化、组件化游戏引擎框架。它旨在作为一个教学和实验平台，展示现代图形引擎的核心架构设计、脚本集成以及高级渲染技术。

## **项目架构设计**

项目采用了类似 Unity 的 **ECS (Entity-Component-System)** 风格设计，并结合 **Scene Graph (场景图)** 来管理复杂的对象层级和空间变换。

### **1. 核心分层架构**
- **Framework Layer (框架层)**: 封装了底层窗口管理 (GLFW)、输入处理 (Input System)、时间管理 (Time) 以及应用生命周期 (Application)。
- **Engine Layer (引擎核心)**: 
    - `GameObject`: 场景中的基本实体，继承自 `Tree::Node` 以支持层级结构。
    - `Component`: 行为逻辑的载体，支持 `Update`、`Awake` 等生命周期钩子。
    - `Transform`: 每个 GameObject 必备的组件，负责空间位姿（位置、旋转、缩放）及其父子级变换同步。
- **Renderer Layer (渲染层)**: 
    - 封装了 OpenGL 状态机，提供 `Shader`、`Material`、`Texture`、`FrameBuffer` 等抽象。
    - 实现了多种渲染器：`MeshRenderer` (通用模型)、`SkinnedMeshRenderer` (骨骼动画)、`InstanceRender` (实例化渲染)、`Terrain` (地形)、`Grass` (草地)。
- **Scripting Layer (脚本层)**: 利用 Lua 和 Sol2 实现了深度的 C++/Lua 绑定，允许在 Lua 中编写游戏逻辑。
- **Resource Layer (资源层)**: 通过 `Assimp` 加载多种模型格式，`MaterialManager` 统一管理材质资源。

### **2. 关键技术点分析**
- **运行时反射 (RTTR)**: 使用 RTTR 库实现 C++ 类的运行时类型信息获取，使得组件的动态查找和序列化变得更加高效和灵活。
- **场景图管理**: `GameObject` 继承自 `Tree::Node`，实现了父子级变换的自动传递，简化了复杂场景的构建。
- **骨骼动画系统**: 集成了 Assimp 动画解析，支持双线性插值骨骼混合和硬件蒙皮（Vertex Shader 实现）。
- **材质系统**: 抽象出 `Material` 和 `Shader` 类，支持属性绑定和多 Pass 概念（如阴影贴图生成）。
- **高级特效支持**: 集成了 **Effekseer** 粒子引擎，支持复杂的视觉特效表现。
- **Lua 深度绑定**: 通过 `Sol2` 暴露了引擎核心 API 到 Lua，支持在脚本中创建对象、挂载组件和控制逻辑。

---

## **技术栈**

| 类别 | 技术/库 | 说明 |
| :--- | :--- | :--- |
| **编程语言** | C++17 / Lua 5.4 | 核心引擎采用 C++，逻辑脚本采用 Lua |
| **图形 API** | OpenGL 4.1 (Core) | 兼容 macOS 的现代 OpenGL 版本 |
| **窗口管理** | GLFW 3.3 | 处理窗口、上下文及跨平台输入 |
| **图形加载** | GLAD | 动态加载 OpenGL 函数指针 |
| **数学运算** | GLM | 高性能 3D 数学库（向量、矩阵、四元数） |
| **模型加载** | Assimp | 支持 FBX, OBJ, COLLADA 等多种 3D 格式 |
| **脚本集成** | Sol2 | 现代 C++ 风格的 Lua 绑定库 |
| **类型反射** | RTTR | 提供强大的运行时类型检查和属性访问 |
| **粒子特效** | Effekseer | 专业级粒子特效引擎集成 |
| **XML 解析** | RapidXML | 用于配置文件和资源描述解析 |

---

## **后续开发改进建议**

作为架构师，针对当前项目的教学性质和技术现状，提出以下改进建议：

### **1. 渲染管线优化 (Rendering Pipeline)**
- **渲染队列 (Render Queue)**: 目前渲染是简单的遍历。建议引入渲染队列，按材质、距离对物体进行排序（不透明物体从前到后，透明物体从后到前），以减少 Overdraw。
- **合批技术 (Batching)**: 实现静态合批 (Static Batching) 和动态合批 (Dynamic Batching)，大幅减少 Draw Call。
- **延迟渲染 (Deferred Shading)**: 增加对延迟渲染管线的支持，以便高效处理大量光源。

### **2. 资源管理增强 (Resource Management)**
- **异步加载**: 目前资源加载在主线程完成，建议引入异步资源加载队列，防止加载大模型时界面卡顿。
- **引用计数**: 进一步完善资源的自动生命周期管理，防止内存泄漏。

### **3. 架构解耦与扩展**
- **事件系统 (Event System)**: 建立全局事件总线，解耦组件间的通信。
- **多线程化 (Job System)**: 将动画计算、物理更新等独立任务放入多线程执行，充分利用多核 CPU。

### **4. 调试与工具链**
- **集成 ImGui**: 引入 Dear ImGui 编写一个简单的编辑器面板，用于实时查看场景树、修改材质参数和监控性能指标。
- **脚本热重载**: 实现 Lua 脚本的热重载功能，极大提升开发迭代效率。

---

## **项目目录说明**
- `engine/`: C++ 引擎核心源码。
- `engine_lua/`: 引擎的 Lua 逻辑封装。
- `res/`: 核心资源目录（Shader, Texture, Model, Material）。
- `example/`: C++ 与 Lua 的使用示例代码。
- `thirdparty/`: 第三方依赖库。