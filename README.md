前言：这个项目所有代码是由Trae生成的，不保证没有bug，我只提供架构思路同时帮助AI查找部分的bug。内容中明显由很多问题比如这篇教程。~~但是我不想改（懒）~~比如说确保文件存在中 `RPGEngine` 应当是自己的项目名，将dll放在自己项目的build文件夹中即可，还有，dll不在 `RPGEngine/build/libRPGEngine.dll` 中，在release中。并且在我个人的编译测试中，貌似并不需要`libRPGEngine.dll.a` 尽管ai认为需要。所以我没有在这里保留.dll.a文件。此外，这是我的C++课的作业完成的内容，所以未来也不会有什么太大的维护了，不过还是很欢迎基于这个去做一个更好的2d游戏引擎。顺带一提，我只保证在example中使用过的函数是八成没什么问题的，其它函数不做任何担保。

# RPG Engine 中文使用教程

## 一、项目概述

RPG Engine 是一个基于 raylib 的 2D 游戏引擎，采用 C/C++ 开发，提供简洁的 C 语言 API 接口。引擎支持多平台编译，包含物理模拟、碰撞检测、纹理渲染等核心功能。

**项目结构**:
```
RPGEngine/
├── Egine_dll/              # 引擎核心源码目录
│   ├── Engine.cpp/h        # 引擎主类
│   ├── World.cpp/h         # 世界管理类
│   ├── Object.cpp/h        # 游戏对象类
│   ├── API.cpp             # C接口实现
│   ├── cheatsheet.h        # API声明
│   └── CmakeLists.txt      # 引擎构建配置
├── cheatsheet.h            # 引擎API头文件（使用预编译DLL时需要）
├── PlatformerGame.cpp      # 平台游戏示例
├── CMakeLists.txt          # 游戏构建配置（使用预编译DLL）
├── build/                  # 构建输出目录（包含预编译的DLL）
│   ├── libRPGEngine.dll    # 预编译引擎动态库
│   └── libRPGEngine.dll.a  # MinGW导入库
└── 资源文件                # player.png, platform.png, background.png
```

---

## 二、环境构建与文件准备

### 2.1 两种构建模式

| 模式 | 说明 | 需要 raylib | 适用场景 |
|------|------|------------|----------|
| **使用预编译DLL** | 只构建游戏，使用已编译的 libRPGEngine.dll | 否 | 快速开发游戏，不需要修改引擎 |
| **从源码构建** | 同时构建引擎和游戏 | 是 | 需要修改引擎源码或调试 |

### 2.2 文件复制说明

使用预编译DLL时，确保以下文件存在：

| 文件来源 | 目标位置 | 说明 |
|----------|----------|------|
| `Egine_dll/cheatsheet.h` | `RPGEngine/cheatsheet.h` | API头文件，游戏代码需要包含 |
| `Egine_dll/build/libRPGEngine.dll` | `RPGEngine/build/libRPGEngine.dll` | 编译好的引擎DLL（编译和运行都需要） |
| 资源文件 | `RPGEngine/build/` | 运行时需要的图片资源 |

### 2.3 工具要求

**Windows 环境**:
- **编译器**: MinGW-w64 (gcc 8.0+) 或 MSVC 2019+
- **构建工具**: CMake 3.10+

**安装 MinGW-w64**:
```bash
# 使用 Chocolatey 安装（推荐）
choco install mingw-w64
```

---

## 三、CMakeLists.txt 详解

当前项目根目录的 `CMakeLists.txt` 专门用于使用预编译 DLL 构建游戏：

```cmake
cmake_minimum_required(VERSION 3.10)  # CMake最低版本要求

project(PlatformerGame)               # 项目名称

set(CMAKE_CXX_STANDARD 17)            # 使用C++17标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # 强制要求C++17

# 编译器优化（仅GCC/Clang）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

link_directories(${CMAKE_CURRENT_BINARY_DIR})  # 链接库搜索路径（build目录）

add_executable(PlatformerGame                  # 创建可执行文件
    PlatformerGame.cpp                         # 源代码文件
)

if(WIN32)
    target_link_libraries(PlatformerGame libRPGEngine)  # 链接引擎库
endif()

if(UNIX)
    target_link_libraries(PlatformerGame pthread dl)    # Linux/Mac链接系统库
endif()
```

**关键说明：**
- `${CMAKE_CURRENT_BINARY_DIR}` 指向执行 `cmake` 时指定的构建目录（通常是 `build/`）
- `libRPGEngine.dll.a` 是 MinGW 生成的导入库，链接时需要它来解析 DLL 中的函数

---

## 四、PlatformerGame.cpp 代码详解

### 4.1 头文件依赖

```cpp
#include "cheatsheet.h"   // 引擎API声明
#include <stdbool.h>      // 布尔类型
#include <cstddef>        // 标准定义（NULL等）
#include <thread>          // 线程支持（用于等待窗口就绪）
#include <chrono>          // 时间支持（用于sleep）
```

### 4.2 全局变量

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

### 4.3 键盘事件处理函数

```cpp
void HandleKeyboard(void *world, int key, bool pressed)
{
    double vx, vy;
    GetObjectV(g_player, &vx, &vy);  // 获取玩家当前速度

    // 跳跃键处理：空格/W（支持ASCII码 87=W, 119=w）
    if (key == KEY_SPACE || key == KEY_W || key == 87 || key == 119)
    {
        if (pressed && g_can_jump)   // 仅在可跳跃状态且按键按下时执行跳跃
        {
            SetObjectV(g_player, vx, JUMP_FORCE);  // 设置垂直速度
            g_can_jump = false;       // 禁用跳跃，落地后重新启用
        }
    }
    else if (key == KEY_LEFT || key == KEY_A || key == 65 || key == 97)
    {
        g_left_pressed = pressed;     // 更新左移状态
    }
    else if (key == KEY_RIGHT || key == KEY_D || key == 68 || key == 100)
    {
        g_right_pressed = pressed;    // 更新右移状态
    }
}
```

### 4.4 模拟信号函数（物理更新）

```cpp
void SimulateMid(void *world)
{
    double ax, ay;
    GetObjectA(g_player, &ax, &ay);  // 获取玩家加速度

    // 通过检测Y轴加速度判断是否落地
    g_can_jump = false;
    if (ay < GRAVITY - 50)           // 当加速度小于重力时（有支撑）
    {
        g_can_jump = true;            // 允许跳跃
    }

    double vx, vy;
    GetObjectV(g_player, &vx, &vy);   // 获取玩家速度

    // 处理水平移动
    if (g_left_pressed && g_right_pressed)
    {
        vx = 0;                       // 同时按左右键，速度为0
    }
    else if (g_left_pressed)
    {
        vx = -MOVE_SPEED;             // 向左移动
    }
    else if (g_right_pressed)
    {
        vx = MOVE_SPEED;              // 向右移动
    }

    SetObjectV(g_player, vx, vy);     // 设置新速度
}
```

### 4.5 主函数流程

```cpp
int main()
{
    // 1. 初始化引擎（窗口大小800x600，标题"Platformer Game"）
    Init(800, 600, "Platformer Game");

    // 等待窗口就绪（引擎启动需要时间）
    while (!IsRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // 2. 创建游戏世界
    g_world = CreateWorld();

    // 3. 创建平台（环境对象，静态，不参与物理模拟）
    void *platform1 = CreateObject();
    SetObjectPos(platform1, 400, 526);      // 设置位置
    SetObjectSize(platform1, 256, 32);      // 设置尺寸
    CreateRectHitbox(platform1, 1000.0);    // 创建矩形碰撞盒（高硬度）
    SetObjectImage(platform1, "platform.png"); // 设置纹理
    AddEnvironment(g_world, platform1);     // 添加为环境对象

    // ... 创建其他平台 ...

    // 4. 创建玩家（生物对象，动态，参与物理模拟）
    g_player = CreateObject();
    SetObjectPos(g_player, 400, 450);
    SetObjectSize(g_player, 32, 32);
    CreateRectHitbox(g_player, 1.0);        // 低硬度碰撞盒
    SetObjectImage(g_player, "player.png");
    AddCreature(g_world, g_player);         // 添加为生物对象

    // 5. 设置背景
    SetBackground(g_world, "background.png", 400, 300, 800, 600);

    // 6. 配置物理参数
    SetGravity(g_world, 0, GRAVITY);       // 设置重力（仅垂直方向）
    SetCameraPos(g_world, 400, 300, 800, 600); // 设置相机位置和视口

    // 7. 注册信号回调
    AddKeyboardSignal(g_world, HandleKeyboard);  // 键盘事件
    AddSimulateSignalMid(g_world, SimulateMid); // 物理模拟中间触发

    // 8. 显示世界并进入主循环
    ShowWorld(g_world);
    WaitForClose();  // 阻塞等待窗口关闭

    return 0;
}
```

### 4.6 操作说明

| 按键 | 功能 |
|------|------|
| `A` / ← | 左移 |
| `D` / → | 右移 |
| `W` / 空格 | 跳跃 |

---

## 五、API函数完整清单

### 5.1 引擎控制函数

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `Init` | `width`, `height`, `title` | `void` | 初始化引擎，创建窗口（默认60FPS） | **是** |
| `InitWithFps` | `width`, `height`, `title`, `fps` | `void` | 初始化引擎，自定义帧率 | 否 |
| `Shutdown` | 无 | `void` | 关闭引擎，释放资源 | 否 |
| `IsRunning` | 无 | `bool` | 检查引擎是否运行中 | **是** |
| `ProcessEvents` | 无 | `void` | 处理输入事件 | 否 |
| `RenderFrame` | 无 | `void` | 渲染一帧 | 否 |
| `SimulateFrame` | 无 | `void` | 模拟一帧物理更新 | 否 |
| `ShowWorld` | `world` | `void` | 显示指定世界 | **是** |
| `WaitForClose` | 无 | `void` | 阻塞等待窗口关闭 | **是** |

### 5.2 对象创建函数

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `CreateWorld` | 无 | `void*` | 创建游戏世界对象 | **是** |
| `CreateObject` | 无 | `void*` | 创建游戏对象 | **是** |
| `AddEnvironment` | `world`, `object` | `int` | 添加为环境对象（静态） | **是** |
| `AddCreature` | `world`, `object` | `int` | 添加为生物对象（动态） | **是** |
| `ChangeObjectId` | `world`, `object`, `new_id` | `bool` | 修改对象ID | 否 |

### 5.3 相机控制函数

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `SetCameraPos` | `world`, `x`, `y`, `size_x`, `size_y` | `void` | 设置相机位置和视口 | **是** |
| `MoveCamera` | `world`, `x`, `y` | `void` | 相对移动相机 | 否 |
| `ChangeCameraSize` | `world`, `size_x`, `size_y` | `void` | 修改相机视口大小 | 否 |

### 5.4 世界物理参数

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `SetGravity` | `world`, `g_x`, `g_y` | `void` | 设置重力加速度 | **是** |
| `SetWorldSpeed` | `world`, `speed` | `void` | 设置模拟速度倍数 | 否 |
| `SetSimulateType` | `world`, `type` | `void` | 设置模拟模式（线程模式） | 否 |

### 5.5 对象属性操作

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `SetObjectPos` | `object`, `x`, `y` | `void` | 设置对象位置 | **是** |
| `GetObjectPos` | `object`, `*x`, `*y` | `void` | 获取对象位置 | 否 |
| `SetObjectSize` | `object`, `size_x`, `size_y` | `void` | 设置对象尺寸 | **是** |
| `GetObjectSize` | `object`, `*size_x`, `*size_y` | `void` | 获取对象尺寸 | 否 |
| `SetObjectV` | `object`, `v_x`, `v_y` | `void` | 设置对象速度 | **是** |
| `GetObjectV` | `object`, `*v_x`, `*v_y` | `void` | 获取对象速度 | **是** |
| `AddObjectV` | `object`, `v_x`, `v_y` | `void` | 增加对象速度 | 否 |
| `SetObjectA` | `object`, `a_x`, `a_y` | `void` | 设置对象加速度 | 否 |
| `GetObjectA` | `object`, `*a_x`, `*a_y` | `void` | 获取对象加速度 | **是** |
| `AddObjectA` | `object`, `a_x`, `a_y` | `void` | 增加对象加速度 | 否 |
| `SetObjectImage` | `object`, `image_path` | `int` | 设置对象纹理 | **是** |
| `ChangeObjectShowImage` | `object`, `image_id` | `void` | 切换显示的图片 | 否 |

### 5.6 碰撞检测

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `AddHitBox` | `object`, `x1`, `y1`, `x2`, `y2`, `hardness` | `void` | 添加线段碰撞盒 | 否 |
| `CreateRectHitbox` | `object`, `hardness` | `void` | 创建矩形碰撞盒 | **是** |

### 5.7 纹理渲染

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `SetBackground` | `world`, `image_path`, `x`, `y`, `size_x`, `size_y` | `void` | 设置世界背景图 | **是** |

### 5.8 信号系统

| 函数名 | 参数 | 返回值 | 功能说明 | PlatformerGame是否使用 |
|--------|------|--------|----------|----------------------|
| `AddSimulateSignalBefore` | `world`, `SimulateSignal` | `void` | 物理模拟前触发 | 否 |
| `AddSimulateSignalMid` | `world`, `SimulateSignal` | `void` | 物理模拟中触发 | **是** |
| `AddSimulateSignalAfter` | `world`, `SimulateSignal` | `void` | 物理模拟后触发 | 否 |
| `AddKeyboardSignal` | `world`, `KeyboardSignal` | `void` | 键盘事件信号 | **是** |
| `AddMouseSignal` | `world`, `MouseSignal` | `void` | 鼠标事件信号 | 否 |

### 5.9 按键常量定义

| 常量 | 值 | 说明 |
|------|-----|------|
| `KEY_SPACE` | 32 | 空格键 |
| `KEY_ENTER` | 257 | 回车键 |
| `KEY_ESCAPE` | 256 | ESC键 |
| `KEY_UP` / `KEY_DOWN` / `KEY_LEFT` / `KEY_RIGHT` | 265-262 | 方向键 |
| `KEY_A` - `KEY_Z` | 65-90 | 字母键 |
| `KEY_0` - `KEY_9` | 48-57 | 数字键 |
| `MOUSE_BUTTON_LEFT` | 0 | 鼠标左键 |
| `MOUSE_BUTTON_RIGHT` | 1 | 鼠标右键 |
| `MOUSE_BUTTON_MIDDLE` | 2 | 鼠标中键 |

---

## 六、编译步骤

### 6.1 使用预编译DLL构建（推荐）

```bash
# 创建并进入构建目录
mkdir build && cd build

# 生成 Makefile（Windows 使用 MinGW Makefiles）
cmake .. -G "MinGW Makefiles"

# 编译
mingw32-make

# 运行游戏（确保 libRPGEngine.dll 在同一目录）
./PlatformerGame.exe
```

### 6.2 从源码完整构建（需要 raylib）

```bash
# 进入引擎目录
cd RPGEngine/Egine_dll

# 创建构建目录
mkdir build && cd build

# 生成 Makefile
cmake .. -G "MinGW Makefiles"

# 编译引擎和游戏
mingw32-make

# 运行游戏
./PlatformerGame.exe
```

---

## 七、常见问题

### 7.1 运行时缺少 libRPGEngine.dll

**解决方案**:
- 将 `libRPGEngine.dll` 复制到可执行文件同级目录
- 或从 Egine_dll 重新编译生成

### 7.2 图片无法加载

**解决方案**:
- 确保图片文件（player.png, platform.png, background.png）与可执行文件在同一目录

### 7.3 链接错误：找不到 libRPGEngine

**解决方案**:

- 确保 `libRPGEngine.dll` 存在于 build 目录
- 检查 CMakeLists.txt 中的 `link_directories` 路径是否正确

---

## 八、完整使用流程模板

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

## 总结

RPG Engine 提供了简洁的 API 接口，适合快速开发 2D 游戏。PlatformerGame 示例展示了引擎的核心用法，包括：

1. **引擎初始化**：`Init()` + `IsRunning()` 等待窗口就绪
2. **世界创建**：`CreateWorld()` 创建游戏世界
3. **对象管理**：`CreateObject()` 创建对象，`SetObjectPos/Siz`e 设置属性，`AddEnvironment/AddCreature` 添加到世界
4. **物理系统**：`SetGravity()` 设置重力，`SetObjectV()` 控制速度
5. **信号系统**：`AddKeyboardSignal()` 和 `AddSimulateSignalMid()` 注册回调
6. **主循环**：`ShowWorld()` + `WaitForClose()` 启动游戏

推荐使用**预编译DLL**的方式开发游戏，这样不需要配置 raylib，开发更便捷。如果需要修改引擎源码，可以从 Egine_dll 目录完整构建。
