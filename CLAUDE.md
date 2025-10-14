# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

ABCEnchance是一个为Sven Co-op游戏设计的MetaHookSV插件，提供UI增强、视觉效果改进和游戏体验优化。这是一个C++项目，使用VGUI2界面框架，集成了OpenGL渲染、Steam API和各种第三方库。

**警告**: 此插件仅支持SvEngine引擎类型，在其他引擎上会报错退出。

## 构建系统

### 构建配置
项目使用Visual Studio (MSVC)构建，支持三种配置:
- **Release**: 标准发布版本，指令集优化为NoExtensions，优化级别MinSpace
- **Release_AVX2**: 启用AVX2指令集的优化版本
- **Debug**: 调试版本，禁用优化，使用MultiThreadedDebug运行时

### 构建命令
```powershell
# 使用构建脚本(交互式选择配置)
.\build-ABCEnchance.ps1

# 或直接使用MSBuild
msbuild /m /p:Configuration=Release /t:ABCEnchance
msbuild /m /p:Configuration=Release_AVX2 /t:ABCEnchance
msbuild /m /p:Configuration=Debug /t:ABCEnchance
```

### 项目依赖
- **MetaHookSV**: 核心框架，位于`thirdparty/MetaHookSv/`，必须通过`git submodule update --init --recursive`初始化
- **依赖库**: GLEW (静态链接), FreeImage, Capstone, RapidJSON
- **SDK**: SourceSDK (tier0/tier1/vstdlib), HLSDK, VGUI2, Steam API
- **构建属性**: 通过`MetaHookSv.props`导入MetaHookSV的全局构建配置

### 构建输出
- Debug构建: `Debug/ABCEnchance.dll`
- Release构建: `Release/ABCEnchance.dll` (自动复制到`Build/`)
- Release_AVX2构建: `Release_AVX2/ABCEnchance.dll` (自动复制到`Build/ABCEnchance_AVX2.dll`)

## 代码架构

### 核心模块结构

#### 插件入口 (`src/Source/plugins.cpp`)
- 实现`IPluginsV4`接口，作为MetaHookSV插件的生命周期管理
- `LoadEngine()`: 引擎加载时初始化，进行引擎类型检查、地址填充、Hook安装
- `LoadClient()`: 客户端DLL加载时导出函数替换(HUD_Init, HUD_VidInit等)
- `Shutdown()`: 清理资源

#### HUD系统 (`src/Source/HUD/`)
- **CCustomHud** (`CCustomHud.cpp`): 主HUD管理器，处理UserMsg Hook、武器/弹药状态、计分板数据
- **VGUI面板** (`vgui/`子目录):
  - `health.h/cpp`: 生命值和护甲显示
  - `crosshair.h/cpp`: 动态准心系统
  - `radar.h/cpp`: 小地图雷达
  - `weaponchoose.h/cpp`, `ammostack.h/cpp`, `weaponstack.h/cpp`: 武器选择和弹药显示
  - `scoreboard.h/cpp`: 计分板增强
  - `deadmsg.h/cpp`: 死亡消息
  - `itemhighlight.h/cpp`: 物品高亮
  - `vote.h/cpp`, `textmenu.h/cpp`: 投票和文本菜单
  - `motd.h/cpp`: MOTD显示
  - `playerboard.h/cpp`: 玩家信息面板
  - `sidetext.h/cpp`: 侧边文本(速度、分数等)

#### 渲染系统 (`src/Source/Utility/opengl/`)
- `gl_def.cpp`: OpenGL初始化和GLEW扩展加载
- `gl_shader.cpp`: Shader编译和管理
- `gl_draw.cpp`: 绘制工具函数
- `gl_utility.cpp`: OpenGL实用函数
- 着色器文件位于`Build/svencoop/abcenchance/shader/`

#### 效果增强 (`src/Source/efxenchance.cpp`)
- 粒子效果增强(血液、高斯枪特效等)
- 通过Hook `HUD_TempEntUpdate`等函数修改临时实体渲染

#### 工具模块
- **config.cpp**: CVars和配置管理
- **player_info.cpp**: 玩家信息跟踪
- **parsemsg.cpp**: 网络消息解析
- **httpclient.cpp**: HTTP客户端(用于网易云音乐API等)
- **crypto.cpp**: 加密工具
- **Task.cpp**: 异步任务系统
- **mymathlib.cpp**: 数学工具函数

#### 变量和状态 (`src/Source/Var/`)
- `local.cpp`: 本地玩家状态和CVars
- `vguilocal.cpp`: VGUI相关变量
- `extraprecache.cpp`: 额外资源预缓存
- `voice_status.cpp`, `voice_banmgr.cpp`: 语音状态管理

#### 其他功能
- **viewmodellag.cpp**: 视图模型延迟效果(武器晃动)
- **soundengine.cpp**: FMOD音频引擎集成
- **steamrichpresence.cpp**: Steam Rich Presence集成
- **playertrace.cpp**: 玩家追踪和碰撞检测

### VGUI2系统 (`VGUI2/` 和 `vgui_controls/`)
- 自定义的VGUI2实现，扩展了原版VGUI接口
- `ClientVGUI.cpp`: VGUI客户端接口Hook
- `GameUI/`: 游戏UI组件(BasePanel, GameConsole, OptionAdvancedDlg等)
- `vgui_controls/`: 大量自定义控件和面板(Button, Label, ImagePanel, ScrollBar等)

### 第三方库集成
- **Particleman** (`src/Particleman/`): 粒子系统
- **wadlib** (`src/thirdparty/wadlib/`): WAD文件纹理读取
- **qrcodegen** (`src/thirdparty/`): QR码生成
- **FMOD** (`src/thirdparty/Fmod/`): 音频播放(仅头文件，链接动态库)

## 资源文件结构

### 构建输出目录 (`Build/svencoop/`)
```
svencoop/
├── abcenchance/
│   ├── ABCEnchance.res          # 资源配置文件
│   ├── HudAnimations.txt        # HUD动画定义
│   ├── ItemHighLightList.txt    # 物品高亮列表
│   ├── localize/                # 本地化文本(english.txt, schinese.txt, spanish.txt)
│   ├── shader/                  # GLSL着色器
│   ├── mdl/, spr/, tga/        # 模型、精灵、纹理资源
│   └── scence/                  # 场景资源
└── metahook/plugins/
    └── ABCEnchance.dll
```

### AngelScript脚本 (`AngelScripts/`)
可选的服务端脚本，用于配合插件实现更好的体验(如`DeathMsg.as`)。

## 开发注意事项

### 内存和地址操作
- 大量使用MetaHook API进行内存Hook和函数替换
- `autofunc.cpp`/`autofunc.h`: 自动函数查找和签名匹配
- 注意引擎版本兼容性检查(`g_iEngineType`, `g_dwEngineBuildnum`)

### Hook机制
- 引擎Hook通过`InstallEngineHook()`安装
- UserMsg Hook通过`gEngfuncs.pfnHookUserMsg()`注册
- VGUI接口通过`VGUI2Extension_Init()`和各种`*_InstallHooks()`函数Hook

### CVars系统
通过`local.cpp`中的`DEFINE_CVAR`宏定义CVars，支持热更新和持久化配置。

### 错误处理
使用`SYS_ERROR()`宏报告致命错误，会弹出消息框并终止游戏。

### 编译选项
- 字符集: MultiByte(非Unicode)
- C++标准: C++20 (`/std:c++20`)
- C标准: C17
- 禁用警告: 4311, 4312(指针转换), 4819(代码页), 4996(不安全函数)
- 预处理器定义:
  - `GLEW_STATIC`: 静态链接GLEW
  - `VGUI_USE_SURFACE2`, `VGUI_USE_SCHEME2`, `VGUI_USE_INPUT2`: 使用VGUI2扩展接口
  - `NO_MALLOC_OVERRIDE`, `NO_TIER0_MINIDUMP`: 禁用Source SDK内存覆盖

## 常见任务

### 添加新的HUD元素
1. 在`src/Header/HUD/vgui/`创建头文件，继承自`vgui::Panel`或其子类
2. 在`src/Source/HUD/vgui/`实现面板逻辑
3. 在`CCustomHud::Init()`或`Viewport`中注册和初始化面板
4. 添加对应的CVars到`local.cpp`控制显示/隐藏

### 添加新的OpenGL着色器
1. 在`Build/svencoop/abcenchance/shader/`添加`.vsh`(顶点)和`.fsh`(片段)文件
2. 在代码中通过`gl_shader.cpp`的接口加载和编译
3. 使用`gl_draw.cpp`的工具函数进行绘制

### Hook新的引擎函数
1. 在`exportfuncs.cpp`或相应模块声明函数指针
2. 在`autofunc.cpp`中通过签名搜索或直接地址获取原函数
3. 在`*_InstallHooks()`函数中使用MetaHook API安装Hook
4. 实现Hook函数，调用原函数并添加自定义逻辑

### 调试技巧
- 使用Debug配置构建，启用调试信息和断言
- 检查`g_iEngineType`和`g_dwEngineBuildnum`确保在正确引擎上运行
- 查看`src/Header/Var/enginedef.h`了解引擎内部结构定义

## 本地化
所有用户可见文本应通过本地化系统定义:
- 字符串定义在`Build/svencoop/abcenchance/localize/*.txt`
- 使用`vgui::localize()->Find()`或`L10n()`宏访问本地化字符串
- 支持简体中文、英语、西班牙语

## 测试和部署
- 插件必须在安装了MetaHookSV的Sven Co-op环境中运行
- 在`svencoop/metahook/configs/plugins.lst`中注册DLL，需在CaptionMod之前加载
- 资源文件变更需要重启游戏生效
- 可使用`ABCEnchanceInstaller.exe`自动安装
