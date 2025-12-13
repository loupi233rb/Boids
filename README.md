# Boids

轻量级 Boids（群集）仿真项目，使用 C++ 和 EasyX（Windows）渲染。

## 仓库结构（简要）
- `src/` — 主要源文件（见 `src/main.cpp`、`src/bird.cpp`、`src/crossList.cpp`、`src/timer.cpp`）
- `include/` — 头文件（例如 `include/bird.h`、`include/crossList.h`、`include/timer.h`、第三方单文件库 `include/json.hpp`）
- `bin/` — 运行时文件，例如配置文件 `bin/config.json`
- `.vscode/` — VS Code 任务配置（`tasks.json`）
- 其它：`README.md`

## 快速上手

依赖
- Windows
- EasyX 图形库（确保 include/lib 路径与构建任务匹配，注意修改路径）
- TDM-GCC/MinGW 或其他支持的 GCC 工具链

构建（与 VS Code 任务一致）
- 使用 VS Code 任务：打开命令面板 → Run Build Task（参考 `.vscode/tasks.json`）
- 或在项目根目录运行（示例）：

```powershell
# from repository root (example using the provided task args)
C:\TDM-GCC-64\bin\g++.exe -g -I${PWD}\include -I F:/environment/easyX/include -L F:/environment/easyX/lib src/*.cpp -o bin/main.exe -leasyX -lwinmm
```

如果你使用 VS Code 任务，打开命令面板并运行 `Tasks: Run Build Task` 即可。

## 运行

构建后运行二进制：

```powershell
# 运行构建后的可执行文件
.\bin\main.exe
```

配置项位于 `bin/config.json`，程序会在启动时读取（参见 `src/main.cpp` 中的 `ReadSetting` 实现）。

## 主要模块（快速说明）
- 启动与配置
  - `src/main.cpp`：程序入口，读取配置并初始化仿真。
- Boid / 物体
  - `include/bird.h` / `src/bird.cpp`：`bird` 结构与行为实现；包含更新与渲染逻辑。
- 网格加速（空间划分）
  - `include/crossList.h` / `src/crossList.cpp`：稀疏十字链表（`CrossList<T>`）用于加速邻居搜索。
- 定时与帧率控制
  - `include/timer.h` / `src/timer.cpp`：高精度计时与帧率控制，控制逻辑步长与统计。
- 配置解析
  - `include/json.hpp`：嵌入的 nlohmann::json 单文件库，用于解析 `bin/config.json`。

## 配置（示例）
- 在 `bin/config.json` 中设置环境与鸟群参数（窗口大小、行为权重、感知半径、帧率等）。程序通过 `ReadSetting` 将这些值加载到 `EnvSetting` / `BirdSetting`。

## 开发与扩展建议
- 增加单元测试：建议引入 Catch2 或 GoogleTest，并为 `CrossList` 与关键逻辑编写测试用例。
- 配置热加载：支持在运行时重新加载 `config.json` 以便快速调参。
- 性能剖析：对 `bird::update` 与空间划分进行剖析，优化邻居查询与内存分配。
- 移植与跨平台：将 EasyX 替换为 SDL2 / OpenGL 等跨平台渲染库以便支持非 Windows 平台。

## 代码索引（快速定位）
- 程序入口：`src/main.cpp`
- Boid 实现：`include/bird.h`, `src/bird.cpp`
- 网格/十字链表：`include/crossList.h`, `src/crossList.cpp`
- 定时器：`include/timer.h`, `src/timer.cpp`
- JSON 库：`include/json.hpp`
- 运行配置：`bin/config.json`
- VS Code 构建任务：`.vscode/tasks.json`

## 许可与致谢
- `include/json.hpp` 使用 nlohmann::json
