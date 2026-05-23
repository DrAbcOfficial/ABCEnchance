# IMetaRenderer 接口完整参考文档

> 生成日期: 2026-05-23  
> 接口版本: `MetaRenderer_API_002`  
> 接口文件: `thirdparty/MetaHookSv/include/Interface/IMetaRenderer.h`

---

## 1. 架构概述

`IMetaRenderer` 是 **ABCEnchance** 与 **MetaHookSV 渲染器插件**之间的核心桥接接口。它包装了底层 OpenGL 渲染管线，提供 framebuffer 管理、shader 编译、2D/3D 绘图、矩阵操作、stencil、blit、VRAM 缓冲区上传等功能。

```
┌─────────────────────────┐      ┌──────────────────────────────┐
│  ABCEnchance (Client)   │      │  MetaHookSV Renderer Plugin  │
│                         │      │  (thirdparty/MetaHookSv/     │
│  MetaRenderer()  ──────────────►  Plugins/Renderer/)          │
│  全局函数                │      │                              │
│                         │      │  CMetaRenderer : IMetaRenderer│
│  g_pMetaRenderer        │      │  gl_rmain.cpp:6749            │
│  静态指针                │      │                              │
└─────────────────────────┘      └──────────────────────────────┘
```

- **接口定义文件**: `thirdparty/MetaHookSv/include/Interface/IMetaRenderer.h`
- **接口版本常量**: `METARENDERER_INTERFACE_VERSION` = `"MetaRenderer_API_002"`
- **实现类**: `CMetaRenderer` 定义于 `thirdparty/MetaHookSv/Plugins/Renderer/gl_rmain.cpp:6749`
- **导出方式**: `EXPOSE_SINGLE_INTERFACE(CMetaRenderer, IMetaRenderer, METARENDERER_INTERFACE_VERSION)` (同文件第 7452 行)
- **引擎限制**: 仅在 SvEngine 上渲染。`src/plugins.cpp:55-59` — 若非 `ENGINE_SVENGINE` 则 `SYS_ERROR()` 致命退出
- **编译目标**: 仅 Win32 (x86)，无 x64 配置
- **语言标准**: C++20, C17
- **字符集**: MultiByte (源文件使用 GBK 编码)

---

## 2. 获取 IMetaRenderer 指针

### 2.1 单例访问函数（推荐）

```cpp
#include <IMetaRenderer.h>

IMetaRenderer* pRenderer = MetaRenderer();
// 注意：初始化之前或关闭之后，返回值可能为 nullptr
```

全局函数声明位于 `IMetaRenderer.h:729`：
```cpp
IMetaRenderer* MetaRenderer();
```

实际实现位于 `src/core/metahook/MetaRenderer.cpp:6-11`：
```cpp
static IMetaRenderer* g_pMetaRenderer;

IMetaRenderer* MetaRenderer()
{
    return g_pMetaRenderer;
}
```

### 2.2 初始化流程

`MetaRenderer_Init()` 位于 `src/core/metahook/MetaRenderer.cpp:13`，调用链：

1. `src/exportfuncs.cpp:269` 中 `GL_Init()` 调用 `MetaRenderer_Init()`
2. `MetaRenderer_Init()` 内部：
   - 通过 `g_pMetaHookAPI->GetPluginInfoByBaseFileName("Renderer", &rendererInfo)` 查找名为 "Renderer" 的插件
   - 获取插件模块句柄 `rendererInfo.PluginModuleBase`
   - 调用 `Sys_GetFactory(g_hMetaRenderer)` 获取工厂函数
   - 调用 `factory(METARENDERER_INTERFACE_VERSION, NULL)` 获取 `IMetaRenderer*`
   - 存储到静态指针 `g_pMetaRenderer`

### 2.3 生命周期约束

- 仅在 `MetaRenderer_Init()` 之后、`MetaRenderer_Shutdown()` 之前可用
- 任何使用者都必须检查 `MetaRenderer() != nullptr`
- 关闭时 `g_hMetaRenderer` 被置 `NULL`，但**从不卸载 DLL**（注释: `"Never unload by ourself!"`）
- 所有 `IMetaRenderer` 调用应在渲染线程中进行

---

## 3. IMetaRenderer 完整 API 参考

### 3.1 交换链信息 (Swap Chain Info)

| 方法 | 签名 | 说明 |
|------|------|------|
| `GetSwapChainWidth` | `unsigned int GetSwapChainWidth() const` | 对应 `glwidth`，返回交换链像素宽度 |
| `GetSwapChainHeight` | `unsigned int GetSwapChainHeight() const` | 对应 `glheight`，返回交换链像素高度 |

---

### 3.2 预分配 FBO 获取 (Pre-built Framebuffers)

以下 FBO 由渲染器预分配，不同格式用于不同渲染阶段。所有尺寸均为 `SwapChainWidth × SwapChainHeight`（除非特别说明）。

| 方法 | 返回类型 | 内部格式 | 说明 |
|------|----------|----------|------|
| `GetFinalBufferFBO()` | `FBO_Container_t*` | RGBA8 D24S8 | 最终输出缓冲区 |
| `GetBackBufferFBO()` | `FBO_Container_t*` | RGBA16F D24S8 | HDR 后备缓冲区 |
| `GetBackBufferFBO2()` | `FBO_Container_t*` | RGBA16F D24S8 | HDR 后备缓冲区 2 |
| `GetBackBufferFBO3()` | `FBO_Container_t*` | RGBA8 D24S8 | 后备缓冲区 3 |
| `GetBackBufferFBO4()` | `FBO_Container_t*` | RGBA8 D24S8 | 后备缓冲区 4 |
| `GetBackBufferFBO5()` | `FBO_Container_t*` | RGBA8 D24S8 | 后备缓冲区 5（最新添加） |
| `GetBlurPassFBO(pass, vertical)` | `FBO_Container_t*` | RGB16F | 模糊 Pass FBO。`pass` 范围 0~2，`vertical` 范围 0~1 |
| `GetDownSampleFBO(pass)` | `FBO_Container_t*` | RGB16F D24S8 | 降采样 FBO。`pass` 范围 0~1 |

**GetBlurPassFBO 尺寸规则**:
- `GetBlurPassFBO(0, *)` = (W / 4) × (H / 4)
- `GetBlurPassFBO(1, *)` = (W / 8) × (H / 8)
- `GetBlurPassFBO(2, *)` = (W / 16) × (H / 16)

**GetDownSampleFBO 尺寸规则**:
- `GetDownSampleFBO(0)` = (W / 2) × (H / 2)
- `GetDownSampleFBO(1)` = (W / 4) × (H / 4)

---

### 3.3 绘图 (Drawing)

#### 3.3.1 基础三角形列表绘制

```cpp
void DrawTexturedRect(
    int gltexturenum,              // OpenGL 纹理 ID
    const texturedrectvertex_t* verticeBuffer,
    size_t verticeCount,
    const uint32_t* indices,
    size_t indicesCount,           // 必须是 3 的倍数
    uint64_t programState,         // 位掩码标志，见 §4.3
    const char* debugMetadata      // RenderDoc/Nsight 调试标签
) = 0;
```

```cpp
void DrawTexturedRectMask(
    int gltexturenum,              // 基础纹理（纹理单元 0）
    int maskTextureId,             // 遮罩纹理（纹理单元 1）
    const texturedrectvertex_t* verticeBuffer,
    size_t verticeCount,
    const uint32_t* indices,
    size_t indicesCount,           // 必须是 3 的倍数
    uint64_t programState,
    const char* debugMetadata
) = 0;
```

```cpp
void DrawFilledRect(
    const filledrectvertex_t* verticeBuffer,
    size_t verticeCount,
    const uint32_t* indices,
    size_t indicesCount,           // 必须是 3 的倍数
    uint64_t programState,
    const char* debugMetadata
) = 0;
```

#### 3.3.2 矩形快捷方式 (Quad Wrappers)

这些是简化封装，内部自动构建 4 顶点 + 6 索引的矩形。

```cpp
// 纹理矩形
void DrawTexturedQuad(
    int gltexturenum,
    int x0, int y0, int x1, int y1,
    const float* color4v,          // RGBA, 各分量 [0.0, 1.0]
    uint64_t programState,
    const char* debugMetadata
) = 0;

// 带遮罩的纹理矩形
void DrawTexturedQuadMask(
    int gltexturenum,
    int maskTextureId,
    int x0, int y0, int x1, int y1,
    const float* color4v,
    uint64_t programState,
    const char* debugMetadata
) = 0;

// 纯色矩形
void DrawFilledQuad(
    int x0, int y0, int x1, int y1,
    const float* color4v,
    uint64_t programState,
    const char* debugMetadata
) = 0;
```

---

### 3.4 Shader 编译 (Shader Compilation)

```cpp
// 基础编译 (VS + FS)
uint32_t CompileShaderFile(
    const char* vsfile,            // 顶点 shader 路径 (来自 GoldSrc FileSystem)
    const char* fsfile,            // 片元 shader 路径
    const char* vsdefine = nullptr, // VS 端宏定义 (可选, 如 "#define FOO 1\n")
    const char* fsdefine = nullptr  // FS 端宏定义 (可选)
) = 0;
// 返回值: OpenGL program ID, 失败返回 0
```

```cpp
// 扩展编译 (支持 Geometry Shader)
uint32_t CompileShaderFileEx(
    const CCompileShaderArgs* args
) = 0;
```

**CCompileShaderArgs 结构体** (定义于 `gl_common.h`):
```cpp
class CCompileShaderArgs
{
public:
    const char* vsfile{};    // 顶点 shader 路径
    const char* gsfile{};    // 几何 shader 路径 (可选)
    const char* fsfile{};    // 片元 shader 路径
    const char* vsdefine{};  // VS 端宏
    const char* gsdefine{};  // GS 端宏
    const char* fsdefine{};  // FS 端宏
};
```

**Shader 文件路径示例**: `"svencoop/abcenchance/shader/GaussianBlur.vs"` — 从 GoldSrc FileSystem 加载

---

### 3.5 PMB RingBuffer (持久映射缓冲)

```cpp
IPMBRingBuffer* CreatePMBRingBuffer(
    const char* name,              // 调试名称
    size_t bufferSize,             // 总缓冲大小（字节）
    int OpenGLBufferTarget         // GL 目标 (GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER 等)
) = 0;
```

**IPMBRingBuffer 接口** (定义于 `thirdparty/MetaHookSv/Plugins/Renderer/gl_ringbuffer.h:15`):
```cpp
class IPMBRingBuffer : public IBaseInterface
{
public:
    virtual void Destroy() = 0;
    virtual bool Allocate(size_t size, CPMBRingBufferAllocation& allocation) = 0;
    virtual void BeginFrame() = 0;    // 每帧开始必须调用
    virtual void EndFrame() = 0;      // 每帧结束、present 之前必须调用
    virtual GLuint GetGLBufferObject() const = 0;
    virtual bool IsEmpty() const = 0;
    virtual bool IsFull() const = 0;
    virtual size_t GetUsedSize() const = 0;
};
```

**CPMBRingBufferAllocation**:
```cpp
class CPMBRingBufferAllocation
{
public:
    void* ptr{};       // 映射指针
    size_t offset{};   // 缓冲区内偏移
    size_t size{};     // 分配大小
    bool valid{};      // 分配是否成功
};
```

---

### 3.6 2D 渲染模式 (2D Mode)

| 方法 | 签名 | 说明 |
|------|------|------|
| `Set2D()` | `void Set2D()` | 设置世界+投影矩阵为全屏 2D 模式 |
| `Set2DEx(x,y,w,h)` | `void Set2DEx(int x, int y, int width, int height)` | 设置指定视口的 2D 模式 |
| `Finish2D()` | `void Finish2D()` | 还原到之前的矩阵状态 |

---

### 3.7 纹理/程序绑定 (Texture & Program Binding)

| 方法 | 签名 | 说明 |
|------|------|------|
| `Bind(tex)` | `void Bind(int gltexturenum)` | 绑定纹理到 GL_TEXTURE0 |
| `BindTextureUnit(unit, target, tex)` | `void BindTextureUnit(int OpenGLTextureUnit, int OpenGLTextureTarget, int gltexturenum)` | 绑定到指定纹理单元，操作后还原回单元 0 |
| `UseProgram(prog)` | `void UseProgram(int glprogram)` | glUseProgram 封装 |

---

### 3.8 FBO 管理 (Framebuffer Management)

| 方法 | 签名 | 说明 |
|------|------|------|
| `SetCurrentSceneFBO(fbo)` | `void SetCurrentSceneFBO(FBO_Container_t* src)` | 设置当前场景 FBO |
| `GetCurrentSceneFBO()` | `FBO_Container_t* GetCurrentSceneFBO()` | 获取当前场景 FBO |
| `GetCurrentRenderingFBO()` | `FBO_Container_t* GetCurrentRenderingFBO()` | 获取当前渲染目标 FBO |
| `BindFrameBuffer(fbo)` | `void BindFrameBuffer(FBO_Container_t* fbo)` | 绑定 FBO 为当前渲染目标 |
| `BindFrameBufferWithTextures(fbo, color, depth, depth_stencil, w, h)` | `void BindFrameBufferWithTextures(FBO_Container_t*, unsigned int color, unsigned int depth, unsigned int depth_stencil, int width, int height)` | 绑定 FBO 并显式指定纹理附件 |
| `PushFrameBuffer()` | `void PushFrameBuffer()` | 压栈当前 FBO 绑定状态 |
| `PopFrameBuffer()` | `void PopFrameBuffer()` | 弹栈恢复 FBO 绑定状态 |
| `GenFrameBuffer(fbo, name)` | `void GenFrameBuffer(FBO_Container_t* s, const char* szFrameBufferName)` | 创建新 FBO (调用 glGenFramebuffers) |
| `FreeFBO(fbo)` | `void FreeFBO(FBO_Container_t* s)` | 释放 FBO 及关联的所有纹理和渲染缓冲 |
| `ClearFBO(fbo)` | `void ClearFBO(FBO_Container_t* s)` | 清除 FBO 颜色和深度缓冲 |
| `GetFrameBufferName(fbo)` | `const char* GetFrameBufferName(FBO_Container_t* s)` | 获取 FBO 的调试名称 |
| `FrameBufferColorTexture(fbo, fmt)` | `void FrameBufferColorTexture(FBO_Container_t* s, unsigned int iInternalFormat)` | 为 FBO 附加指定格式的颜色纹理 |
| `FrameBufferDepthTexture(fbo, fmt)` | `void FrameBufferDepthTexture(FBO_Container_t* s, unsigned int iInternalFormat)` | 为 FBO 附加指定格式的深度纹理 |

---

### 3.9 纹理创建 (Texture Generation)

| 方法 | 签名 | 说明 |
|------|------|------|
| `GenTexture()` | `unsigned int GenTexture()` | 新建 OpenGL 纹理对象 (glGenTextures) |
| `GenTextureRGBA8(w, h, imm)` | `unsigned int GenTextureRGBA8(int w, int h, bool immutable)` | 创建 RGBA8 纹理 |
| `GenTextureColorFormat(w, h, fmt, filter, border, imm)` | `unsigned int GenTextureColorFormat(int w, int h, int iInternalFormat, bool filter, float* borderColor, bool immutable)` | 指定内部格式，带过滤和边框色选项 |
| `GenTextureArrayColorFormat(w, h, depth, fmt, filter, border, imm)` | `unsigned int GenTextureArrayColorFormat(int w, int h, int depth, int iInternalFormat, bool filter, float* borderColor, bool immutable)` | 创建 2D 纹理数组 |
| `GenDepthTexture(w, h, imm)` | `unsigned int GenDepthTexture(int w, int h, bool immutable)` | 创建深度纹理 |
| `GenDepthStencilTexture(w, h, imm)` | `unsigned int GenDepthStencilTexture(int w, int h, bool immutable)` | 创建深度-模板纹理 |
| `CreateDepthViewForDepthTexture(texId)` | `unsigned int CreateDepthViewForDepthTexture(int texId)` | 为已有深度纹理创建深度视图 |
| `CreateStencilViewForDepthTexture(texId)` | `unsigned int CreateStencilViewForDepthTexture(int texId)` | 为已有深度纹理创建模板视图 |
| `DeleteTexture(texid)` | `void DeleteTexture(unsigned int texid)` | 删除纹理对象 |

**关于 `immutable` 参数**:
- `true`: 使用 `glTexStorage2D` 创建不可变纹理存储（推荐，性能更好）
- `false`: 使用 `glTexImage2D` 创建可变纹理

---

### 3.10 缓冲对象 (Buffer Objects)

#### 3.10.1 创建/销毁

| 方法 | 签名 | 说明 |
|------|------|------|
| `GenBuffer()` | `unsigned int GenBuffer()` | 新建 OpenGL buffer (glGenBuffers) |
| `GenVAO()` | `unsigned int GenVAO()` | 新建 VAO (glGenVertexArrays) |
| `DeleteBuffer(buf)` | `void DeleteBuffer(unsigned int buf)` | 删除 buffer 对象 |
| `DeleteVAO(VAO)` | `void DeleteVAO(unsigned int VAO)` | 删除 VAO |
| `DeleteTexture(texid)` | `void DeleteTexture(unsigned int texid)` | 删除纹理对象 |

#### 3.10.2 绑定

| 方法 | 签名 | 说明 |
|------|------|------|
| `BindVAO(VAO)` | `void BindVAO(unsigned int VAO)` | 绑定 VAO |
| `BindABO(ABO)` | `void BindABO(unsigned int ABO)` | 绑定 Indirect Draw 属性缓冲 |
| `GetEmptyVAO()` | `unsigned int GetEmptyVAO()` | 获取空 VAO（用于无顶点属性的渲染，如全屏三角形） |

#### 3.10.3 数据上传方法族

所有 `Upload*` 方法签名模式: `(unsigned int handle, size_t size, const void* data)`  
带 `SubData` 的额外加 `size_t offset` 参数。

| 目标类型 | Static Draw | Dynamic Draw | Stream Draw | Stream Map | Sub Data |
|----------|-------------|--------------|-------------|------------|----------|
| **UBO** | — | `UploadDataToUBODynamicDraw` | — | — | `UploadSubDataToUBO` |
| **VBO** | `UploadDataToVBOStaticDraw` | `UploadDataToVBODynamicDraw` | `UploadDataToVBOStreamDraw` | `UploadDataToVBOStreamMap` | `UploadSubDataToVBO` |
| **EBO** | `UploadDataToEBOStaticDraw` | `UploadDataToEBODynamicDraw` | `UploadDataToEBOStreamDraw` | `UploadDataToEBOStreamMap` | `UploadSubDataToEBO` |
| **ABO** | `UploadDataToABOStaticDraw` | `UploadDataToABODynamicDraw` | — | — | — |
| **SSBO** | `UploadDataToSSBOStaticDraw` | — | — | — | — |

**使用场景指南**:
- **Static Draw**: 数据一次上传，多次使用（如静态模型几何体）
- **Dynamic Draw**: 数据频繁变化（如动态 UI 顶点）
- **Stream Draw**: 数据每帧完全重写（`glBufferData` + `GL_STREAM_DRAW`）
- **Stream Map**: 使用 persistent-mapped buffer (`glMapBufferRange`)，更低延迟，适合大数据流
- **Sub Data**: 局部更新 (`glBufferSubData`)，不重新分配缓冲区

---

### 3.11 清除缓冲 (Clearing)

| 方法 | 签名 |
|------|------|
| `ClearColor(color)` | `void ClearColor(float* color)` — 4 分量 RGBA, 各分量 [0.0, 1.0] |
| `ClearDepth(depth)` | `void ClearDepth(float depth)` — 深度值 [0.0, 1.0] |
| `ClearDepthStencil(depth, ref, mask)` | `void ClearDepthStencil(float depth, int stencilref, int stencilmask)` |
| `ClearColorDepthStencil(color, depth, ref, mask)` | `void ClearColorDepthStencil(float* color, float depth, int stencilref, int stencilmask)` |
| `ClearStencil(mask)` | `void ClearStencil(int mask)` |

---

### 3.12 Stencil 操作 (Stencil Operations)

| 方法 | 签名 | 说明 |
|------|------|------|
| `BeginStencilCompareEqual(ref, mask)` | `void BeginStencilCompareEqual(int ref, int mask)` | 开启 `GL_EQUAL` 模板测试 |
| `BeginStencilCompareNotEqual(ref, mask)` | `void BeginStencilCompareNotEqual(int ref, int mask)` | 开启 `GL_NOTEQUAL` 模板测试 |
| `BeginStencilWrite(ref, writeMask)` | `void BeginStencilWrite(int ref, int write_mask)` | 开启模板写入 (`GL_REPLACE`) |
| `EndStencil()` | `void EndStencil()` | 结束所有模板操作，恢复默认状态 |

**Stencil 掩码常量** (来自 `gl_common.h`):
```cpp
#define STENCIL_MASK_ALL               0xFF
#define STENCIL_MASK_NONE              0

// 主视图 stencil 位
#define STENCIL_MASK_NO_FOG            0x1
#define STENCIL_MASK_NO_LIGHTING       0x2
#define STENCIL_MASK_NO_BLOOM          0x4
#define STENCIL_MASK_NO_GLOW           0x8
#define STENCIL_MASK_HAS_FLATSHADE     0x10
#define STENCIL_MASK_HAS_DECAL         0x20
#define STENCIL_MASK_HAS_OUTLINE       0x40

// Studio 视图 stencil 位
#define STENCIL_MASK_HAS_SHADOW        0x1
#define STENCIL_MASK_HAS_FACE          0x2
```

---

### 3.13 Blit / 后处理 (Blitting & Post-Processing)

| 方法 | 签名 | 说明 |
|------|------|------|
| `CopyColor(src, dst)` | `void CopyColor(FBO_Container_t* src, FBO_Container_t* dst)` | 拷贝颜色缓冲区（使用 shader 实现） |
| `BlurPass(src, dst, scale, vertical)` | `void BlurPass(FBO_Container_t* src, FBO_Container_t* dst, float scale, bool vertical)` | 高斯模糊单 Pass（需配合 `GetBlurPassFBO` 使用） |
| `FXAA(src, dst)` | `void FXAA(FBO_Container_t* src, FBO_Container_t* dst)` | Fast Approximate Anti-Aliasing 后处理 |
| `DownSample(srcColor, srcStencil, dst, filter2x2, stencilFilter)` | `void DownSample(FBO_Container_t* src_color, FBO_Container_t* src_stencil, FBO_Container_t* dst, bool bUseFilter2x2, bool bUseStencilFilter)` | 降采样（2x2），可选模板过滤 |

| Blit 方法 | 拷贝的内容 |
|-----------|-----------|
| `BlitFrameBufferToFrameBufferDepthStencil(src, dst)` | 深度 + 模板 |
| `BlitFrameBufferToFrameBufferStencilOnly(src, dst)` | 仅模板 |
| `BlitFrameBufferToFrameBufferDepthOnly(src, dst)` | 仅深度 |
| `BlitFrameBufferToFrameBufferColorDepthStencil(src, dst)` | 颜色 + 深度 + 模板 |
| `BlitFrameBufferToFrameBufferColorDepth(src, dst)` | 颜色 + 深度 |
| `BlitFrameBufferToFrameBufferColorOnly(src, dst)` | 仅颜色 |
| `BlitFrameBufferToScreen(src)` | 将 FBO 颜色 blit 到屏幕（默认 framebuffer） |

> 所有 Blit 方法底层使用 `glBlitFramebuffer`，源和目标 FBO 尺寸必须匹配

---

### 3.14 矩阵操作 (Matrix Operations)

#### 3.14.1 世界矩阵 (World/Model Matrix)

| 方法 | 签名 | 说明 |
|------|------|------|
| `GetWorldMatrix()` | `float* GetWorldMatrix()` | 返回 4×4 世界矩阵指针（16 个 float, 列主序） |
| `PushWorldMatrix()` | `void PushWorldMatrix()` | 压栈世界矩阵 |
| `PopWorldMatrix()` | `void PopWorldMatrix()` | 弹栈恢复世界矩阵 |
| `LoadIdentityForWorldMatrix()` | `void LoadIdentityForWorldMatrix()` | 载入单位矩阵 |
| `RotateWorldMatrix(angle, x, y, z)` | `void RotateWorldMatrix(float angle, float x, float y, float z)` | 绕 (x,y,z) 轴旋转 angle 度 |
| `TranslateWorldMatrix(x, y, z)` | `void TranslateWorldMatrix(float x, float y, float z)` | 平移 |
| `SetupPlayerViewWorldMatrix(origin, viewangles)` | `void SetupPlayerViewWorldMatrix(const float* origin, const float* viewangles)` | 基于玩家位置和朝向构建世界矩阵 |

#### 3.14.2 投影矩阵 (Projection Matrix)

| 方法 | 签名 | 说明 |
|------|------|------|
| `GetProjectionMatrix()` | `float* GetProjectionMatrix()` | 返回 4×4 投影矩阵指针（16 个 float, 列主序） |
| `PushProjectionMatrix()` | `void PushProjectionMatrix()` | 压栈投影矩阵 |
| `PopProjectionMatrix()` | `void PopProjectionMatrix()` | 弹栈恢复投影矩阵 |
| `LoadIdentityForProjectionMatrix()` | `void LoadIdentityForProjectionMatrix()` | 载入单位矩阵 |
| `SetupPerspective(fovx, fovy, zn, zf)` | `void SetupPerspective(float fovx, float fovy, float zNear, float zFar)` | 设置透视投影（使用水平和垂直 FOV） |
| `SetupFrustumProjectionMatrix(l,r,b,t,zn,zf)` | `void SetupFrustumProjectionMatrix(float left, float right, float bottom, float top, float zNear, float zFar)` | 设置棱台投影（非对称） |
| `SetupOrthoProjectionMatrix(l,r,b,t,zn,zf,neg)` | `void SetupOrthoProjectionMatrix(float left, float right, float bottom, float top, float zNear, float zFar, bool NegativeOneToOneZ)` | 设置正交投影。`NegativeOneToOneZ=true` 时 Z 范围 [-1,1]，否则 [0,1] |

#### 3.14.3 视口 (Viewport)

| 方法 | 签名 | 说明 |
|------|------|------|
| `SetViewport(x, y, w, h)` | `void SetViewport(float x, float y, float w, float h)` | 设置 OpenGL 视口（左下角为原点） |

---

### 3.15 相机系统 (Camera System)

```cpp
void SetupCameraView(camera_view_t* view) = 0;
void UploadCameraUBOData(const camera_ubo_t* CameraUBO) = 0;
```

上传到 GPU 绑定点 `BINDING_POINT_CAMERA_UBO` (绑定點 1)。

**camera_view_t** 结构体 (`gl_common.h:451-463`, 16 字节对齐):
```cpp
#pragma pack(push, 16)

typedef struct camera_view_s
{
    mat4 worldMatrix{};        // 世界矩阵
    mat4 projMatrix{};         // 投影矩阵
    mat4 invWorldMatrix{};     // 逆世界矩阵
    mat4 invProjMatrix{};      // 逆投影矩阵
    vec4_t frustum[4]{};       // 4 个视锥平面
    vec4_t viewport{};         // 视口 (x, y, w, h)
    vec4_t viewpos{};          // 观察位置
    vec4_t vpn{};              // 观察方向 (view plane normal)
    vec4_t vright_znear{};     // (right.x, right.y, right.z, zNear)
    vec4_t vup_zfar{};         // (up.x, up.y, up.z, zFar)
} camera_view_t;
```

**camera_ubo_t** 结构体 (`gl_common.h:465-472`):
```cpp
typedef struct camera_ubo_s
{
    camera_view_t views[6];    // 最多 6 个视图（用于 multiview / cubemap）
    int numViews;              // 实际视图数
    int padding;
    int padding2;
    int padding3;
} camera_ubo_t;

static_assert((sizeof(camera_ubo_t) % 16) == 0, "Size check");

#pragma pack(pop)
```

---

### 3.16 场景渲染 (Scene Rendering)

| 方法 | 签名 | 说明 |
|------|------|------|
| `GetRefDef()` | `void* GetRefDef()` | 获取 GoldSrc `refdef_t*` 指针 |
| `SetRefDefViewAngles(ang)` | `void SetRefDefViewAngles(const float* viewangles)` | 复制 3 分量到 `r_refdef.viewangles` |
| `SetRefDefViewOrigin(org)` | `void SetRefDefViewOrigin(const float* vieworg)` | 复制 3 分量到 `r_refdef.vieworg` |
| `PushRefDef()` | `void PushRefDef()` | 压栈当前 vieworg 和 viewangles |
| `UpdateRefDef()` | `void UpdateRefDef()` | 根据 refdef 重新计算渲染原点和视图向量 |
| `RenderScene()` | `void RenderScene()` | 底层调用 `R_RenderScene`，渲染当前场景 |
| `PopRefDef()` | `void PopRefDef()` | 弹栈恢复 vieworg 和 viewangles |

**典型使用流程（Radar 模式）**:
```
PushRefDef → 修改 vieworg/viewangles → UpdateRefDef → RenderScene → PopRefDef
```

---

### 3.17 实体绘制 (Entity Drawing)

| 方法 | 签名 | 说明 |
|------|------|------|
| `SetCurrentEntity(entity)` | `void SetCurrentEntity(cl_entity_t* entity) const` | 设置当前渲染实体（类型为 GoldSrc `cl_entity_t`） |
| `DrawCurrentEntity(transparent)` | `void DrawCurrentEntity(bool bTransparent)` | 底层调用 `R_DrawCurrentEntity`。`bTransparent=true` 用于透明物体。绘制前会检查 `DrawClassify` 和 `HideEntity` |

---

### 3.18 渲染状态开关 (State Toggles)

| 方法 | 签名 | 说明 |
|------|------|------|
| `SetDrawClassify(classify)` | `void SetDrawClassify(int drawClassify)` | 设置绘制分类掩码（见下方常量） |
| `GetDrawClassify()` | `int GetDrawClassify() const` | 获取当前绘制分类掩码 |
| `SetMultiviewEnabled(b)` | `void SetMultiviewEnabled(bool b)` | 开启/关闭 Multiview 渲染（VR/立体） |
| `IsMultiviewEnabled()` | `bool IsMultiviewEnabled() const` | 查询 Multiview 状态 |
| `SetDrawGammaBlendEnabled(b)` | `void SetDrawGammaBlendEnabled(bool b)` | 开启/关闭 Gamma 空间混合 |
| `IsDrawGammaBlendEnabled()` | `bool IsDrawGammaBlendEnabled() const` | 查询 Gamma 混合状态 |

**DrawClassify 掩码常量** (`gl_common.h:19-36`):
```cpp
#define DRAW_CLASSIFY_WORLD            0x1
#define DRAW_CLASSIFY_SKYBOX           0x2
#define DRAW_CLASSIFY_OPAQUE_ENTITIES  0x4
#define DRAW_CLASSIFY_TRANS_ENTITIES   0x8
#define DRAW_CLASSIFY_PARTICLES        0x10
#define DRAW_CLASSIFY_DECAL            0x20
#define DRAW_CLASSIFY_WATER            0x40
#define DRAW_CLASSIFY_LIGHTMAP         0x80

#define DRAW_CLASSIFY_ALL  (所有标志的 OR 组合)
```

---

### 3.19 调试工具 (Debug Utilities)

| 方法 | 签名 | 说明 |
|------|------|------|
| `BeginDebugGroup(name)` | `void BeginDebugGroup(const char* name)` | 在 RenderDoc/Nsight 中开启命名调试组 |
| `BeginDebugGroupFormat(fmt, ...)` | `void BeginDebugGroupFormat(const char* fmt, ...)` | 格式化版本 (类似 printf) |
| `EndDebugGroup()` | `void EndDebugGroup()` | 关闭当前调试组 |
| `SetTextureDebugName(id, name)` | `void SetTextureDebugName(unsigned int textureId, const char* name)` | 设置纹理在调试器中的显示名称 |
| `SetFrameBufferDebugName(id, name)` | `void SetFrameBufferDebugName(unsigned int framebufferId, const char* name)` | 设置 FBO 在调试器中的显示名称 |

---

### 3.20 Render 回调注册 (Render Callbacks)

| 方法 | 签名 | 说明 |
|------|------|------|
| `RegisterRenderCallbacks(cb)` | `void RegisterRenderCallbacks(IMetaRendererCallbacks* RenderCallbacks)` | 注册渲染回调 |
| `UnregisterRenderCallbacks(cb)` | `void UnregisterRenderCallbacks(IMetaRendererCallbacks* RenderCallbacks)` | 注销渲染回调 |

内部实现在 `gl_rmain.cpp:325` 维护一个 `std::vector<IMetaRendererCallbacks*> g_RenderCallbacks`。

---

## 4. 支持类型详解

### 4.1 FBO_Container_t — FBO 容器

定义于 `thirdparty/MetaHookSv/Plugins/Renderer/gl_common.h:48-64`（运行时使用的版本）:

```cpp
typedef struct FBO_Container_s
{
    GLuint s_hBackBufferFBO;        // FBO handle (OpenGL framebuffer object name)
    GLuint unused;                   // 未使用（保留字段）
    GLuint s_hBackBufferDepthView;   // 深度视图 handle
    GLuint s_hBackBufferTex;         // 颜色纹理附件 0
    GLuint s_hBackBufferTex2;        // 颜色纹理附件 1
    GLuint s_hBackBufferTex3;        // 颜色纹理附件 2
    GLuint s_hBackBufferTex4;        // 颜色纹理附件 3
    GLuint s_hBackBufferDepthTex;    // 深度-模板纹理
    GLuint s_hBackBufferStencilView; // 模板视图
    int iWidth;                      // 缓冲区宽度（像素）
    int iHeight;                     // 缓冲区高度（像素）
    int iTextureColorFormat;         // 颜色纹理内部格式 (GL_RGBA8, GL_RGBA16F 等)
    int iTextureDepthFormat;         // 深度纹理内部格式 (GL_DEPTH24_STENCIL8 等)
    char szFrameBufferName[64]{};    // 调试名称
} FBO_Container_t;
```

> **注意**: ABCEnchance 本地 `src/core/opengl/gl_common.h:54` 的版本略有不同（使用 `s_hBackBufferCB` / `s_hBackBufferDB` 代替 `unused` / `s_hBackBufferDepthView`）。但运行时使用的是 MetaHookSV Renderer 插件中的结构，因此以本定义为准。

### 4.2 绘制顶点结构

```cpp
// 带纹理矩形顶点 — Vertex Attribute 布局见 §4.4
typedef struct texturedrectvertex_s
{
    vec2_t pos;        // float[2] 屏幕坐标
    vec2_t texcoord;   // float[2] UV 坐标
    vec4_t col;        // float[4] RGBA 颜色 [0.0, 1.0]
} texturedrectvertex_t;

// 纯色矩形顶点 — Vertex Attribute 布局见 §4.4
typedef struct filledrectvertex_s
{
    vec2_t pos;        // float[2] 屏幕坐标
    vec4_t col;        // float[4] RGBA 颜色 [0.0, 1.0]
} filledrectvertex_t;
```

### 4.3 ProgramState 标志位

用于 `DrawTexturedRect` / `DrawTexturedQuad` 的 `programState` 参数（`uint64_t` 位掩码）:

```cpp
#define DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED           0x1ull   // 标准 Alpha 混合
#define DRAW_TEXTURED_RECT_ADDITIVE_BLEND_ENABLED        0x2ull   // 加法混合 (src*1 + dst*1)
#define DRAW_TEXTURED_RECT_ALPHA_BASED_ADDITIVE_ENABLED  0x4ull   // Alpha 缩放加法混合 (src.a*src + dst)
#define DRAW_TEXTURED_RECT_SCISSOR_ENABLED               0x8ull   // 开启 Scissor 测试
#define DRAW_TEXTURED_RECT_ALPHA_TEST_ENABLED            0x10ull  // 开启 Alpha 测试 (丢弃低于阈值的片元)
#define DRAW_TEXTURED_RECT_MASK_TEXTURE_ENABLED          0x20ull  // 开启遮罩纹理（使用纹理单元 1）
```

用于 `DrawFilledRect` / `DrawFilledQuad` 的 `programState` 参数:

```cpp
#define DRAW_FILLED_RECT_ALPHA_BLEND_ENABLED              0x1ull
#define DRAW_FILLED_RECT_ADDITIVE_BLEND_ENABLED           0x2ull
#define DRAW_FILLED_RECT_ALPHA_BASED_ADDITIVE_ENABLED     0x4ull
#define DRAW_FILLED_RECT_ZERO_SRC_ALPHA_BLEND_ENABLED     0x8ull   // ⚠️ 与 SCISSOR 值冲突
#define DRAW_FILLED_RECT_SCISSOR_ENABLED                  0x8ull   // ⚠️ 与 ZERO_SRC_ALPHA 值冲突
#define DRAW_FILLED_RECT_LINE_ENABLED                     0x10ull  // 线框模式 (GL_LINES)
```

> **已知 Bug**: `DRAW_FILLED_RECT_ZERO_SRC_ALPHA_BLEND_ENABLED` 和 `DRAW_FILLED_RECT_SCISSOR_ENABLED` 均使用 `0x8ull`，不能同时使用

### 4.4 Vertex Attribute Location 常量

**texturedrectvertex_t 的 VAO 布局**:
```cpp
#define TEXTUREDRECT_VA_POSITION   0   // vec2 pos
#define TEXTUREDRECT_VA_TEXCOORD   1   // vec2 texcoord
#define TEXTUREDRECT_VA_COLOR      2   // vec4 col
#define TEXTUREDRECT_VA_MATRIX0    3   // 实例化矩阵第 0 行 (mat4, 每行一个 attribute)
#define TEXTUREDRECT_VA_MATRIX1    4   // 实例化矩阵第 1 行
#define TEXTUREDRECT_VA_MATRIX2    5   // 实例化矩阵第 2 行
#define TEXTUREDRECT_VA_MATRIX3    6   // 实例化矩阵第 3 行
```

**filledrectvertex_t 的 VAO 布局**:
```cpp
#define FILLEDRECT_VA_POSITION     0   // vec2 pos
#define FILLEDRECT_VA_COLOR        1   // vec4 col
#define FILLEDRECT_VA_MATRIX0      2   // 实例化矩阵第 0 行
#define FILLEDRECT_VA_MATRIX1      3   // 实例化矩阵第 1 行
#define FILLEDRECT_VA_MATRIX2      4   // 实例化矩阵第 2 行
#define FILLEDRECT_VA_MATRIX3      5   // 实例化矩阵第 3 行
```

### 4.5 UBO/SSBO 绑定点常量

```cpp
#define BINDING_POINT_SCENE_UBO           0   // scene_ubo_t — 场景全局参数
#define BINDING_POINT_CAMERA_UBO          1   // camera_ubo_t — 相机数据
#define BINDING_POINT_DLIGHT_UBO          2   // dlight_ubo_t — 动态光源数据
#define BINDING_POINT_ENTITY_UBO          3   // entity_ubo_t — 实体变换数据
#define BINDING_POINT_STUDIO_UBO          3   // studio_ubo_t — Studio 模型数据 (与 entity_ubo 共用)
#define BINDING_POINT_MATERIAL_SSBO       4   // 材质参数
#define BINDING_POINT_OIT_FRAGMENT_SSBO   5   // OIT 片元链表
#define BINDING_POINT_OIT_NUMFRAGMENT_SSBO 6  // OIT 片元计数
#define BINDING_POINT_OIT_COUNTER_SSBO    7   // OIT 计数器
```

---

## 5. IMetaRendererCallbacks 接口

该接口允许 ABCEnchance 钩入渲染管线的特定阶段。定义在 `IMetaRenderer.h:24-76`。

```cpp
class IMetaRendererCallbacks : public IBaseInterface
{
public:
    // 来自 GL_BeginRendering — 渲染开始
    virtual void OnBeginRendering(int* x, int* y, int* width, int* height) = 0;

    // 来自 GL_BeginRendering → R_RenderFrameStart — 帧渲染开始
    virtual void OnRenderFrameStart() = 0;

    // 来自 GL_BeginRendering（后处理阶段）
    virtual void OnBeginRenderingPost(int* x, int* y, int* width, int* height) = 0;

    // 来自 GL_FreeFrameBuffers — 屏幕分辨率变化或引擎重载时释放
    virtual void OnFreeFrameBuffers() = 0;

    // 来自 GL_GenerateFrameBuffers — 屏幕分辨率变化或引擎重载后重建
    virtual void OnGenerateFrameBuffers() = 0;

    // 来自 R_PreRenderView — 场景渲染前
    virtual void OnPreRenderView() = 0;

    // 来自 R_PreRenderView，切换到目标 FBO 之后
    virtual void OnPreRenderViewPost() = 0;

    // 来自 R_PostRenderView
    virtual void OnPostRenderView() = 0;

    // 来自 R_PostRenderView，执行完所有内建后处理之后
    virtual void OnPostRenderViewPost() = 0;

    // 来自 R_RenderEndFrame — 帧渲染结束
    virtual void OnRenderEndFrame() = 0;
};
```

**ABCEnchance 实现** (`src/core/metahook/MetaRendererCallbacks.h:4`):

```cpp
class CMetaRendererCallbacks : public IMetaRendererCallbacks
{
    // 大部分回调为空桩
    // OnPreRenderView() 调用 GetBaseViewPort()->GetRadarPanel()->RenderRadar()
};
```

全局实例: `src/core/metahook/MetaRendererCallbacks.cpp:5` — `CMetaRendererCallbacks g_MetaRendererCallbacks;`

注册于 `src/exportfuncs.cpp:278`:
```cpp
MetaRenderer()->RegisterRenderCallbacks(&g_MetaRendererCallbacks);
```

---

## 6. 典型使用模式

### 6.1 绘制带纹理的矩形

```cpp
#include <IMetaRenderer.h>
#include "core/opengl/gl_common.h"

auto* pRenderer = MetaRenderer();
if (!pRenderer)
    return;

uint64_t state = DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED;

// 构建 4 顶点 + 6 索引 = 2 个三角形组成的矩形
texturedrectvertex_t vertices[4] = {
    {{0.0f,   0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},  // 左上
    {{0.0f, 100.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},  // 左下
    {{100.0f, 100.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // 右下
    {{100.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}   // 右上
};
uint32_t indices[6] = {0, 1, 2, 0, 2, 3};

pRenderer->DrawTexturedRect(
    textureId, vertices, 4, indices, 6,
    state, "MyPass::DrawRect"
);
```

### 6.2 使用 Quad 快捷方式（更简洁）

```cpp
float color[4] = {1.0f, 1.0f, 1.0f, 0.75f};
pRenderer->DrawTexturedQuad(
    textureId,
    x0, y0, x1, y1,
    color,
    DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED,
    "MyPass::DrawQuad"
);
```

### 6.3 在 3D 场景中渲染自定义视图（Radar 模式）

```cpp
// 参见 src/hud/vgui/radar.cpp 了解完整实现
auto* p = MetaRenderer();
if (!p) return;

float myOrigin[3] = {x, y, z};
float myAngles[3] = {pitch, yaw, roll};

p->PushRefDef();                          // 保存当前 refdef
p->SetRefDefViewOrigin(myOrigin);
p->SetRefDefViewAngles(myAngles);
p->UpdateRefDef();                        // 重新计算视图向量
{
    // 可选：渲染到特定 FBO
    p->PushFrameBuffer();
    p->BindFrameBuffer(myFBO);
    p->RenderScene();                     // 执行 R_RenderScene
    p->PopFrameBuffer();
}
p->PopRefDef();                           // 还原原始 refdef
```

### 6.4 创建和使用自定义 FBO

```cpp
FBO_Container_t myFBO = {};

// 步骤 1: 创建
pRenderer->GenFrameBuffer(&myFBO, "MyCustomFBO");
pRenderer->FrameBufferColorTexture(&myFBO, GL_RGBA8);
pRenderer->FrameBufferDepthTexture(&myFBO, GL_DEPTH24_STENCIL8);

// 步骤 2: 绑定并使用
pRenderer->PushFrameBuffer();
pRenderer->BindFrameBuffer(&myFBO);
{
    pRenderer->ClearColorDepthStencil(
        clearColor, 1.0f, 0, STENCIL_MASK_ALL
    );
    // ... 渲染操作 ...
}
pRenderer->PopFrameBuffer();

// 步骤 3: 释放
pRenderer->FreeFBO(&myFBO);
```

### 6.5 编译 Shader

```cpp
auto* p = MetaRenderer();
if (!p) return 0;

// 方式 A: 基础编译
uint32_t progId = p->CompileShaderFile(
    "svencoop/abcenchance/shader/MyVertex.vs",
    "svencoop/abcenchance/shader/MyFragment.fs",
    "#define ENABLE_FEATURE 1\n",  // VS 端宏 (可选)
    nullptr                         // FS 端宏 (可选)
);

// 方式 B: 扩展编译（支持 Geometry Shader）
CCompileShaderArgs args;
args.vsfile = "path/to/vertex.vs";
args.fsfile = "path/to/fragment.fs";
args.gsfile = "path/to/geometry.gs";  // 可选
args.vsdefine = "#define USE_INSTANCING\n";
args.fsdefine = "#define GAMMA_CORRECT\n";
uint32_t progId2 = p->CompileShaderFileEx(&args);

// 使用
if (progId) {
    p->UseProgram(progId);
    // ... 绘制调用 ...
}
```

### 6.6 后处理：高斯模糊

```cpp
auto* p = MetaRenderer();
if (!p) return;

// 获取预分配的模糊 FBO
auto* blurH = p->GetBlurPassFBO(0, 0);  // pass 0, 水平
auto* blurV = p->GetBlurPassFBO(0, 1);  // pass 0, 垂直

// 两 Pass 高斯模糊
p->BlurPass(sourceFBO, blurH, 1.0f, false);  // 水平 Pass
p->BlurPass(blurH, destFBO, 1.0f, true);      // 垂直 Pass
```

详见 `vgui_controls/GaussianBlurPanel.cpp` 获取完整的模糊 + 降采样流程。

### 6.7 矩阵操作：构建自定义 3D 相机

```cpp
auto* p = MetaRenderer();

// 设置投影
p->SetupPerspective(fovX, fovY, 0.1f, 4096.0f);
p->SetViewport(0, 0, viewW, viewH);

// 构建 GoldSrc 世界矩阵（注意 GoldSrc 坐标系的特殊旋转）
p->LoadIdentityForWorldMatrix();
p->RotateWorldMatrix(-90.0f, 1.0f, 0.0f, 0.0f);   // GoldSrc 坐标系修正
p->RotateWorldMatrix(90.0f, 0.0f, 0.0f, 1.0f);     // GoldSrc 坐标系修正
p->RotateWorldMatrix(-yaw, 0.0f, 0.0f, 1.0f);
p->RotateWorldMatrix(pitch, 0.0f, 1.0f, 0.0f);
p->RotateWorldMatrix(roll, 1.0f, 0.0f, 0.0f);
p->TranslateWorldMatrix(-originX, -originY, -originZ);
// 现在可以获取世界矩阵用于 Shader
float* worldMat = p->GetWorldMatrix();
```

### 6.8 使用 PMB RingBuffer 高效上传数据

```cpp
auto* p = MetaRenderer();
if (!p) return;

// 创建 2MB 的 RingBuffer 用于 VBO
IPMBRingBuffer* pRing = p->CreatePMBRingBuffer(
    "MyVBO_Ring", 2 * 1024 * 1024, GL_ARRAY_BUFFER
);

// 每帧使用模式
pRing->BeginFrame();

CPMBRingBufferAllocation alloc;
if (pRing->Allocate(vertexDataSize, alloc)) {
    memcpy(alloc.ptr, myVertexData, vertexDataSize);
    // 使用 alloc.offset 作为 glDrawArrays 的起始偏移
}

pRing->EndFrame();

// 引擎关闭时释放
pRing->Destroy();
```

---

## 7. 关键注意事项

### 7.1 安全
1. **空指针检查**: 始终检查 `MetaRenderer() != nullptr`。初始化前和关闭后返回 `nullptr`
2. **引擎类型检查**: 仅 SvEngine 可用。`src/plugins.cpp:55-59` 中对非 SvEngine 执行 `SYS_ERROR()`
3. **线程安全**: 所有 `IMetaRenderer` 调用必须在渲染线程中进行（GoldSrc 为单线程渲染模型）

### 7.2 数据库差异
4. **FBO_Container_t 版本差异**: 两个 `gl_common.h` 副本中的 `FBO_Container_t` 定义不同。编译时使用 ABCEnchance 本地版 (`src/core/opengl/gl_common.h:54`)，运行时使用 MetaHookSV Renderer 版 (`thirdparty/MetaHookSv/Plugins/Renderer/gl_common.h:48`)。不要依赖前两个字段 (`unused`/`s_hBackBufferDepthView` vs `s_hBackBufferCB`/`s_hBackBufferDB`) 的值

### 7.3 已知 Bug
5. **DrawFilledRect 标志位冲突**: `DRAW_FILLED_RECT_ZERO_SRC_ALPHA_BLEND_ENABLED` 和 `DRAW_FILLED_RECT_SCISSOR_ENABLED` 都使用 `0x8ull`。不能同时使用这两个功能

### 7.4 参数约束
6. **索引数量**: `DrawTexturedRect` / `DrawFilledRect` 的 `indicesCount` 必须是 3 的倍数（三角形列表）
7. **坐标空间**: 2D 绘制的坐标是屏幕像素坐标，左上角为原点。这与 OpenGL 默认的左下原点不同，`Set2D()` / `DrawTexturedRect` 等内部已处理翻转
8. **颜色值范围**: 所有颜色分量范围为 `[0.0, 1.0]`（标准化浮点数），不是 0-255

### 7.5 资源
9. **Shader 路径**: `CompileShaderFile` 从 GoldSrc FileSystem 加载文件，路径格式如 `"svencoop/abcenchance/shader/XXX.vs"`
10. **FBO 生命周期**: `GenFrameBuffer` 创建的 FBO 必须用 `FreeFBO` 释放。在分辨率变化时（`OnFreeFrameBuffers` 回调）需重建
11. **RingBuffer 生命周期**: `CreatePMBRingBuffer` 返回的 `IPMBRingBuffer` 需要在引擎关闭前手动调用 `Destroy()`
12. **仅 Win32**: 该接口仅在 Win32 (x86) 构建中可用，无 x64 支持

### 7.6 矩阵
13. **矩阵内存布局**: `GetWorldMatrix()` 和 `GetProjectionMatrix()` 返回的 `float*` 指向 16 个 float 的 4×4 矩阵，列主序（OpenGL 标准）
14. **矩阵栈**: `PushWorldMatrix`/`PopWorldMatrix` 和 `PushProjectionMatrix`/`PopProjectionMatrix` 应成对使用

---

## 8. 相关文件索引

| 文件 | 内容 |
|------|------|
| `thirdparty/MetaHookSv/include/Interface/IMetaRenderer.h` | 接口定义（`IMetaRenderer` + `IMetaRendererCallbacks`） |
| `thirdparty/MetaHookSv/Plugins/Renderer/gl_rmain.cpp` | `CMetaRenderer` 实现（`IMetaRenderer.h:78-727` 的所有方法） |
| `thirdparty/MetaHookSv/Plugins/Renderer/gl_common.h` | 支持类型定义（运行时版本） |
| `thirdparty/MetaHookSv/Plugins/Renderer/gl_ringbuffer.h` | `IPMBRingBuffer` 接口 |
| `src/core/opengl/gl_common.h` | 支持类型定义（ABCEnchance 编译时版本） |
| `src/core/opengl/gl_draw.cpp` | 纹理矩形绘制封装（`DrawSPRIconRect` 等） |
| `src/core/opengl/gl_def.cpp` | Shader 编译使用示例 |
| `src/core/metahook/MetaRenderer.cpp` | `MetaRenderer()` 函数 + `MetaRenderer_Init()` 初始化 |
| `src/core/metahook/MetaRenderer.h` | MetaRenderer 初始化和关闭声明 |
| `src/core/metahook/MetaRendererCallbacks.h` | `CMetaRendererCallbacks` 类声明 |
| `src/core/metahook/MetaRendererCallbacks.cpp` | `CMetaRendererCallbacks` 实现（雷达渲染钩子） |
| `src/exportfuncs.cpp` | 初始化调用点 + 回调注册 |
| `src/hud/vgui/radar.cpp` | 雷达渲染 — 最全面的使用示例 |
| `vgui_controls/GaussianBlurPanel.cpp` | 模糊后处理使用示例 |
| `vgui_controls/GaussianBlurPanel.h` | GaussianBlur 声明 |
| `vgui_controls/ModelViewPanel.cpp` | 模型预览使用示例 |
| `src/plugins.cpp` | 入口点，引擎类型检查 |
