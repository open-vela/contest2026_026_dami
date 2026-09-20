# 基于 ESP32-S3 Eye 的智能投屏设备

> 队伍编号：contest2026_026 | 队伍名称：dami | 赛道：AI 硬件产品创新

## 一、作品简介

本项目实现了一套基于 ESP32-S3 Eye 开发板的**智能投屏系统**，能够将 PC 桌面画面实时投射到开发板的 240×240 LCD 屏幕上。系统采用上位机/下位机架构：上位机（Linux PC）通过 X11 截取屏幕内容，经双线性插值缩放并转换为 RGB565 格式后，通过 WiFi TCP 协议传输至下位机；下位机运行 openvela RTOS，使用 LVGL 图形框架将接收到的图像数据实时渲染到 ST7789 LCD 屏幕。整个系统具备低延迟、实时显示的特点，展示了嵌入式设备作为智能显示终端的可能性。

## 二、选题方向

**AI 硬件产品创新**

基于 ESP32-S3 Eye 开发板，利用 openvela RTOS 和 LVGL 图形框架实现 PC 屏幕内容的实时投射。该项目展示了嵌入式设备作为低成本、便携式智能显示终端的潜力，可拓展至信息看板、远程监控、智能家居控制面板等场景。

## 三、系统架构

```
┌─────────────────────────────────────────────────────┐
│                    上位机 (Linux PC)                  │
│  X11 屏幕截图 → 双线性插值缩放 (240×240) → RGB565    │
│                   cm_server (TCP:8080)               │
└───────────────────────┬─────────────────────────────┘
                        │ WiFi (TCP Socket)
                        ▼
┌─────────────────────────────────────────────────────┐
│              下位机 (ESP32-S3 Eye)                    │
│  WiFi 连接 → Socket 接收 RGB565 → LVGL 渲染 → LCD   │
│                 cm_app (openvela RTOS)               │
└─────────────────────────────────────────────────────┘
```

## 四、技术栈

| 类别 | 技术 |
|------|------|
| 嵌入式操作系统 | openvela (基于 NuttX) |
| 图形框架 | LVGL (v9, RGB565 色彩格式) |
| 硬件平台 | ESP32-S3 Eye (Xtensa 双核, 240×240 ST7789 LCD) |
| 通信协议 | WiFi TCP Socket (端口 8080) |
| 上位机语言 | C (X11 截屏 + Socket 服务) |
| 调试工具 | Python (串口通信 + 截图工具) |
| 构建系统 | CMake + Make |

### openvela 能力使用

本项目深度使用了 openvela 的以下核心能力，符合大赛"基于 openvela 开发"的判定标准（落地图形核心能力）：

| openvela 能力 | 使用详情 |
|---------------|---------|
| **图形（LVGL 集成）** | 使用 openvela 原生 LVGL port，通过 `/dev/lcd0` Framebuffer 驱动 ST7789 LCD，实现 RGB565 图片渲染和文本终端 GUI |
| **WiFi 子系统** | 使用 WAPI（`wapi psk`/`wapi essid`）进行 WiFi 配网，通过 `netlib` 接口获取 IPv4 地址 |
| **LCD 驱动框架** | 使用 openvela 的 ST7789 LCD 驱动（SPI 接口），配置 Framebuffer 双缓冲 |
| **NuttX POSIX API** | 使用标准 POSIX socket（TCP）、文件 I/O、pthreads 等接口进行网络通信和多线程编程 |
| **Xtensa ESP32-S3 架构支持** | 基于 openvela 的 ESP32-S3 BSP，启用 SMP 双核、SPI RAM、USB 串口等特性 |

## 五、目录结构

- `app/camera_app/` — cm_app 主应用（WiFi 连接、Socket 通信、LVGL GUI 显示）
- `app/hello_app/` — Hello 示例应用（用于验证构建环境）
- `board/contest_board/` — 自定义开发板配置（板级初始化代码）
- `quickapp/hello_quickapp/` — 快应用形态样例
- `socket_proj/` — 上位机程序（X11 屏幕截图 + TCP 服务器 + RGB565 转换）
- `python/` — Python 串口调试工具（自动配网、时间同步、截图测试）
- `tools/` — 构建/烧录/配置脚本（`update.sh`、`build.sh`、`flash.sh`、`remove.sh`、`defconfig`）
- `logs/` — AI Coding 日志

## 六、运行方式

### 前置条件

**硬件要求：**
- ESP32-S3 Eye 开发板
- USB 数据线（Type-C 或 Micro-USB）
- 2.4GHz WiFi 网络（ESP32-S3 不支持 5GHz）

**软件要求：**
- openvela 开发环境（已配置好基础编译工具链）
- Xtensa ESP32-S3 工具链（`xtensa-esp32s3-elf`）
- Linux 系统 + X11 桌面环境（上位机截图需要）
- CMake ≥ 3.10（上位机编译需要）

### 下位机编译与烧录

该工程基于 openvela 开发，需确保已安装 openvela 的基础环境。编译与运行只需如下四步（请确保在 openvela 根目录执行）：

**（1）初始化项目**
```bash
sudo bash ./contest2026_026_dami/tools/update.sh
```
此脚本会：
- 记录你的 openvela env 路径和 Xtensa 工具链路径到 `cm_app_build.config`
- 复制 `defconfig` 到 `vendor/espressif/boards/esp32s3/esp32s3-eye/configs/openvela/`

**（2）构建固件**
```bash
sudo bash ./contest2026_026_dami/tools/build.sh
```

**（3）配置菜单**
在弹出的 `menuconfig` 界面中，勾选 `cm_app`（位于 Contest 2026 team 026 分类下）。

**（4）烧录固件**
```bash
sudo bash ./contest2026_026_dami/tools/flash.sh
```
烧录完成后会自动打开 minicom 串口终端。

### 上位机编译与运行

```bash
# 1. 进入上位机项目目录
cd /home/log/openvela/contest2026_026_dami/socket_proj/build

# 2. 安装依赖（X11 开发库）
sudo apt-get install imagemagick

# 3. 编译
cmake .. && make

# 4. 运行，等待 ESP32 连接
./cm_server
```

### 系统启动流程

1. 烧录完成后，通过串口终端输入 `cm_app <WiFi名称> <WiFi密码> <上位机IP>` 启动投屏
2. 下位机自动连接 WiFi 并与上位机建立 TCP 连接
3. 上位机开始持续截取屏幕并推送到下位机
4. 下位机将接收到的画面实时渲染到 LCD 屏幕

### 常见问题

**Q: 屏幕发灰或显示异常？**

执行以下步骤修复 LCD 初始化：
1. 运行 `build.sh` 打开 menuconfig，勾选 `LCD_ST7789_3WIRE`（Use 3 wire interface），保存并烧录
2. 启动板子后运行 `lvgldemo`，此时屏幕可能不亮
3. 退出后重新关闭 `LCD_ST7789_3WIRE`，再次烧录
4. 重新启动即可恢复正常

**Q: ESP32 无法与上位机连接？**

1. 如果上位机环境是 VMware 虚拟机中的 Linux，**必须使用桥接模式**，否则 ESP32-S3 客户端无法与上位机建立连接
2. 如果提示 `ERROR: netlib_obtain_ipv4addr()`，请检查 WiFi 是否为 **2.4GHz 频段**（ESP32-S3 不支持 5GHz）

### 演示效果

- 上位机启动 `cm_server` 后，ESP32-S3 Eye 连接 WiFi 并自动建立 TCP 连接
- PC 桌面画面实时投射到 240×240 LCD 屏幕，画面经过双线性插值缩放，色彩通过 RGB565 格式还原
- 投屏为持续推送模式，可实现基本的桌面内容同步显示

完整演示视频见项目根目录 `演示视频.mp4`。

## 七、AI Coding 使用说明

本项目在以下环节借助 AI 辅助开发：

- **需求拆解**：AI 协助将"屏幕投射"这一整体需求拆分为下位机 WiFi 连接、Socket 通信、LVGL 渲染、上位机截屏与转换等独立模块
- **方案设计**：AI 辅助设计上下位机通信协议，确定使用 0x80 请求码 + 2 字节图片信息头 + RGB565 像素数据的简洁数据格式
- **编码实现**：AI 辅助编写 socket_tools 通信库（TCP 连接管理、数据收发封装）、GUI 封装层（LVGL 初始化、图片渲染、文本终端）、上位机屏幕截图与色彩转换模块（X11 截图、双线性插值缩放、RGB888→RGB565 转换）
- **调试排查**：AI 协助解决 WiFi 配网失败、LCD 屏幕初始化顺序导致的显示异常、VMware 网络桥接等实际开发中的常见问题

完整对话日志见 `logs/` 目录。

**AI Skill 沉淀**：本项目暂未沉淀独立的 AI Skill 文件，AI 辅助开发的完整经验和对话记录均保留在 `logs/` 目录中供参考。

## 八、应用场景与商业潜力

本项目的投屏方案基于低成本 ESP32-S3 开发板（单价约 30-50 元），具备以下可落地的应用场景：

| 场景 | 说明 |
|------|------|
| **会议室状态看板** | 将会议室预约系统、日程安排等内容投射到门口的小型显示屏，替代昂贵的商用信息屏 |
| **智能家居控制面板** | 将 Home Assistant 等智能家居控制界面投射到墙壁嵌入式屏幕，实现低成本触控/显示终端 |
| **工业设备远程监控** | 将产线监控仪表盘远程投射到工位旁的嵌入式屏幕，方便操作员实时查看设备状态 |
| **教育/创客演示** | 用于嵌入式开发教学演示，学生可通过本项目学习 WiFi 通信、图形渲染、RTOS 开发等完整链路 |

技术方案具备规模化推广潜力：上位机可扩展为云端服务（替代单机 X11 截屏），下位机可适配更多带 LCD 的 openvela 支持硬件，形成通用的"云端→嵌入式屏幕"投屏方案。

## 九、许可协议

本项目基于 [Apache License 2.0](LICENSE) 开源发布。
