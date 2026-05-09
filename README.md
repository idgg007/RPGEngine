前言：这个项目所有代码是由Trae生成的，不保证没有bug，我只提供架构思路同时帮助AI查找部分的bug。内容中明显由很多问题比如这篇教程。~~但是我不想改（懒）~~比如说环境配置中，只要保证在windows环境中，在不考虑编译dll（引擎）的情况下，并不需要安装raylib库。同时还有一点没有提到，如果只需要使用已经编译好的dll，使用CMAKE编译时候可以删去所有带有RPGengine的行，除了`target_link_libraries(PlatformerGame RPGEngine)` 。

# RPG Engine 中文使用教程

## 一、项目概述

RPG Engine 是一个基于 raylib 的 2D 游戏引擎，采用 C/C++ 开发，提供简洁的 C 语言 API 接口。引擎支持多平台编译，包含物理模拟、碰撞检测、纹理渲染等核心功能。

**项目结构**:
```
RPGEngine/
├── Egine_dll/              # 引擎核心目录
│   ├── Engine.cpp/h        # 引擎主类
│   ├── World.cpp/h         # 世界管理类
│   ├── Object.cpp/h        # 游戏对象类
│   ├── API.cpp             # C接口实现
│   ├── cheatsheet.h        # API声明（核心头文件）
│   └── CmakeLists.txt      # CMake配置
├── PlatformerGame.cpp      # 平台游戏示例
└── 资源文件                # player.png, platform.png, background.png
```

---

## 二、环境配置

### 2.1 Windows 环境

**工具要求**:
- **编译器**: MinGW-w64 (gcc 8.0+) 或 MSVC 2019+
- **构建工具**: CMake 3.10+
- **依赖**: raylib 库

**安装步骤**:

1. **安装 MinGW-w64**
   ```bash
   # 使用 Chocolatey 安装（推荐）
   choco install mingw-w64
   ```

2. **安装 raylib**
   - 下载 raylib 预编译库: https://github.com/raysan5/raylib/releases
   - 将 `raylib/include` 和 `raylib/lib` 添加到系统路径

3. **环境变量配置**
   ```bash
   set PATH=C:\raylib\lib;%PATH%
   set INCLUDE=C:\raylib\include;%INCLUDE%
   set LIB=C:\raylib\lib;%LIB%
   ```

### 2.2 Linux 环境

**工具要求**:
- **编译器**: GCC 8.0+
- **构建工具**: CMake 3.10+
- **依赖**: raylib、OpenGL

**安装步骤**:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake libraylib-dev

# Arch Linux
sudo pacman -S base-devel cmake raylib
```

### 2.3 macOS 环境

**工具要求**:
- **编译器**: Clang (Xcode Command Line Tools)
- **构建工具**: CMake 3.10+
- **依赖**: raylib

**安装步骤**:

```bash
# 安装 Xcode Command Line Tools
xcode-select --install

# 使用 Homebrew 安装 raylib
brew install raylib cmake
```

---

## 三、编译步骤

### 3.1 使用 CMake 编译

```bash
# 进入引擎目录
cd RPGEngine/Egine_dll

# 创建构建目录
mkdir build && cd build

# 生成 Makefile（Windows 使用 MinGW Makefiles）
cmake .. -G "MinGW Makefiles"  # Windows
cmake ..                        # Linux/macOS

# 编译
make -j$(nproc)                 # Linux/macOS
mingw32-make                    # Windows

# 运行游戏
./PlatformerGame.exe            # Windows
./PlatformerGame                # Linux/macOS
```

### 3.2 CMakeLists.txt 解析

```cmake
cmake_minimum_required(VERSION 3.10)
project(RPGEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译器优化
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Windows DLL 导出
if(WIN32)
    add_definitions(-D_WINDLL -DMYLIBRARY_EXPORTS)
endif()

# 构建引擎动态库
add_library(RPGEngine SHARED
    World.cpp
    Object.cpp
    Engine.cpp
    API.cpp
)

target_link_libraries(RPGEngine raylib)

# 平台特定链接库
if(WIN32)
    target_link_libraries(RPGEngine opengl32 gdi32 winmm)
endif()

if(UNIX)
    target_link_libraries(RPGEngine pthread dl)
endif()

# 构建示例游戏
add_executable(PlatformerGame
    ../PlatformerGame.cpp
)

target_link_libraries(PlatformerGame RPGEngine)
```

---

## 四、核心概念

### 4.1 引擎架构

```
┌─────────────────────────────────────────────────────────┐
│                      Engine                             │
│  ┌─────────────┐  ┌─────────────┐                       │
│  │ RenderLoop  │  │SimulateLoop │                       │
│  │  (渲染线程)  │  │  (模拟线程)  │                       │
│  └──────┬──────┘  └──────┬──────┘                       │
└─────────┼────────────────┼───────────────────────────────┘
          │                │
          ▼                ▼
┌─────────────────────────────────────────────────────────┐
│                       World                             │
│  ┌─────────────┐  ┌─────────────┐                       │
│  │ Environment │  │  Creature   │                       │
│  │ (环境对象)   │  │  (生物对象)   │                       │
│  └─────────────┘  └─────────────┘                       │
└─────────────────────────────────────────────────────────┘
```

### 4.2 关键组件说明

| 组件 | 职责 | 特点 |
|------|------|------|
| **Engine** | 引擎核心，管理窗口和线程 | 单例模式，双线程架构 |
| **World** | 游戏世界，管理所有对象 | 支持相机、重力、信号系统 |
| **Object** | 游戏对象，包含物理属性 | 支持碰撞检测、纹理渲染 |

---

## 五、PlatformerGame 示例解析

### 5.1 项目结构与核心变量

PlatformerGame.cpp 是一个完整的 2D 平台跳跃游戏示例，展示了 RPG Engine 的核心用法。整个代码结构分为三个主要部分：

| 代码段 | 功能 | 行数范围 |
|--------|------|----------|
| 头文件与全局变量 | 引入依赖库，定义全局状态 | 1-17 |
| 回调函数 | 处理键盘输入和物理模拟 | 19-69 |
| 主函数 | 游戏初始化和主流程控制 | 71-137 |

**完整代码结构分析**:

```cpp
#include <raylib.h>              // raylib 图形库
#include "Egine_dll/cheatsheet.h" // RPG Engine API
#include <iostream>               // 标准输入输出
#include <stdbool.h>              // 布尔类型
#include <cstddef>                // 标准定义
#include <thread>                 // 线程支持
#include <chrono>                 // 时间支持
```

**全局变量设计**:

```cpp
void *g_world = NULL;           // 游戏世界对象指针
void *g_player = NULL;          // 玩家对象指针
bool g_can_jump = false;        // 跳跃状态标志（防止二段跳）
bool g_left_pressed = false;    // 左移按键状态
bool g_right_pressed = false;   // 右移按键状态
const double GRAVITY = 800.0;   // 重力加速度（像素/秒²）
const double JUMP_FORCE = -500.0; // 跳跃初速度（向上为负）
const double MOVE_SPEED = 200.0; // 水平移动速度（像素/秒）
```

**变量设计思路**:
- `g_world` 和 `g_player` 使用 `void*` 类型，通过 C API 与引擎交互
- `g_can_jump` 实现跳跃冷却机制，避免连续跳跃
- 物理参数（`GRAVITY`、`JUMP_FORCE`、`MOVE_SPEED`）定义为常量便于调整
- 按键状态使用布尔标志，在模拟阶段统一处理移动

### 5.2 键盘事件处理

```cpp
void HandleKeyboard(void *world, int key, bool pressed)
{
    double vx, vy;
    GetObjectV(g_player, &vx, &vy);

    // 跳跃键: 空格/W
    if (key == KEY_SPACE || key == KEY_W || key == 87 || key == 119)
    {
        if (pressed && g_can_jump)
        {
            SetObjectV(g_player, vx, JUMP_FORCE);
            g_can_jump = false;
        }
    }
    else if (key == KEY_LEFT || key == KEY_A || key == 65 || key == 97)
    {
        g_left_pressed = pressed;
    }
    else if (key == KEY_RIGHT || key == KEY_D || key == 68 || key == 100)
    {
        g_right_pressed = pressed;
    }
}
```

**设计要点**:
- 支持多种按键映射（大写/小写 ASCII 码）
- 跳跃使用速度设置而非加速度，确保跳跃高度一致

### 5.3 模拟信号（物理更新）

```cpp
void SimulateMid(void *world)
{
    double ax, ay;
    GetObjectA(g_player, &ax, &ay);

    // 通过检测加速度判断是否落地
    g_can_jump = false;
    if (ay < GRAVITY - 50)
    {
        g_can_jump = true;
    }

    double vx, vy;
    GetObjectV(g_player, &vx, &vy);

    // 处理水平移动
    if (g_left_pressed && g_right_pressed)
    {
        vx = 0;
    }
    else if (g_left_pressed)
    {
        vx = -MOVE_SPEED;
    }
    else if (g_right_pressed)
    {
        vx = MOVE_SPEED;
    }

    SetObjectV(g_player, vx, vy);
}
```

**设计要点**:
- 通过检测 Y 轴加速度判断玩家是否接触地面
- 使用信号机制在物理模拟中间插入自定义逻辑

### 5.4 主函数流程

```cpp
int main()
{
    // 1. 初始化引擎
    Init(800, 600, "Platformer Game");

    // 等待窗口就绪
    while (!IsRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // 2. 创建游戏世界
    g_world = CreateWorld();

    // 3. 创建平台（环境对象）
    void *platform1 = CreateObject();
    SetObjectPos(platform1, 400, 526);
    SetObjectSize(platform1, 256, 32);
    CreateRectHitbox(platform1, 1000.0);
    SetObjectImage(platform1, "platform.png");
    AddEnvironment(g_world, platform1);

    // ... 创建更多平台 ...

    // 4. 创建玩家（生物对象）
    g_player = CreateObject();
    SetObjectPos(g_player, 400, 450);
    SetObjectSize(g_player, 32, 32);
    CreateRectHitbox(g_player, 1.0);
    SetObjectImage(g_player, "player.png");
    AddCreature(g_world, g_player);

    // 5. 设置背景
    SetBackground(g_world, "background.png", 400, 300, 800, 600);

    // 6. 配置物理参数
    SetGravity(g_world, 0, GRAVITY);
    SetCameraPos(g_world, 400, 300, 800, 600);

    // 7. 注册信号回调
    AddKeyboardSignal(g_world, HandleKeyboard);
    AddSimulateSignalMid(g_world, SimulateMid);

    // 8. 显示世界并进入主循环
    ShowWorld(g_world);
    WaitForClose();

    return 0;
}
```

**执行流程**:
```
初始化引擎 → 创建世界 → 创建对象 → 设置属性 → 注册信号 → 显示世界 → 等待关闭
```

---

## 六、Cheatsheet API 函数详解

### 6.1 引擎控制函数

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `Init` | `width`, `height`, `title` | `void` | 初始化引擎，创建指定尺寸窗口（默认60FPS） |
| `InitWithFps` | `width`, `height`, `title`, `fps` | `void` | 初始化引擎，自定义帧率 |
| `Shutdown` | 无 | `void` | 关闭引擎，释放资源 |
| `IsRunning` | 无 | `bool` | 检查引擎是否运行中 |
| `ProcessEvents` | 无 | `void` | 处理输入事件（键盘/鼠标） |
| `RenderFrame` | 无 | `void` | 渲染一帧 |
| `SimulateFrame` | 无 | `void` | 模拟一帧物理更新 |
| `WaitForClose` | 无 | `void` | 阻塞等待窗口关闭 |

### 6.2 对象创建函数

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `CreateWorld` | 无 | `void*` | 创建游戏世界对象 |
| `CreateObject` | 无 | `void*` | 创建游戏对象 |
| `AddEnvironment` | `world`, `object` | `int` | 将对象添加为环境（静态对象），返回ID |
| `AddCreature` | `world`, `object` | `int` | 将对象添加为生物（动态对象），返回ID |
| `ChangeObjectId` | `world`, `object`, `new_id` | `bool` | 修改对象ID，成功返回true |
| `ShowWorld` | `world` | `void` | 显示指定世界 |

### 6.3 相机控制函数

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `SetCameraPos` | `world`, `x`, `y`, `size_x`, `size_y` | `void` | 设置相机位置和视口大小 |
| `MoveCamera` | `world`, `x`, `y` | `void` | 相对移动相机 |
| `ChangeCameraSize` | `world`, `size_x`, `size_y` | `void` | 修改相机视口大小 |

### 6.4 世界物理参数

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `SetGravity` | `world`, `g_x`, `g_y` | `void` | 设置世界重力加速度 |
| `SetWorldSpeed` | `world`, `speed` | `void` | 设置世界模拟速度倍数 |
| `SetSimulateType` | `world`, `type` | `void` | 设置模拟模式：true=独立线程，false=渲染线程 |

### 6.5 对象属性操作

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `SetObjectPos` | `object`, `x`, `y` | `void` | 设置对象位置 |
| `GetObjectPos` | `object`, `*x`, `*y` | `void` | 获取对象位置（输出参数） |
| `SetObjectSize` | `object`, `size_x`, `size_y` | `void` | 设置对象尺寸 |
| `GetObjectSize` | `object`, `*size_x`, `*size_y` | `void` | 获取对象尺寸（输出参数） |
| `SetObjectV` | `object`, `v_x`, `v_y` | `void` | 设置对象速度 |
| `GetObjectV` | `object`, `*v_x`, `*v_y` | `void` | 获取对象速度（输出参数） |
| `AddObjectV` | `object`, `v_x`, `v_y` | `void` | 增加对象速度 |
| `SetObjectA` | `object`, `a_x`, `a_y` | `void` | 设置对象加速度 |
| `GetObjectA` | `object`, `*a_x`, `*a_y` | `void` | 获取对象加速度（输出参数） |
| `AddObjectA` | `object`, `a_x`, `a_y` | `void` | 增加对象加速度 |

### 6.6 碰撞检测

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `AddHitBox` | `object`, `x1`, `y1`, `x2`, `y2`, `hardness` | `void` | 添加线段碰撞盒 |
| `CreateRectHitbox` | `object`, `hardness` | `void` | 根据对象尺寸创建矩形碰撞盒 |

### 6.7 纹理渲染

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `SetObjectImage` | `object`, `image_path` | `int` | 加载纹理图片，返回图片ID |
| `ChangeObjectShowImage` | `object`, `image_id` | `void` | 切换当前显示的图片 |
| `SetBackground` | `world`, `image_path`, `x`, `y`, `size_x`, `size_y` | `void` | 设置世界背景图 |

### 6.8 信号系统

| 函数名 | 参数 | 返回值 | 功能说明 |
|--------|------|--------|----------|
| `AddSimulateSignalBefore` | `world`, `SimulateSignal` | `void` | 在物理模拟**之前**触发的信号 |
| `AddSimulateSignalMid` | `world`, `SimulateSignal` | `void` | 在物理模拟**中间**触发的信号 |
| `AddSimulateSignalAfter` | `world`, `SimulateSignal` | `void` | 在物理模拟**之后**触发的信号 |
| `AddKeyboardSignal` | `world`, `KeyboardSignal` | `void` | 键盘事件信号 |
| `AddMouseSignal` | `world`, `MouseSignal` | `void` | 鼠标事件信号 |

### 6.9 回调函数类型定义

```c
// 模拟信号：在物理更新时触发
typedef void (*SimulateSignal)(void* world);

// 键盘信号：按键状态改变时触发
// 参数：world, key(按键码), pressed(是否按下)
typedef void (*KeyboardSignal)(void* world, int key, bool pressed);

// 鼠标信号：鼠标按钮状态改变时触发
// 参数：world, x(世界坐标), y(世界坐标), button(按钮索引), pressed(是否按下)
typedef void (*MouseSignal)(void* world, double x, double y, int button, bool pressed);
```

### 6.10 按键常量定义

| 常量 | 值 | 说明 |
|------|-----|------|
| `KEY_SPACE` | 32 | 空格键 |
| `KEY_ENTER` | 257 | 回车键 |
| `KEY_ESCAPE` | 256 | ESC键 |
| `KEY_UP` | 265 | 上方向键 |
| `KEY_DOWN` | 264 | 下方向键 |
| `KEY_LEFT` | 263 | 左方向键 |
| `KEY_RIGHT` | 262 | 右方向键 |
| `KEY_A` - `KEY_Z` | 65-90 | 字母键 |
| `KEY_0` - `KEY_9` | 48-57 | 数字键 |
| `MOUSE_BUTTON_LEFT` | 0 | 鼠标左键 |
| `MOUSE_BUTTON_RIGHT` | 1 | 鼠标右键 |
| `MOUSE_BUTTON_MIDDLE` | 2 | 鼠标中键 |

---

## 七、完整使用流程模板

```cpp
#include "cheatsheet.h"

void* g_world = NULL;

void OnKeyboard(void* world, int key, bool pressed) {
    // 处理键盘输入
}

void OnSimulate(void* world) {
    // 每帧模拟逻辑
}

int main() {
    // 1. 初始化
    Init(800, 600, "My Game");
    while (!IsRunning());  // 等待窗口就绪
    
    // 2. 创建世界
    g_world = CreateWorld();
    
    // 3. 创建对象
    void* obj = CreateObject();
    SetObjectPos(obj, 100, 100);
    SetObjectSize(obj, 50, 50);
    SetObjectImage(obj, "texture.png");
    AddCreature(g_world, obj);
    
    // 4. 设置世界参数
    SetGravity(g_world, 0, 980);
    SetCameraPos(g_world, 400, 300, 800, 600);
    
    // 5. 注册回调
    AddKeyboardSignal(g_world, OnKeyboard);
    AddSimulateSignalMid(g_world, OnSimulate);
    
    // 6. 启动游戏
    ShowWorld(g_world);
    WaitForClose();
    
    return 0;
}
```

---

## 八、常见问题

### 8.1 链接错误：找不到 raylib

**解决方案**:
- 确保 raylib 库路径已添加到系统环境变量
- 在 CMakeLists.txt 中明确指定 raylib 路径：
  ```cmake
  find_package(raylib REQUIRED PATHS "/path/to/raylib")
  ```

### 8.2 运行时缺少 libRPGEngine.dll

**解决方案**:
- 将 `libRPGEngine.dll` 复制到可执行文件同级目录
- 或添加 DLL 所在路径到系统 PATH

### 8.3 图片无法加载

**解决方案**:
- 确保图片文件与可执行文件在同一目录
- 使用绝对路径加载图片
- 支持格式：PNG, JPG, BMP, TGA

---

## 九、性能优化建议

1. **使用独立模拟线程**：
   ```cpp
   SetSimulateType(world, true);  // 启用独立模拟线程
   ```

2. **合理设置世界速度**：
   ```cpp
   SetWorldSpeed(world, 1.0);  // 默认1.0，可根据需求调整
   ```

3. **减少不必要的对象更新**：
   - 环境对象（AddEnvironment）不会被物理引擎更新
   - 仅对需要物理模拟的对象使用 AddCreature

---

## 十、扩展开发

引擎采用模块化设计，可通过以下方式扩展：

1. **添加新组件**：继承 Object 类扩展功能
2. **自定义信号**：通过信号系统注入自定义逻辑
3. **修改渲染管线**：修改 World::Render 方法

---

**总结**：RPG Engine 提供了简洁的 API 接口，适合快速开发 2D 游戏。通过信号系统和物理引擎，开发者可以轻松实现复杂的游戏逻辑。建议先从 PlatformerGame 示例入手，熟悉 API 后再进行自定义开发。