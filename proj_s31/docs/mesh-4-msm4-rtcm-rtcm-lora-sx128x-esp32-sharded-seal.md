# SX128x RTCM Mesh 网络实施计划（v0.2 协议 / 纯 C / 借鉴 MeshCore 架构）

## Context

设计并实现一个 2.4GHz LoRa mesh 网络，业务：
- **RTCM MSM4 四系统修正流**：网关下行广播 1~2 KB/s，免重传
- **控制指令**：网关下行，端到端确认，必须到达
- **叶子定位上行**：1 Hz
- **RTCM 源两种模式**：① NRTK——root 经 WiFi/Ethernet 获取（主线 M0–M7，协议零改动）；② 自建基站——leaf/node 发起，UL_DATA 隧道上行到 root 再广播（v0.3 扩展，M8 验收）

已确认的决策：
| 项 | 决策 |
|---|---|
| 协议规范 | **v0.2 已定稿**（`D:\project\sx128x-mesh\docs\protocol_v0.2.md`），主线 M0–M7 不改协议只做实现；**RTCM 多源扩展新建 `protocol_v0.3.md`（v0.2 原文保留），M8 验收** |
| 架构蓝本 | **MeshCore**（事件循环/静态包池/优先级队列骨架）；Meshtastic 为对照（纯洪泛教训） |
| 语言 | **纯 C99**（工具链约束 + 学习目标），不依赖 Arduino/RadioLib |
| MCU | **全角色统一 ESP32-S31**（网关/中继/叶子同平台，ESP-IDF，纯 C）；GD32F4 为叶子低成本化的未来选项，本期不做 |
| PC 仿真 | **不做**，全部直接硬件验证 |
| 规模 | 小规模验证：≤10 叶子、≤3 跳、单网关 |

现状：`D:\project\sx128x-mesh\` 只有 `docs/protocol_v0.2.md`，零代码；`C:\Users\Roborock\.cursor\plans\sx128x_mesh_final_plan.plan.md` 的 8 项 todo 全部 pending。

统一 ESP32-S31 的收益：单一工具链（ESP-IDF）、单一 HAL、`MESH_ROLE` 宏编译三份固件；硬件 AES/HMAC 加速器（esp_aes）、TRNG 硬件随机数（JOIN 退避熵源）、512KB SRAM / 外挂 Flash 宽裕、双核 RISC-V 320MHz 可将 mesh 任务钉核运行；**片上千兆以太网 MAC（RMII/MII）让 NRTK root 的有线回传不需要外挂 SPI 以太网模块**（Wi-Fi 6 仅 2.4GHz，与 SX1280 同频段，作备选）。

## 关键架构决策

### D1：协议 v0.2 与 MeshCore 的分工

v0.2 是"RFC"（帧格式/超帧/确认/安全/join 全部照做）；MeshCore 只借**软件骨架**：

| 借鉴（保留） | 不借鉴（删除/替代） |
|---|---|
| 单 loop 事件驱动、中断只置标志（Dispatcher.cpp:66） | CAD 退避（Dispatcher.cpp:289-304）——TDMA 超帧已消除碰撞 |
| 单一在空包、TX 完成即取下一包 = burst（Dispatcher.cpp:86-131） | tx_budget 占空比（2.4GHz 无法规限制） |
| (priority, scheduled_for) 延迟队列（StaticPoolPacketManager.cpp:22-69） | calcRxDelay/SNR 竞争（Dispatcher.cpp:55-57） |
| 静态包池 free/tx/rx 三队列 | SHA256 去重哈希表 → v0.2 §5 的 (src,seq,retry) 32 项滑动位图更省 |
| ms 时钟回绕补码比较（Dispatcher.cpp:381-384） | 独立 ACK 包/multi-ACK → v0.2 §6.1 滑动窗口确认（ack 捎带 UL_STATUS，零开销） |
| TX 看门狗 1.5×airtime（Dispatcher.cpp:327,339） | MeshCore 无 SX128x 支持（platformio.ini:40 显式排除）→ 驱动全新编写 |

### D2：角色定制 = 编译期宏为主 + 4~6 个业务回调

不做 C++ 式虚函数/vtable。`MESH_ROLE_GW / MESH_ROLE_RELAY / MESH_ROLE_LEAF` 编译宏裁剪互斥代码（网关独有的 hw_id 映射/命令窗口/信道扫描不进叶子固件）；业务钩子用函数指针：`on_dl_cmd`（叶子幂等执行）、`on_rtcm_byte_ready`、`on_ul_report`、`on_event`。core/ 单一源码、三份固件产物。

### D3：超帧调度器（对照 Dispatcher loop 的 TDMA 化）

v0.2 §4"只定义 TX 义务、默认 RX"，`mesh_poll()` 骨架：
1. 消费 radio 中断标志：rx_done（记 µs 时间戳 → 读 FIFO → MIC 双试校验 → 父信标则校时 → 按包型分派 → 转发判定"下一跳是我 && ttl>0 && 位图未见"则入队）；tx_done（burst 中且队列非空且过 IFS → 发下一包；空 → 发信标收尾 → 回 RX）
2. TX 义务检查（全部相对父信标时间戳偏移）：UL slot / DL 转发窗口 / JOIN 时隙
3. housekeeping：丢信标计数与漂移外推（3 次判掉线 §9）、网关命令窗口补发（§6.1）、BUNDLE_ACK 未收重发（§7.2）、TX 看门狗

纪律：`mesh_poll()` 无阻塞、单次 <1ms（IFS=3ms 内要完成 RX 处理 + 下一包 SetTx）；日志异步（环形缓冲 + 后台输出）。

### D4：中断边界（时序正确性的命门）

DIO1 ISR 只做：读 IRQ 状态（清中断）、capture RX/TX-done 的 µs 时间戳（信标校时基准 §4.4，最高中断优先级 + IRAM_ATTR）、置 volatile 标志 / 唤醒 mesh 任务。**SPI 读 FIFO、CMAC、包处理、NVS 写全部在 mesh 任务上下文**；NVS 写（ms 级阻塞）只允许在 §4.3 时间轴空闲段执行。

### D5：包池尺寸（静态分配，无 malloc）

每包 ≈272B（255B 帧 + 元数据）：
- root：**32**（一超帧 DL burst 9~12 块 + 命令补发 + JOIN/BUNDLE 暂存；M8 起 +UL_DATA 收流暂存）
- node：**10**（边收边转的重叠峰值 + BUNDLE 2 fragment + 信标；M8 起源路径上的 node 加到 12）
- leaf：**4**（RX 1 + UL_STATUS 1 + JOIN 1 + 余量；RTCM 重组不占包，MIC 过即拷入 4KB 环形缓冲后 free；被配置为 RTCM 源时加到 6）

## 工程结构

```
D:\project\sx128x-mesh\
├── docs/protocol_v0.2.md          # 已有，协议 RFC
├── core/                          # 平台无关纯 C99，三角色同源
│   ├── config.h                   # v0.2 §13 常量唯一入口（M0 实测校准落点）
│   ├── mesh.c/h                   # 顶层状态机 + mesh_poll()（§4,§9,§12）
│   ├── frame.c/h                  # 帧编解码（§3）
│   ├── scheduler.c/h              # 超帧 TDMA：信标校时/DL burst/UL 栅格（§4）
│   ├── dl.c/h                     # RTCM 切流+环缓冲重组、DL_CTRL 滑动窗口（§5,§6）
│   ├── ul.c/h                     # UL_STATUS 组装、UL_BUNDLE 合并/ACK（§7）
│   ├── join.c/h                   # SCAN→JOIN→RUN、选父打分、漫游/掉线（§8,§9）
│   ├── security.c/h + aes_esp32.c # 密钥派生+CMAC（§10）；AES 用 esp_aes 硬件，保留软件表驱动实现作回退与互验
│   ├── chanmgr.c/h                # 51 信道扫描、CH_ANNOUNCE 迁移（§2,§11）
│   ├── pool.c/h                   # 静态包池 + 优先级延迟队列
│   └── tables.c/h                 # seen32 位图/候选信标表/子表/hw_id 映射/cmdwin（§12）
├── drivers/
│   ├── radio.h                    # 无线抽象：rx_continuous/tx(非阻塞)/irq标志/read_rx(含rssi,snr,ts_us)/airtime_ms/cad
│   └── sx1280.c                   # Semtech SWDR005 官方 C 驱动去阻塞化移植
├── platform/
│   ├── hal.h                      # hal_millis/micros/spi/dio1_cb/flash/hw_id/log
│   └── esp32s31/ hal_esp32s31.c + root_main.c/node_main.c/leaf_main.c（ESP-IDF + FreeRTOS：mesh_task 钉 Core 0 最高优先级，app_task/log_task 另核，无锁环形队列通信）
└── tools/                         # ping-pong 实测固件（M0）、GPIO 时序量测辅助
```

## SX1280 驱动方案

- **来源**：Semtech 官方 **SWDR005**（SX1280 开发套件软件包，含官方驱动源码，纯 C、无 OS 依赖）为一手来源；交叉参考 mbed-semtech/SX1280Lib 与 RadioLib SX128x 寄存器配置序列（只读，不链接——C++）。注意 ~~SWSD001~~ 是 SX126x/LR11xx 的 LoRa Basics Modem SDK，**不含 SX1280 驱动，勿用**
- **去阻塞化**：IRQ 全部走 DIO1 中断标志；仅保留 Busy 引脚 µs 级轮询
- **radio.h 映射**：`SetModulationParams(SF5,BW1625,CR4/5)`、`SetPacketParams(前导12,显式头,255B,CRC)`、`SetRfFrequency(CH0_HZ+k×CH_STEP_HZ)`（§2）、`SetRx(RX_CONTINUOUS)`、`SetDioIrqParams(TxDone|RxDone|CrcErr→DIO1)`、`GetPacketStatus`（RSSI/SNR——选父打分 §8.1 要用）、`SetCad`（网关选信道 §2）
- **RX-done 时间戳**：DIO1 ISR 内 capture 自由运行定时器（µs 级），SX1280 无硬件时间戳，禁止主循环补读
- **airtime**：M0 实测后三档插值表实现 `radio_airtime_ms()`
- SPI 10MHz（255B 读 FIFO ≈0.2ms），ESP32-S31 用 spi_master；**hw_id** 用 eFuse 内置 MAC（8B 对齐 §1）；**JOIN 退避熵源** esp_random()（TRNG）；**NetKey 保护** flash encryption（XTS-AES，验证期明文烧录、联调后启用 eFuse）

## 里程碑（M0–M8，对应原 8 项 todo + 多源扩展）

**M0 — 空口实测校准**（todo#1 = v0.2 §14 全部）
两块 ESP32+SX1280 ping-pong + GPIO 翻转量测。验收：① 24/128/255B 空中时间与 §2 偏差 <5%，回写 config.h；② IFS=3ms 内"SPI 读 255B + CMAC + 分派"实测 <2ms（ESP32-S31 用 esp_aes 硬件 AES，不够则 IFS_MS→4）；③ 250m 视距 255B PER 记录；④ burst 末尾信标到达抖动 <1ms（GUARD 足够性）；⑤ 重叠区信标接收率 vs RSSI 记录（60%/10dB 参数合理性）。

**M1 — radio 驱动**（todo#2）
radio.h 语义落地、全中断化。验收：双板 ping-pong 1h PER <0.5%（近距）；RX-done 时间戳抖动 <50µs（GPIO 量测）；CAD 能识别 WiFi 占用。

**M2 — frame + security**（todo#3）
验收：RFC 4493 AES-CMAC 官方测试向量全过（esp_aes 硬件与软件表驱动实现结果一致互验）；密钥派生（§10.1 三组）双板一致；互发帧 MIC 通过；SF/SF-1 边界双试用例通过；MIC 错误丢包+计数。

**M3 — 最小闭环**（todo#4）：网关 + 1 直挂叶子（跳过中继）
验收：① RTCM 下行：叶子重组流 seq 连续统计可观测，丢块后 0xD3 自同步恢复（§5）；② UL_STATUS 1Hz 十分钟到达率 >99%；③ 命令链路：DL_CMD→执行→ack_map 置位；注入丢包→下一超帧 DL 队首补发（retry=1）且**叶子不重复执行**（§6.1 幂等）；cmd_seq u8 回绕点用例；8 条未确认→上报告警。

**M4 — 调度器全量**（todo#5）
验收：① 校时后 UL slot 发射抖动 <1ms（GPIO 量测）；② 屏蔽信标 1~2 超帧：漂移外推保持同步、RX 窗放宽（§4.4）；③ 屏蔽 3 超帧判掉线（§9）；④ IFS=3ms 下 12 包 burst 无丢包。

**M5 — 中继链**（todo#6）：GW→R1→叶子（无仿真，直接硬件，故障注入靠拔天线/屏蔽箱）
验收：① 转发延迟=父 UL 窗口+GUARD 可观测；② 重复包（同 src,seq,retry）只转一次；③ UL_BUNDLE 合并+网关 BUNDLE_ACK 缺失时原 seq 重发且网关去重（§7.2）；④ 漏听父信标时 RX 空闲 >15ms 判尾并转发已收部分（§4.1 容错）。

**M6 — join 全状态机**（todo#7）
验收：① 冷启动入网 <10s；② 网关重启后叶子 hw_id 复用旧 node_id（§1）；③ 两叶子同时 JOIN 撞车→esp_random() 硬件 RNG 退避后先后成功（§8.2）；④ 拔中继天线→叶子 3 超帧判掉线按候选清单重选；⑤ 漫游 6dB 滞后+持续 2 超帧才切换；⑥ 中继换父子树不集体重入（UL_BUNDLE new_parent 上报 §9）。

**M7 — 信道迁移**（todo#8）
验收：① CH_ANNOUNCE 8 超帧公告后全网在 switch_superframe ±1 SF 内切换（实现层加固：切换边界新旧信道各听一超帧，协议不变）；② 全程错过公告的叶子丢信标 3 次后逐信道重扫 <51s 恢复；③ 网关重启先复查旧信道。

**M8 — RTCM 多源扩展**（协议 v0.3，设计见下节）
验收：① 基站以五种挂接位置（root 直挂叶子 / hop1 中继 / R1 下叶子 / hop2 中继 / R2 下叶子）轮换实测，RTCM epoch 完整率与 NRTK 基线对比不显著劣化；② SET_SRC 指令切换 NRTK↔基站↔另一基站，切换损失 ≤1 epoch，源节点按指令起流/停流；③ 最坏拓扑（hop3 源、2 KB/s 跑满、超帧占用 ~92%）压测 1h，UL_STATUS 到达率与命令确认不劣化；④ 路径中继上行去重：同 (src,seq) 只转一次；⑤ root 侧 K_node 验签 → K_dl 重签名下发，epoch 首块标志保留正确；⑥ 基站漫游换父后 hop 变化，窗口按新 hop 自动选择，流不中断超 2 超帧。

## RTCM 多源扩展（协议 v0.3 新建文件，M8 落地）

**背景**：RTCM 发起者不限于 root。模式① NRTK——root 经 WiFi/Ethernet 拉流注入 dl_feed_rtcm()，**协议零改动**，主线 M0–M7 即覆盖（部署注意：root 的 WiFi 与 SX1280 同处 2.4GHz 自干扰，回传**优先用 Ethernet**，WiFi 仅备选，信道按 §2 选 WiFi 1/6/11 主瓣缝隙）。模式② 自建基站——RTCM 由某 leaf/node 发起，需 v0.3 扩展。

**架构（五种挂接位置可行性已逐一核算，全部可行）**：基站沿树上行打隧道到 root（上行免路由：父指针即路由），root 验证 MIC 后重新分块，下一超帧走正常 DL burst 全网分发——下行完全复用 v0.2 机制。全网同一时刻仅一个活跃源，由 root 经 SET_SRC 指令指定/切换。

### v0.3 协议增量（相对 v0.2，新建 `docs/protocol_v0.3.md`，v0.2 原文保留）

| 位置 | 增量 |
|---|---|
| §3 包型 | 新增 UL_DATA 0x7：≤243 B RTCM 上行，`src`=基站 node_id，`dst`=0x01，ttl 置 0 |
| §4.3 超帧 | B1 之后新增 **UL RTCM 窗口链**：按 hop 固定分配，每跳窗口 130 ms + GUARD，深跳先发、逐跳向 root 递；只有本跳有 UL_DATA 待发的节点才在对应窗口发射，无源配置时窗口空置（默认 RX） |
| §6.2 | 新增 DL_CTRL 子类型 **SET_SRC 0x05**（mode(1): 0=NRTK/1=node + src_id(1)），走 §6.1 滑动窗口确认，可靠性零新增设计 |
| §5/§7 | 中继上行转发规则：收 UL_DATA 且 (src,seq) 未见 → 入队待本跳 UL RTCM 窗口转父；上行 seen 位图与下行分立（tables.c 加一项） |
| §10 | 源以 K_node_i 签名上行；root 验证后以 K_dl 重新签名下发，重新分块并**保留 epoch 首块标志** |

### 容量与时延核算（2 KB/s 全速率 = 9 块/超帧 ≈ 130 ms/跳）

| 基站位置 | 上行跳数 | 超帧总占用 | 余量 |
|---|---|---|---|
| root 直挂叶子 (hop1) | 1 | ~664 ms (66%) | 336 ms |
| hop1 中继 | 1 | ~664 ms (66%) | 336 ms |
| R1 下叶子 (hop2) | 2 | ~794 ms (79%) | 206 ms |
| hop2 中继 | 2 | ~794 ms (79%) | 206 ms |
| R2 下叶子 (hop3，最深) | 3 | **~924 ms (92%)** | 76 ms |

时延：任何位置相对 NRTK 统一多 ~1 个超帧（~1 s，等 root 下一个 DL 窗口），RTK 修正龄期可接受。文档写明"基站挂 hop3 时速率上限 2 KB/s、无富余"。中继作源（B/D 类）与 DL 转发/信标/BUNDLE 职责时间不重叠，零新增硬件职责。

### 实现增量

- `ul.c`：UL_DATA 组装/发送（源节点）+ 上行转发（路径中继）；`dl.c`：root 侧 UL_DATA 收流 → 重新分块 → DL 队列
- `tables.c`：上行 seen 位图（按 src 索引）；`scheduler.c`：UL RTCM 窗口链 TX 义务
- D2 新增回调 `on_rtcm_src_feed`：app_task 从基站 GNSS 接收机 UART 读 RTCM 喂 core；leaf 配置为源时包池 4→6（leaf 固件从纯收 DL_DATA 变为带一条 UL_DATA 发送链）
- SET_SRC 处理：root 侧切换输入源（NTRIP feed ↔ UL_DATA feed），DL seq 空间不变、下游无感；源节点侧按指令起流/停流；切换损失 ≤1 epoch，符合免重传语义
- 基站漫游换父后 hop 深度变化，窗口按"自己的 hop"选择，无需重新协商——天然兼容 §9 漫游

## ESP32-S31 平台要点

| 项 | 方案 |
|---|---|
| AES | esp_aes 硬件（CMAC 255B µs 级，IFS 瓶颈只剩 SPI + 任务唤醒）；软件表驱动实现作回退与互验 |
| 定时器 | esp_timer µs 级自由运行；DIO1 GPIO 中断最高优先级 + IRAM_ATTR |
| RTOS | FreeRTOS：mesh_task 钉 Core 0 最高优先级；app_task（RTCM 源接入/输出、命令业务）+ log_task 另核，无锁环形队列通信 |
| Flash 写 | NVS 关中断 ms 级 → 仅 §4.3 空闲段（hw_id 映射、信道号持久化） |
| 时钟 | 40MHz ±10ppm，满足 §4.4（GUARD 1ms 有 50 倍余量） |
| 熵源 | esp_random()（TRNG 硬件随机数，JOIN 退避、nonce） |
| NetKey 保护 | flash encryption（XTS-AES，验证期不用，联调后启用 eFuse） |
| RAM/Flash 预算 | 512KB SRAM（+ 可选封装内 16/32MB Octal PSRAM）/ 外挂 Flash 宽裕，三角色同源无压力 |
| Ethernet | **片上千兆 MAC（RMII/MII）**：NRTK root 有线回传首选，避开 WiFi/SX1280 2.4G 共存问题 |
| 新片风险 | S31 于 2026-07 量产、数据手册仍 Pre-release；时钟精度/外设细节以正式手册为准（见 R12） |

## 风险清单

- **R1~R5（协议层）**：v0.2 §14 五项待实测，全部压在 M0 收口；IFS/GUARD/门槛参数都在 config.h 宏里，实测后改一处
- **R6 RX-done 时间戳污染**：中断边界纪律（D4）+ M1 的 50µs 抖动验收兜底
- **R7 burst IFS 抖动**：mesh_poll 轮询周期 ≪1ms；长操作（RTCM 解析、printf）一律移出临界路径
- **R8 时钟回绕**：ms 用补码比较（借 Dispatcher.cpp:381-384）；superframe u32 回绕瞬间由 MIC 的 SF/SF-1 双试覆盖（§10.2）
- **R9 三角色同源回归**：core 任何改动须 gw/relay/leaf 三份固件都编译冒烟（本地脚本）
- **R10 JOIN 撞车退避相关性**：esp_random() 硬件 RNG（M6 验收③）
- **R11 多源最坏拓扑余量薄**：hop3 源 + 2 KB/s 跑满时超帧占用 ~92%，空闲段被压到 ~76ms（NVS 写/日志纪律更紧）→ M8 压测收口；NRTK 模式下 root 的 WiFi 与 SX1280 同处 2.4GHz 自干扰 → 部署优先 Ethernet（S31 片上千兆 MAC，成本低）
- **R12 ESP32-S31 平台新**：2026-07 才量产，数据手册 Pre-release、ESP-IDF 支持版本新 → ① 晶振精度/定时器/中断延迟等时序参数以 M0/M1 实测为准，不抄手册典型值；② 锁定一个验证过的 ESP-IDF 版本，工具链冻结；③ 双核为 RISC-V（非 Xtensa），IRAM_ATTR/钉核 API 语义相同但汇编层不同，写 HAL 时只用 ESP-IDF 官方接口

## 验证总策略

1. **M0 物理量测**：GPIO 翻转 + 逻辑分析仪（空中时间/IFS/校时抖动）
2. **M2 密码学**：RFC 4493 测试向量 + 双板互验 + 硬件/软件 AES 结果交叉验证
3. **M3~M7 行为验收**：每个里程碑的可观察判据（见上）；故障注入手段 = 拔天线、屏蔽、重启、注入重复包
4. **最终端到端**（M7 后）：真实 RTCM 源（如 u-blox 基站/NTRIP）→ 网关 → 3 跳链 → 叶子 UART 输出接 RTK 引擎，统计 epoch 完整率（目标对齐 §3 容量推算 ~83%+）与命令到达率 100%（补发机制生效）
5. **M8 多源端到端**：自建基站（u-blox base 挂 leaf/node）五种挂接位置轮换实测 + SET_SRC 指令切换 + hop3 源 92% 占用压测（判据见 M8 验收）

## 参考蓝本（只读）

- `D:\project\MeshCore\src\Dispatcher.cpp`（loop/burst/超时骨架）
- `D:\project\MeshCore\src\helpers\StaticPoolPacketManager.cpp`（静态池+优先级延迟队列）
- `D:\project\MeshCore\src\helpers\SimpleMeshTables.h`（去重表——本项目用 v0.2 §5 滑动位图替代）
