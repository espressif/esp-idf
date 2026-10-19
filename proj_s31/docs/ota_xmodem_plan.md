# XModem OTA 方案（ESP32-S31-Function-CoreBoard-1）

> 状态：已定稿（串口拆分）
> 板卡：[ESP32-S31-Function-CoreBoard-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s31/esp32-s31-function-coreboard-1/user_guide.html)
> 涉及目录：`proj_s31/main/common/ota/`、`cmd_system/cmdmgr.*`

## 结论变更

| 项 | 旧方案 | 新方案 |
|---|---|---|
| XModem 物理口 | 与 console 共用 UART0 | **独立 UART1** |
| console | OTA 前 `cmdmgr_stop`，失败后重建 + 重注册命令 | **始终占用 UART0，不停 REPL** |
| PC 侧 | 同一 COM 口交替 monitor / sx | **两路串口**：monitor + OTA |

AB 分区、`esp_ota_*` 写槽核心、rollback / `ota_mark_valid` **不变**。

## 硬件分配（本板）

| 用途 | 控制器 | GPIO | 板端落点 | PC 侧 |
|---|---|---|---|---|
| Console / flash / log | UART0 | TX=58, RX=59 | 板载 USB-UART（Type-C UART 口） | `idf.py monitor` |
| XModem OTA | UART1 | **TX=47, RX=48** | J2 **#13=GPIO47**, **#14=GPIO48** | USB-TTL 转接 |
| 共地 | — | GND | J2 #1/#2/#8 等 | USB-TTL GND |

接线（交叉）：

```text
USB-TTL TX  →  J2 GPIO48 (U1RX)
USB-TTL RX  →  J2 GPIO47 (U1TX)
USB-TTL GND →  J2 GND
```

默认波特率：**115200 8N1**（可经 Kconfig 改；先稳再提速）。

### 为何选 47/48

- J2 相邻引出，飞线方便
- 避开 UART0（58/59）、BOOT（61）、RGB LED（60）、SDIO 组（20–25）
- UART1~3 均可经 GPIO matrix 映射；实现里用 `UART_NUM_1` + `uart_set_pin`

若与后续外设冲突，只改 Kconfig 引脚，不必改协议层。

## 软件架构（不变的分层）

```text
cmd_ota  ──►  ota_start(OTA_TYPE_XMODEM)
                    │
              T_OTA_MODE_CTX (ota_xmodem)
                    │  transport: uart_read/write(UART1)
                    ▼
              xmodem_recv(...) ──write_cb──► ota.c 写 ota_x 槽
                    │
              stop_cb(success) → set_boot_partition / abort
```

- `ota.c`：唯一写 flash 入口（begin/write/end/abort）
- `ota_xmodem.c`：只负责 UART1 + XModem
- `ota_https.c`：后续复用同一套回调，与串口无关

## 实现要点

### 1. `ota_xmodem.c`

- `T_OTA_XMODEM_CTX`：`uart_port=UART_NUM_1`，`tx_gpio`/`rx_gpio`/`baud` 来自 Kconfig
- `__start` 中：
  1. `uart_driver_install` + `uart_param_config` + `uart_set_pin`
  2. 调 `start_cb`（`esp_ota_begin`）
  3. 填 `xymodem_transport_t`（`uart_read_bytes` / `uart_write_bytes`）
  4. `xmodem_recv`；块数据走 `write_cb`
  5. `stop_cb(success)`；`uart_driver_delete`
- **禁止**在传输路径上对 UART1 打 `ESP_LOG`/`printf`（日志走 UART0 即可）
- 失败：abort 槽位，UART1 卸载，console 不受影响

### 2. Kconfig（`main/common/Kconfig` 或 `ota/Kconfig`）

```
config OTA_XMODEM_UART_PORT
    int
    default 1
config OTA_XMODEM_TX_GPIO
    int
    default 47
config OTA_XMODEM_RX_GPIO
    int
    default 48
config OTA_XMODEM_BAUD
    int
    default 115200
```

### 3. `cmdmgr`

- OTA **不再调用** `cmdmgr_stop` / `cmdmgr_start`
- 现有 stop/start 可保留作调试工具，或后续删除；与 OTA 解耦
- `cmd_ota` 在 console 里注册一次即可（`register_ota()` → `cmdmgr_init`）

### 4. Console 命令与主机流程

设备（UART0 monitor）：

```text
esp32> ota xmodem
# 提示：在 UART1 @115200 等待 XModem…
# 进度日志可继续打在 monitor
```

主机（另一 COM，对应 USB-TTL）：

```bash
# 示例：sx 发送（lrzsz）
sx -X build/proj_s31.bin < /dev/ttyUSBx > /dev/ttyUSBx
```

成功：设启动分区 → 重启 → `app_main` 里 `ota_mark_valid()`。  
失败：monitor 仍可用，可重试。

## 阶段对照（相对原计划）

| 阶段 | 内容 | 相对原计划 |
|---|---|---|
| A | `ota.c` 写槽核心 | 不变（已基本完成） |
| B | `ota_xmodem` + UART1 transport | **改**：端口/引脚；去掉对 cmdmgr 的依赖 |
| C | CMake + Kconfig 引脚 | **改**：增加 UART GPIO/baud |
| D | `cmd_ota` + `mark_valid` | **改**：去掉 REPL 停启 |
| E | 真机双槽互升 | **改**：两路串口验证 |
| F | `ota_guide.md` | 含本板接线表 |

## 风险与注意

1. 飞线尽量短；先 115200，稳定后再提波特率  
2. XModem 无法区分尾部填充，`ota_write` / `esp_ota_end` 依赖镜像校验，勿自行截断协议填充除非确认库行为  
3. 若只用一根 USB 线：可临时退回「命令内同步收、共用 UART0」，但不作为本板默认方案
