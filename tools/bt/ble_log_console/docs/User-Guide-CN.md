# BLE Log Console 用户指南

## 简介

BLE Log Console 是一个基于终端的实时 BLE 日志捕获与解析工具。它通过 UART DMA 接收 ESP32 固件发出的 BLE Log 帧，实时解析并展示在终端界面中，同时将原始二进制数据保存到文件供离线分析。

## 准备工作

### 1. 固件配置

在 `idf.py menuconfig` 中启用 BLE Log 模块：

**最简配置（推荐）：**

```
Component config → Bluetooth → BT Logs → Enable critical-log-only mode  [y]
```

勾选即可一键启用 BLE Log 模块 + UART DMA 输出 + 仅关键日志模式。

**手动配置：**

```
Component config → Bluetooth → BT Logs → Enable BLE Log Module (Experimental)  [y]
Component config → Bluetooth → BT Logs → BLE Log Module
  → Peripheral Selection → UART DMA
  → UART DMA Configuration
      → UART Port Number        (默认: 0)
      → Baud Rate               (默认: 3000000)
      → TX GPIO Number          (根据硬件设置)
```

| 配置项 | 推荐值 | 说明 |
|--------|--------|------|
| `CONFIG_BT_LOG_CRITICAL_ONLY` | `y` | 一键启用，包含 BLE Log + UART DMA |
| `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` | `3000000` | 3 Mbps，兼顾吞吐量和稳定性 |

> **关于 UART PORT 0**：当配置为 PORT 0 时，固件会自动将 `ESP_LOG` 输出包装为 BLE Log 帧（`REDIR` source），Console 会自动解码并显示为普通日志行。

### 2. 硬件连接

**PORT 0（推荐）：** 直接通过 USB 线连接开发板的 UART 口即可，无需额外接线。

**其他 PORT：** 需要将指定的 TX GPIO 连接到外部 USB 串口适配器：

```
ESP32 TX GPIO ──────── USB 串口适配器 RX
ESP32 GND     ──────── USB 串口适配器 GND
```

确保 USB 串口适配器支持所配置的波特率（默认 3 Mbps）。推荐使用 CP2102N、CH343 或 FT232H 芯片的适配器。

### 3. ESP-IDF 环境

使用前必须先 source ESP-IDF 环境：

```bash
cd <esp-idf 根目录>
. ./export.sh
```

无需额外安装依赖，`textual` 已包含在 ESP-IDF 核心依赖中。

## 启动

```bash
cd <esp-idf 根目录>
. ./export.sh
cd tools/bt/ble_log_console
```

### 交互模式（启动界面）

不带参数运行即可打开启动界面（Launch Screen），在 TUI 中选择串口、波特率和日志保存目录：

```bash
python console.py
```

### 直连模式（CLI）

传入 `--port` 跳过启动界面，直接开始捕获：

```bash
# 基本用法
python console.py -p /dev/ttyUSB0

# 指定波特率（须与固件配置一致）
python console.py -p /dev/ttyUSB0 -b 3000000

# 指定日志保存目录
python console.py -p /dev/ttyUSB0 -d /tmp/my_captures
```

| 参数 | 缩写 | 默认值 | 说明 |
|------|------|--------|------|
| `--port` | `-p` | （可选） | 串口设备路径，如 `/dev/ttyUSB0`、`COM3`。省略时打开启动界面。 |
| `--baudrate` | `-b` | `3000000` | 波特率，须与固件 `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` 一致 |
| `--log-dir` | `-d` | 当前工作目录 | 捕获文件保存目录 |

捕获文件保存到当前工作目录（或 `--log-dir` 指定的目录），文件名按时间戳自动生成：

```
ble_log_YYYYMMDD_HHMMSS.bin
```

## 界面说明

启动后，界面分为两个区域：

### 日志区域（上方）

滚动显示实时日志，包括：

- **`[INFO]`**（绿色）：系统信息，如固件版本
- **`[WARN]`**（黄色）：丢帧警告，格式为 `Frame loss [LL_TASK]: 5 frames, 200 bytes`，表示该 source 新增了丢帧
- **`[SYNC]`**（青色）：同步状态变化
- 普通文本：UART redirect 输出的 `ESP_LOG` 日志（仅 PORT 0 时出现）

### 状态栏（下方）

固定显示在底部，实时更新：

```
Sync: SYNCED  Checksum: XOR | Header+Payload
RX: 1.2 MB  Speed: 2.8 Mbps  Max: 2.9 Mbps  Rate: 3421 fps
Frame Loss: 12 frames, 480 bytes
```

- **Sync**: 同步状态（SEARCHING → CONFIRMING → SYNCED → CONFIRMING_LOSS）
- **Checksum**: 自动检测到的校验模式
- **RX**: 累计接收字节数
- **Speed / Max**: 当前/峰值传输速度
- **Rate**: 当前帧率
- **Frame Loss**: 自 Console 启动以来的累计丢帧数和丢失字节数

## 快捷键

| 按键 | 功能 |
|------|------|
| `q` | 退出 |
| `Ctrl+C` | 退出 |
| `c` | 清屏（清除日志区域） |
| `s` | 切换自动滚动（默认开启） |
| `d` | 打开每个 Source 的帧统计详情（按 `Escape` 或 `d` 关闭） |
| `h` | 显示快捷键帮助（按 `Escape` 关闭） |
| `r` | 通过 DTR/RTS 复位芯片 |

### 帧统计详情（`d` 键）

按 `d` 键会弹出一个覆盖层，以表格形式展示每个 BLE Log Source 自 Console 启动以来的写入和丢失统计：

```
┌─────────── Per-Source Frame Statistics (since console start) ───────────┐
│ Source    │ Written Frames │ Written Bytes │ Lost Frames │ Lost Bytes   │
│ LL_TASK   │          12345 │       56.7 KB │           5 │        200 B │
│ LL_HCI    │            890 │       34.2 KB │           0 │          0 B │
│ HOST      │            456 │       12.1 KB │           0 │          0 B │
│ HCI       │            234 │        8.5 KB │           2 │         80 B │
└─────────────────────────────────────────────────────────────────────────┘
```

有丢帧的行会以红色高亮显示。

## 查看历史捕获文件

```bash
python console.py ls
```

输出示例：

```
Captures in /tmp/ble_log_console:

  2026-03-17 14:30:25     2.3 MB  ble_log_20260317_143025.bin
  2026-03-17 10:15:03   512.0 KB  ble_log_20260317_101503.bin
```

这些 `.bin` 文件包含 UART 接收到的原始二进制数据，可使用 BLE Log Analyzer 的 `ble_log_parser_v2` 模块进行离线解析（HCI 日志提取、btsnoop 转换等）。

## 常见问题

### 状态一直停在 SEARCHING

- **波特率不匹配**：确认 `--baudrate` 与固件 `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` 一致
- **串口错误**：确认连接的是正确的 UART TX 引脚
- **固件未运行**：确认设备已启动且 BLE Log 已初始化
- **信号质量**：3 Mbps 下请使用短线缆，确保 GND 连接可靠

### 出现 Buffer overflow warning

表示解析器内部缓冲区累积超过 8 KB 未能解析出有效帧。通常发生在：

- 设备启动初期，UART 上还没有有效的 BLE Log 帧数据
- 波特率不匹配导致接收到的全是乱码

如果只在启动时出现一次，属于正常现象；如果持续出现，请检查波特率和硬件连接。

### 丢帧严重

- 按 `d` 查看各 Source 的丢帧详情
- 增大固件 buffer：`CONFIG_BLE_LOG_LBM_TRANS_SIZE`、`CONFIG_BLE_LOG_LBM_LL_TRANS_SIZE`
- 增加 LBM 数量：`CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT`
- 提高波特率（需适配器支持）

### 看不到 ESP_LOG 输出

- 确认固件配置了 `CONFIG_BLE_LOG_PRPH_UART_DMA_PORT=0`
- Console 会自动解码 REDIR 帧，无需额外配置
- 日志由 1 秒周期定时器刷新，可能有短暂延迟

### 提示 `ModuleNotFoundError: No module named 'textual'`

重新运行 ESP-IDF 安装脚本：

```bash
cd <esp-idf 根目录>
./install.sh
. ./export.sh
```
