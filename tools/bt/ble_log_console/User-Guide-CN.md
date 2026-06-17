# BLE Log Console 用户指南

## 简介

BLE Log Console 是一个基于终端的实时 BLE 日志捕获与解析工具。它通过 UART DMA 接收 ESP 芯片固件发出的 BLE Log 帧，实时解析并展示在终端界面中，同时将原始二进制数据保存到文件供离线分析。

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
Component config → Bluetooth → BT Logs → Enable BT Log Async Output (Dev Only)  [y]
```

UART DMA 传输、3 Mbps 波特率、PORT 0 均为默认值，大多数情况下无需额外配置。

> **关于 UART PORT 0**：当配置为 PORT 0 时，固件会自动将 `ESP_LOG` 输出包装为 BLE Log 帧（`REDIR` source），Console 会自动解码并显示为普通日志行。

### 2. 硬件连接

**PORT 0（推荐）：** 直接通过 USB 线连接开发板的 UART 口即可，无需额外接线。

**其他 PORT：** 需要将指定的 TX GPIO 连接到外部 USB 串口适配器：

```
ESP32 TX GPIO ──────── USB 串口适配器 RX
ESP32 GND     ──────── USB 串口适配器 GND
```

确保 USB 串口适配器支持所配置的波特率（默认 3 Mbps）。推荐使用 CP2102N、CH343 或 FT232H 芯片的适配器。

## 启动

### 快速启动（推荐）

使用自带的启动脚本 -- 自动激活 ESP-IDF 环境并安装依赖，可在任意目录下运行：

```bash
# Linux / macOS
<esp-idf 根目录>/tools/bt/ble_log_console/run.sh

# Windows
<esp-idf 根目录>\tools\bt\ble_log_console\run.bat
```

不带参数启动时，工具会打开 **启动界面（Launch Screen）** -- 一个交互式配置界面，可以：

- 从下拉列表中 **选择串口**（自动检测可用设备，支持 **Refresh** 按钮重新扫描）
- 从预设选项中 **选择波特率**（115200 至 3000000，默认 3000000）
- 通过文本输入或 **Browse** 文件选择器 **设置日志保存目录**
- 点击 **Connect** 开始捕获

传入 `--port` 可跳过启动界面，直接开始捕获：

```bash
# Linux / macOS
./run.sh -p /dev/ttyUSB0
./run.sh -p /dev/ttyUSB0 -b 3000000 -d /tmp/my_captures

# Windows
run.bat -p COM3
```

所有 CLI 选项均会转发给 `console.py`。

### 手动启动

如果你希望自行管理 ESP-IDF 环境：

```bash
cd <esp-idf 根目录>
. ./export.sh
python -m pip install textual textual-fspicker      # 安装额外依赖
cd tools/bt/ble_log_console
python console.py                                  # 启动界面
python console.py -p /dev/ttyUSB0                  # 直连捕获
python console.py -p /dev/ttyUSB0 -b 3000000       # 指定波特率
python console.py -p /dev/ttyUSB0 -d /tmp/captures  # 指定日志目录
```

### CLI 选项

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
Sync: SYNCED | Checksum: XOR / Header+Payload | Press h for help
RX: 1.2 MB  Speed: 293.0 KB/s  Max: 300.0 KB/s  Rate: 3421 fps  Lost: 12 frames, 480 B
```

- **Sync**: 同步状态（SEARCHING -> CONFIRMING -> SYNCED -> CONFIRMING_LOSS）
- **Checksum**: 自动检测到的校验模式（算法 / 范围）
- **RX**: 累计接收字节数
- **Speed / Max**: 当前/峰值传输速度（单位 KB/s 或 MB/s）
- **Rate**: 当前帧率
- **Lost**: 自 Console 启动以来的累计丢帧数和丢失字节数

## 快捷键

| 按键 | 功能 |
|------|------|
| `q` | 退出 |
| `Ctrl+C` | 退出 |
| `c` | 清屏（清除日志区域） |
| `s` | 切换自动滚动（默认开启） |
| `d` | 打开每个 Source 的帧统计详情（按 `Escape` 或 `d` 关闭） |
| `m` | 查看缓冲区利用率（按 `Escape` 或 `m` 关闭） |
| `h` | 显示快捷键帮助（按 `Escape` 关闭） |
| `r` | 通过 DTR/RTS 复位芯片 |

### 帧统计详情（`d` 键）

按 `d` 键会弹出一个覆盖层，包含两个表格，每秒自动刷新：

**固件计数器（自芯片启动以来）** -- 固件上报的每个 Source 的写入和缓冲区丢帧统计：

| Source | Written Frames | Written Bytes | Buffer Loss Frames | Buffer Loss Bytes |
|--------|---------------|---------------|-------------------|-------------------|
| LL_TASK | 12345 | 56.7 KB | 5 | 200 B |
| LL_HCI | 890 | 34.2 KB | - | - |
| HOST | 456 | 12.1 KB | - | - |
| HCI | 234 | 8.5 KB | 2 | 80 B |

**Console 测量（自 Console 启动以来）** -- Console 端测量的每个 Source 的接收速率和峰值突发：

| Source | Received Frames | Received Bytes | Avg Frames/s | Avg Bytes/s | Peak Frames/10ms | Peak Bytes/s |
|--------|----------------|---------------|-------------|------------|-----------------|-------------|
| LL_TASK | 12340 | 56.5 KB | 412 | 18.8 KB/s | 8 | 24.0 KB/s |
| LL_HCI | 890 | 34.2 KB | 30 | 1.1 KB/s | 3 | 3.6 KB/s |

有缓冲区丢帧的 Source 会在固件计数器表格中以红色高亮显示。

### 缓冲区利用率（`m` 键）

按 `m` 键会弹出一个覆盖层，展示固件上报的每个 LBM（Log Buffer Manager）缓冲区利用率：

| Pool | Idx | Name | Peak | Total | Util% |
|------|-----|------|------|-------|-------|
| COMMON_TASK | 0 | spin | 3 | 4 | 75% |
| COMMON_TASK | 1 | atomic[0] | 4 | 4 | 100% |
| COMMON_ISR | 0 | spin | 2 | 4 | 50% |
| LL | 0 | ll_task | 4 | 4 | 100% |
| LL | 1 | ll_hci | 2 | 4 | 50% |

- **Pool**: 缓冲池类别（COMMON_TASK、COMMON_ISR、LL、REDIR）
- **Idx**: 池内 LBM 索引
- **Name**: LBM 可读名称（spin、atomic[N]、ll_task、ll_hci、redir）
- **Peak**: 同时在途的传输缓冲区峰值数量
- **Total**: 该 LBM 可用的传输缓冲区总数
- **Util%**: Peak / Total 的百分比；100%（红色高亮）表示所有缓冲区曾同时被占用，可能导致丢帧

此功能有助于诊断丢帧时哪个缓冲池资源不足。

## 打包为独立可执行文件

使用自带的构建脚本可将 BLE Log Console 打包为单文件可执行程序，目标机器无需安装 Python 或 ESP-IDF 环境：

```bash
# Linux / macOS
<esp-idf 根目录>/tools/bt/ble_log_console/build.sh

# Windows
<esp-idf 根目录>\tools\bt\ble_log_console\build.bat
```

脚本会自动激活 ESP-IDF 环境、安装 PyInstaller、构建可执行文件、将其放置在当前工作目录下，并清理中间产物。

输出：`./ble_log_console`（Linux/macOS）或 `.\ble_log_console.exe`（Windows）。

> **注意**：请在与目标机器相同的操作系统和架构上构建。PyInstaller 不支持交叉编译。

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
- 按 `m` 查看缓冲区利用率 -- 100% 的池需要增加缓冲区
- 增大固件 buffer：`CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE`、`CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE`
- 增加 LBM 数量：`CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT`
- 提高波特率（需适配器支持）

### 看不到 ESP_LOG 输出

- 确认固件配置了 `CONFIG_BLE_LOG_PRPH_UART_DMA_PORT=0`
- Console 会自动解码 REDIR 帧，无需额外配置
- 日志由 1 秒周期定时器刷新，可能有短暂延迟


