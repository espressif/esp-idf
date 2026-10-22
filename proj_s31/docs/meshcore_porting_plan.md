# MeshCore 路由核心纯 C99 重构移植计划（proj_s31 / esp32s31）

> 版本：v1.0（2026-10-22）
> 状态：待评审 → 批准后按里程碑 M0→M4 执行
> 蓝本源码：`D:\project\MeshCore\`（WSL: `/mnt/d/project/MeshCore/`），meshcore-dev 官方 v1.18.0（tag `companion-v1.18.0`）

---

## 1. 背景与决策

### 1.1 为什么做

proj_s31 需要一个 LoRa mesh 网络承载 RTCM 等自有业务。原自研 v0.2/v0.3 协议方案（`docs/mesh-4-msm4-rtcm-rtcm-lora-sx128x-esp32-sharded-seal.md`，SX1280 + TDMA 超帧）**废弃**，改为重构移植 MeshCore 的成熟路由核心，理由：

- MeshCore 的 flood+direct 混合路由、静态包池、事件驱动 Dispatcher 已经过多硬件平台实战验证
- 核心仅 ~2500 行 C++，重构工作量可控
- 省去自研协议的协议验证成本，业务方专注 RTCM 数据面

### 1.2 已确认决策（不再变更）

| 项 | 决策 |
|---|---|
| 移植方式 | **纯 C99 重写** MeshCore 核心，不引入 Arduino / RadioLib / C++；风格对齐 ESP-IDF |
| 功能范围 | **只要路由核心**（事件循环、包池、flood/direct 路由、advert、identity）；应用层换自有业务（RTCM 等） |
| 与旧方案关系 | **取代** v0.2 自研协议；`main/app/mesh/` 空骨架被本方案填充 |
| 运行形态 | **嵌入现有 proj_s31 应用**，与 WiFi/Ethernet/控制台/OTA 共存，非独立固件 |
| 射频芯片 | **LR2021 或 LR2022**（Semtech 新一代，sub-GHz LoRa + 2.4G FLRC）；RadioLib 是 C++ 不用，vendor Semtech 官方纯 C 驱动 |
| 线格式 | 逐字节对齐 MeshCore 空中格式，保留与真 MeshCore 节点互通的可能 |

### 1.3 关键协议事实（从源码核实，移植时必须保持一致）

1. **线格式**（`Packet::writeTo/readFrom`）：`header(1B) | [transport_code(2B)×2] | path_len(1B) | path | payload`，最大 255B。`path_len` 高 2bit = hash 尺寸-1（0→1B, 1→2B, 2→3B, 3=非法拒绝），低 6bit = hash 个数。
2. **包哈希** = SHA256(type_byte ‖ payload) 截断 8B——去重表、ACK crc 全部依赖它。
3. **`PAYLOAD_TYPE_RAW_CUSTOM`(0x0F) 只在 direct 路由下被处理，flood 不转发**（Mesh.cpp:292-298）。RTCM 广播需要 patch 核心放行 flood（见 §5.4）。
4. **`Identity.cpp` 注释标明 orlp 纯 C 版 `ed25519_verify()` 有 memory corruption bug**（被 `#elif 0` 禁用），线上用 rweather/Crypto 的 C++ 实现。vendor 纯 C 版后必须跑 RFC 8032 KAT（见 §8.1）。
5. **RTCClock 只需 epoch 秒**；`getCurrentTimeUnique()` 的「时间不前进则 last_unique+1」语义必须保留（advert 时间戳去重依赖）。
6. Dispatcher 的 duty-cycle 机制：`tx_budget_ms` 在 1h 滑窗内按 `1/(1+factor)` 补充——正好对应 sub-GHz 法规占空比，必须保留。
7. **millis 回绕**：一律 `(int32_t)(now - ts) > 0` 补码比较法。
8. TRACE 处理里 `offset = (u16)path_len << path_sz` 需用 16 位防回绕（v1.11 修过的 bug，照抄）。

### 1.4 MeshCore 默认射频参数（platformio.ini）

`LORA_FREQ=869.618 MHz, LORA_BW=62.5 kHz, LORA_SF=8, CR=4/5`。本项目默认值对齐之，Kconfig 可改。

---

## 2. 源码盘点与移植判定

MeshCore 核心层共 ~2500 行 C++（`src/` 根目录）+ helpers，逐一判定如下：

| 源文件 | 行数 | 内容 | 判定 | 落地模块 |
|---|---|---|---|---|
| `src/Dispatcher.h/.cpp` | 595 | 事件循环、RX score 延迟入队、TX 优先级调度、airtime 预算、CAD 忙等待、radio 卡死看门狗（8s） | **全量移植** | `mc_dispatcher.c/h` |
| `src/Mesh.h/.cpp` | 968 | 包型分发、flood 去重/转发（追加自身 path hash，转发优先级=hop 数）、direct 路由（removeSelfFromPath + 最高优先级转发）、advert 验签、create*/send* API | **移植核心+裁剪**（§5.3） | `mc_mesh.c/h` |
| `src/Packet.h/.cpp` | 200 | header 位域、path_len 编码、writeTo/readFrom、SHA256 包哈希 | **全量移植** | `mc_packet.c/h` |
| `src/Identity.h/.cpp` | 256 | ed25519 sign/verify/key_exchange、LocalIdentity | 移植，Stream 读写改 NVS blob | `mc_identity.c/h` |
| `src/Utils.h/.cpp` | 338 | sha256、AES-128-ECB、encryptThenMAC/MACThenDecrypt（2B 截断 HMAC-SHA256）、hex | 移植（AES→esp_aes，SHA/HMAC→mbedtls） | `mc_crypto.c/h`、`mc_utils.c/h` |
| `src/MeshCore.h` | 128 | 常量、MainBoard 抽象 | 常量→`mc_core.h`；MainBoard→`mesh_port.h` | — |
| `helpers/StaticPoolPacketManager.*` | 170 | 三队列（unused/send_queue/rx_queue）+ 静态包池，队列项 {packet*, pri u8, schedule u32}，get() 取「已到期中 pri 最小」者 | **全量移植**，new→静态数组 | `mc_pool.c/h` |
| `helpers/SimpleMeshTables.h` | 76 | 160×8B 环形去重表（wasSeen/markSeen/clear）+ dup 计数 | **全量移植** | `mc_tables.c/h` |
| `helpers/radiolib/RadioLibWrappers.*` | 365 | radio 抽象语义：volatile 状态机（IDLE/RX/TX_WAIT + INT_READY）、噪声地板 64 样本滑动平均、CAD、AGC 复位 | **语义移植，实现重写** | `mesh_radio.h` |
| `helpers/radiolib/CustomLR2021*.h` | 240 | LR2021 怪癖处理（见 §6.3） | 作为 glue 需求清单参考 | `radio_lr2021.c` |
| `lib/ed25519/` | ~4500 | orlp/ed25519 纯 C89/C99 零依赖（fe/ge/sc/sha512/sign/verify/keypair/key_exchange/add_scalar/seed + `ed25519_derive_pub`） | **直接 vendor** | `thirdparty/ed25519/` |
| 其余 helpers（BaseChatMesh、CommonCLI、ArduinoSerialInterface、ClientACL、IdentityStore、ConfigSerializer、RegionMap、AdvertDataHelpers、TxtDataHelpers、UTF8Helpers、Board/RTC/bridges/ui/wifi/ethernet） | — | 应用层/平台胶水 | **不移植** | — |

---

## 3. 目标目录结构

```
proj_s31/
├── main/
│   ├── CMakeLists.txt            # 加 include(app/CMakeLists.txt)；MAIN_SRCS += ${APP_SRCS}
│   ├── Kconfig                   # source main/app/Kconfig
│   ├── app_main.c                # 插入 mesh 初始化（§9.2）
│   └── app/
│       ├── Kconfig               # menu "app"（§9.3 全文）
│       ├── CMakeLists.txt        # set(APP_SRCS/APP_INCLUDE_DIRS)，不调 idf_component_register
│       └── mesh/                 # 复用现有空骨架（frame.c/mesh.c/config.h 纳入新结构）
│           ├── include/
│           │   └── mesh_app.h        # 对业务层暴露的唯一公共头
│           ├── core/                 # ★ 零 ESP-IDF 依赖（只用 stdint/string/math），可 host 单测
│           │   ├── mc_core.h             # MeshCore.h 常量部分（~60 行）
│           │   ├── mc_packet.c/h         # ← Packet.*（~150 行）
│           │   ├── mc_pool.c/h           # ← StaticPoolPacketManager（~180 行）
│           │   ├── mc_tables.c/h         # ← SimpleMeshTables（~100 行）
│           │   ├── mc_identity.c/h       # ← Identity.*，无 Stream（~130 行）
│           │   ├── mc_crypto.c/h         # ← Utils 加解密（~130 行）
│           │   ├── mc_utils.c/h          # ← hex/parse 工具（~80 行）
│           │   ├── mc_dispatcher.c/h     # ← Dispatcher.*（~350 行）
│           │   └── mc_mesh.c/h           # ← Mesh.* 裁剪后（~550 行）
│           ├── radio/
│           │   ├── mesh_radio.h          # radio ops 结构体（§6.1）
│           │   └── radio_lr2021.c/h      # Semtech 驱动 glue + DIO ISR（~500 行）
│           ├── port/
│           │   ├── mesh_port.h           # millis/rng/epoch/log 接口
│           │   ├── mesh_port_esp.c       # esp_timer/esp_random/SNTP time（~150 行）
│           │   └── mesh_store_nvs.c      # identity NVS 存取（~80 行）
│           ├── app/
│           │   ├── mesh.c / mesh.h       # mesh_task、初始化、业务回调注册（~250 行）
│           │   ├── mesh_cmd.c            # 控制台命令（~200 行）
│           │   ├── dl_cmd.c/h            # 下行指令：root 重传队列 + leaf 幂等（§10.1，~200 行）
│           │   ├── ul_stat.c/h           # 上行状态：周期/告警发送（§10.2，~120 行）
│           │   └── frame.c/h             # RTCM 分片/重组（业务层，~150 行）
│           └── config.h                  # 改由 Kconfig 宏注入
└── thirdparty/
    ├── ed25519/                  # vendor MeshCore/lib/ed25519（平铺 18 个文件）
    │   ├── CMakeLists.txt        # 仿 thirdparty/xymodem/CMakeLists.txt
    │   └── *.c *.h license.txt
    └── lr2021/                   # vendor Semtech Lora-net LR2021 C 驱动（~8k 行，原版不改）
        ├── CMakeLists.txt
        ├── src/  include/
        └── port/lr2021_hal_esp.c # SPI/RESET/BUSY/DIO 的 ESP-IDF 实现
```

规模：新增 C99 代码 ~2.6k 行；vendor 代码 ed25519 ~4.5k 行 + LR2021 驱动 ~8k 行（不改）。

SRAM 预算（512KB 宽裕）：

| 项 | 计算 | 大小 |
|---|---|---|
| 包池 | 24 × ~264B（header+len+transport+path64+payload184+snr+对齐） | ~6.3 KB |
| 三条队列 | 3 × 24 × 9B | ~0.7 KB |
| 去重表 | 160 × 8B | 1.3 KB |
| mesh_task 栈 | ed25519 verify 栈 ~3KB + 余量 | 8 KB |
| **合计** | | **< 20 KB** |

---

## 4. core/ 模块移植规格

### 4.1 mc_core.h（← MeshCore.h 常量）

- 包型枚举 `MC_PAYLOAD_TYPE_*`（已核对 Packet.h:19-32）：REQ(0x00)/RESPONSE(0x01)/TXT_MSG(0x02)/ACK(0x03)/ADVERT(0x04)/GRP_TXT(0x05)/GRP_DATA(0x06)/ANON_REQ(0x07)/PATH(0x08)/TRACE(0x09)/MULTIPART(0x0A)/CONTROL(0x0B)/RAW_CUSTOM(0x0F)
- 路由类型 `MC_ROUTE_*`：FLOOD/DIRECT/TRANSPORT_FLOOD/TRANSPORT_DIRECT
- header 位域宏：`MC_HEADER_ROUTE_MASK/PH_TYPE_MASK/PH_VER_MASK` 及 get/set 内联函数
- 上限常量：`MAX_PATH_SIZE(64)`、`MAX_PAYLOAD_SIZE(184)`、`MAX_TRANS_UNIT(255)`、去重表尺寸

### 4.2 mc_packet.c/h（← Packet.*，~150 行）

```c
typedef struct {
    uint8_t  header;
    uint8_t  transport_codes[2];
    uint8_t  path_len;          /* 高2bit=hash_size-1, 低6bit=count */
    uint8_t  path[MAX_PATH_SIZE];
    uint16_t payload_len;
    uint8_t  payload[MAX_PAYLOAD_SIZE];
    float    snr;               /* 仅本地，不上线 */
} mc_packet_t;

uint16_t mc_packet_write_to(const mc_packet_t *p, uint8_t *dest);   /* 返回线长 */
bool     mc_packet_read_from(mc_packet_t *p, const uint8_t *src, uint16_t len);
void     mc_packet_calc_hash(const mc_packet_t *p, uint8_t *out8);  /* SHA256(type‖payload) 截8B */
/* path 操作 */
int      mc_packet_get_route_type(const mc_packet_t *p);
int      mc_packet_get_payload_type(const mc_packet_t *p);
uint8_t  mc_packet_path_hash_size(const mc_packet_t *p);
```

要点：readFrom 必须拒绝 hash_size 编码=3（reserved）；writeTo/readFrom 往返 = 恒等（host 单测覆盖）。

### 4.3 mc_pool.c/h（← StaticPoolPacketManager，~180 行）

```c
typedef struct { mc_packet_t *pkt; uint8_t priority; uint32_t scheduled_for; } mc_qitem_t;

void         mc_pool_init(void);                    /* 静态数组，尺寸 = CONFIG_MESH_PACKET_POOL_SIZE */
mc_packet_t *mc_pool_alloc(void);                   /* unused 队列取；空 → NULL */
void         mc_pool_free(mc_packet_t *pkt);
void         mc_pool_queue_tx(mc_packet_t *pkt, uint8_t priority, uint32_t scheduled_for_ms);
void         mc_pool_queue_rx(mc_packet_t *pkt, uint8_t priority, uint32_t scheduled_for_ms);
mc_packet_t *mc_pool_get_next_tx(uint32_t now_ms, uint32_t *delay_out);  /* 已到期中 pri 最小 */
mc_packet_t *mc_pool_get_next_rx(uint32_t now_ms, uint32_t *delay_out);
uint8_t      mc_pool_free_count(void);              /* status 命令用 */
```

语义照抄：`get()` 返回「scheduled_for ≤ now 中 priority 最小」者；全部到期项为空时经 delay_out 告知最近到期时间（驱动 mesh_task 睡眠时长）。**C 无析构兜底，所有 alloc 路径必须有配对的 free/queue，评审重点。**

### 4.4 mc_tables.c/h（← SimpleMeshTables，~100 行）

```c
void mc_seen_init(void);
bool mc_seen_has_seen(const uint8_t hash[8]);
void mc_seen_mark_seen(const uint8_t hash[8]);
uint32_t mc_seen_dup_count(void);   /* status 用 */
```

160×8B 环形数组 + 线性/二分（照抄原实现）。重启即清，不持久化。

### 4.5 mc_crypto.c/h（← Utils.* 加解密部分，~130 行）

```c
/* AES-128-ECB 单块，esp_aes 硬件 */
void mc_aes128_encrypt(uint8_t *block, const uint8_t key[16]);
void mc_aes128_decrypt(uint8_t *block, const uint8_t key[16]);
/* encryptThenMAC / MACThenDecrypt：2B 截断 HMAC-SHA256，mbedtls */
int  mc_encrypt_then_mac(const uint8_t *key16, uint8_t *data, uint32_t *len, uint32_t max);
int  mc_mac_then_decrypt(const uint8_t *key16, uint8_t *data, uint32_t len);
/* SHA256（包哈希用） */
void mc_sha256(const uint8_t *data, size_t len, uint8_t out[32]);
```

保留一条 mbedtls 软件 AES 路径（Kconfig 开关）与 esp_aes 硬件互验（M1）。

### 4.6 mc_identity.c/h（← Identity.*，~130 行）

```c
typedef struct { uint8_t pub_key[32]; uint8_t prv_key[64]; } mc_identity_t;

void mc_identity_generate(mc_identity_t *id);                     /* seed=esp_fill_random → ed25519_create_keypair */
int  mc_identity_sign(const mc_identity_t *id, const uint8_t *msg, size_t len, uint8_t sig[64]);
bool mc_identity_verify(const uint8_t pub[32], const uint8_t *msg, size_t len, const uint8_t sig[64]);
void mc_identity_shared_secret(const mc_identity_t *id, const uint8_t peer_pub[32], uint8_t out[32]); /* key_exchange */
int  mc_identity_store_save(const mc_identity_t *id);             /* → port 层 NVS */
int  mc_identity_store_load(mc_identity_t *id);                   /* 0=ok, 1=not found */
```

### 4.7 mc_dispatcher.c/h（← Dispatcher.*，~350 行，全量）

保留机制清单：
- `loop()` 单次迭代：noise floor 采样 → TX 完成处理 → 到期 RX 队列 → checkRecv → checkSend
- TX 前 `is_receiving()` 判忙（前导检测 + CAD + RSSI vs 噪声地板阈值）；CAD 忙超 4s 记 `ERR_EVENT_CAD_TIMEOUT` 强制发
- RX score 延迟入队：`calcRxDelay(snr, len)` 用 powf()（链接 libm；可留查表优化项）
- airtime 预算：`tx_budget_ms` 1h 滑窗按 `1/(1+factor)` 补充（factor=Kconfig）
- TX 看门狗 1.5×airtime；radio 卡死看门狗（8s 非 RX 状态报警 err_flags）
- burst：TX 完成即取下一到期包

```c
typedef struct mc_dispatcher mc_dispatcher_t;  /* 不透明 */
void mc_dispatcher_init(mc_dispatcher_t *d, const mc_radio_ops_t *radio, void *radio_ctx,
                        const mc_port_t *port);
void mc_dispatcher_loop(mc_dispatcher_t *d);           /* 单次迭代，无阻塞，<1ms */
uint32_t mc_dispatcher_next_wakeup_ms(mc_dispatcher_t *d); /* 供 task notify 超时 */
/* 统计/错误 */
uint32_t mc_dispatcher_get_err_flags(mc_dispatcher_t *d);
float    mc_dispatcher_get_noise_floor(mc_dispatcher_t *d);
uint32_t mc_dispatcher_get_airtime_budget_remaining(mc_dispatcher_t *d);
```

### 4.8 mc_mesh.c/h（← Mesh.* 裁剪后，~550 行）

对外 API（供 app 层）：

```c
typedef struct {
    const mc_identity_t *identity;
    const mc_radio_ops_t *radio;  void *radio_ctx;
    const mc_port_t *port;
    /* 回调（替代 C++ virtual） */
    void (*on_advert_recv)(const uint8_t pub[32], float rssi, float snr, const uint8_t *app_data, uint8_t len, void *ctx);
    void (*on_raw_data_recv)(const mc_packet_t *pkt, void *ctx);        /* RAW_CUSTOM */
    void (*on_peer_data_recv)(const uint8_t pub_hash, const uint8_t *data, uint16_t len, void *ctx);
    void (*on_ack_recv)(const mc_packet_t *pkt, uint32_t ack_crc, void *ctx);  /* direct 包送达确认，§10.1 重传匹配依据 */
    bool (*allow_packet_forward)(const mc_packet_t *pkt, void *ctx);    /* 中继角色返回 true */
    uint32_t (*get_retransmit_delay)(const mc_packet_t *pkt, void *ctx);
    void *cb_ctx;
} mc_mesh_cfg_t;

void mc_mesh_init(const mc_mesh_cfg_t *cfg);
void mc_mesh_loop(void);                                  /* = dispatcher 单次迭代 + mesh 分派 */
/* 发送 */
int  mc_mesh_send_flood(const uint8_t *payload, uint16_t len, uint8_t path_hash_size /*1..3*/);
int  mc_mesh_send_zero_hop(const uint8_t *payload, uint16_t len);
int  mc_mesh_send_direct(const uint8_t dest_hash, const uint8_t *path, uint8_t path_len,
                         const uint8_t *payload, uint16_t len);
int  mc_mesh_advert(void);
/* 状态 */
void mc_mesh_get_stats(mc_mesh_stats_t *out);
```

内部保留机制：flood 去重（calc_hash → seen 表）→ 未见过则转发（追加自身 pub hash，转发 pri=hop 数）；direct 命中自身 path 首 hash 则 removeSelfFromPath 后 pri=0 转发；advert 验签失败丢弃；PATH/path-return 学习对端路径；ACK 与 multipart ACK 中继。

---

## 5. 裁剪清单与核心 patch

### 5.1 保留

flood 去重+转发、zero-hop、direct 路由、advert 验签、path discovery/return、ACK + multipart ACK 中继、TRACE（Kconfig 可关）、transport codes 线格式解析（占 4B 保互通）、REQ/RESPONSE 的 ECDH 单播数据面（peer 表由业务层固定数组提供，不做 contact 管理）。

### 5.2 裁剪

GroupChannel / GRP_TXT / GRP_DATA / searchChannelsByHash；TransportKeyStore；MULTIPART 通用分片（仅留 multipart ACK 分支；业务分片由 frame.c 自做）；CONTROL 零跳发现子集（advert 替代）；ANON_REQ（Kconfig 关闭，省 ~50 行）；全部应用层 helpers。

### 5.3 virtual → 函数指针映射

| C++ virtual | C 替代 |
|---|---|
| `onAdvertRecv/onPeerDataRecv/onRawDataRecv` | cfg 回调（§4.8） |
| `allowPacketForward()` | cfg 回调，**中继角色必须返回 true**（repeater/companion 分野） |
| `getRetransmitDelay()` | cfg 回调，默认实现 = score 公式 |
| `getCurrentTime/Unique` | port 层（SNTP epoch + last_unique） |

### 5.4 必须 patch 核心的一处

`RAW_CUSTOM`(0x0F) 在 MeshCore 里 flood 不转发。新增 `CONFIG_MESH_FLOOD_RAW_CUSTOM`（默认 y）放行 flood 路由，否则 RTCM 广播只能 zero-hop/direct。**M4 前定案**（影响 frame.c 的 type 字段设计）；备选方案是借用 GRP_DATA + 固定组密钥，但会把裁剪掉的组通道代码请回来，不推荐。

---

## 6. radio 抽象层与 LR2021 移植

### 6.1 mesh_radio.h 接口（语义逐项对应 RadioLibWrappers.cpp）

```c
typedef struct {
    bool     (*begin)(void *ctx);
    int      (*recv_raw)(void *ctx, uint8_t *buf, int sz);        /* 消费 INT_READY；返回长度或 0 */
    bool     (*start_send_raw)(void *ctx, const uint8_t *buf, int len); /* 立即返回 */
    bool     (*is_send_complete)(void *ctx);
    void     (*on_send_finished)(void *ctx);                      /* 收尾回 RX */
    bool     (*is_in_recv_mode)(void *ctx);
    bool     (*is_receiving)(void *ctx);                          /* 前导/CAD/RSSI 判忙 */
    int      (*get_est_airtime_for)(void *ctx, int len);          /* ms，SF/BW/CR/preamble 公式 */
    float    (*packet_score)(void *ctx, float snr, int len);      /* snr 阈值表+碰撞罚，照抄 */
    float    (*get_last_rssi)(void *ctx);
    float    (*get_last_snr)(void *ctx);
    float    (*get_noise_floor)(void *ctx);
    void     (*trigger_noise_floor_calibrate)(void *ctx, int threshold);
    void     (*reset_agc)(void *ctx);
    void     (*set_cad_enabled)(void *ctx, bool en);
    int      (*set_params)(void *ctx, uint32_t freq_hz, uint32_t bw_hz, uint8_t sf, uint8_t cr);
    int      (*set_tx_power)(void *ctx, int8_t dbm);
} mc_radio_ops_t;
```

### 6.2 LR2021 驱动移植步骤（M0 主体）

1. **Vendor**：Semtech Lora-net 官方 LR2021 C 驱动。M0 第一步确认确切 repo 与 commit，pin 进 `thirdparty/vendor.lock.json`（API 仍在演进，不追新）。
2. **HAL 层**：`lr2021_hal_esp.c` 实现驱动所需原语（spi_write/read、reset、wait_on_busy、irq 注册），基于 `esp_driver_spi` + `esp_driver_gpio`；SPI 总线 mutex 内嵌（korvo 衍生板屏/codec 可能共总线）；SPI ≤10MHz 起步。
3. **去阻塞化**：RX done / TX done / CAD done 改走 DIO IRQ 标志（RadioLib `setPacketReceivedAction(setFlag)` 语义）；仅保留 BUSY 引脚 µs 级轮询。
4. **怪癖处理**（来自 CustomLR2021.h/Wrapper，逐项落实）：
   - `startReceive` 前必须先 standby；
   - `readData` 后芯片仍停留 RX，**勿再调 startReceive**（报 -706）；
   - preamble 规则：SF≤8 → 32 symbol，否则 16；
   - `calcMaxPacketMillis` 双超时（preamble-detect→header-valid、header-valid→rx-done）写入超时寄存器；
   - setParams 后重进 RX 前 standby；
   - rx boost 增益档位默认 7；
   - side detector（多 SF 并行监听）列 P2，初版不做。
5. **FLRC（2.4G）为第二阶段**，LoRa sub-GHz 先行。

### 6.3 CAD/LBT 与法规

- sub-GHz LoRa 硬件 CAD 有效；FLRC 无 CAD，回退 RSSI 阈值判忙。
- 868MHz（EU）0.1%/1%/10% 子带占空比：由 Dispatcher airtime 预算兜底，`MESH_AIRTIME_BUDGET_FACTOR` 按区域出厂配置（EU 1% 子带设 ~99；默认 1=50% 偏保守）。915MHz（US）单信道 fixed 部署需自行确认合规。2.4G FLRC 无占空比限制。

---

## 7. port 层（ESP-IDF 适配）

```c
typedef struct {
    uint32_t (*millis)(void);                 /* esp_timer_get_time()/1000，u32 截断 */
    uint64_t (*micros)(void);
    void     (*random_bytes)(uint8_t *buf, size_t len);   /* esp_fill_random (TRNG) */
    uint32_t (*time_epoch)(void);             /* time(NULL)，SNTP 未同步返回 0 */
    uint32_t (*time_epoch_unique)(void);      /* 保留 getCurrentTimeUnique 单调语义 */
    void     (*log)(int level, const char *fmt, ...);     /* ESP_LOGx */
} mc_port_t;
```

`mesh_store_nvs.c`：NVS namespace `"mesh"`，key `"id"`，blob = prv64‖pub32 共 96B。优于 littlefs：原子、不会被 FTP/filemgr 误删。去重表不持久化（重启即清，符合 MeshCore 行为）。

advert 时间戳：mesh_task 内等待 `time(NULL) > 1.7e9` 再发首个 advert，不阻塞启动流程。

---

## 8. crypto 与身份

### 8.1 ed25519 vendor 与验证（最高优先级正确性风险）

- 直接复制 `MeshCore/lib/ed25519/` 全部文件（平铺 18 个：add_scalar/fe/ge/key_exchange/keypair/sc/seed/sha512/sign/verify + 头文件 + precomp_data.h + license.txt）。
- **Identity.cpp 注释标明该纯 C 版 `ed25519_verify()` 有 memory corruption bug**（`#elif 0` 禁用，线上用 rweather/Crypto C++ 版）。M1 强制验证：
  1. RFC 8032 官方测试向量 KAT 跑 `ed25519_sign/ed25519_verify`；
  2. 与 rweather 输出交叉验证同密钥/消息；
  3. `key_exchange` 双向一致性（A 私 B 公 == B 私 A 公）。
- 若 verify 不过：备胎 **monocypher**（单文件、公域、纯 C），替换工作量 < 换 rweather C 化。
- ed25519 自带 sha512 与 mbedtls 无符号冲突（`sha512_init` 等命名不同），编译时确认即可。

### 8.2 AES/SHA

- AES-128-ECB → `esp_aes` 硬件（µs 级）；保留 mbedtls 软件路径做 M1 互验。
- SHA256 / HMAC-SHA256（2B 截断 MAC）→ mbedtls。
- 熵源：`esp_fill_random()`（TRNG）生成 identity seed。

---

## 9. 构建集成

### 9.1 CMake

`main/app/CMakeLists.txt`（仿 common/bsp，不调 idf_component_register）：

```cmake
# Do not call idf_component_register here; only set variables.
if(CONFIG_APP_MESH_ENABLE)
    set(APP_SRCS
        app/mesh/core/mc_packet.c
        app/mesh/core/mc_pool.c
        app/mesh/core/mc_tables.c
        app/mesh/core/mc_identity.c
        app/mesh/core/mc_crypto.c
        app/mesh/core/mc_utils.c
        app/mesh/core/mc_dispatcher.c
        app/mesh/core/mc_mesh.c
        app/mesh/radio/radio_lr2021.c
        app/mesh/port/mesh_port_esp.c
        app/mesh/port/mesh_store_nvs.c
        app/mesh/app/mesh.c
        app/mesh/app/mesh_cmd.c
        app/mesh/app/frame.c
    )
    set(APP_INCLUDE_DIRS app/mesh/include app/mesh/core app/mesh/radio app/mesh/port app/mesh/app)
else()
    set(APP_SRCS "")
    set(APP_INCLUDE_DIRS "")
endif()
```

`main/CMakeLists.txt`：顶部 `include(${CMAKE_CURRENT_LIST_DIR}/app/CMakeLists.txt)`；`MAIN_SRCS` 追加 `${APP_SRCS}`；`MAIN_PRIV_REQUIRES` 追加 `ed25519 lr2021 mbedtls`（条件：mesh 使能时）。

`thirdparty/ed25519/CMakeLists.txt` 与 `thirdparty/lr2021/CMakeLists.txt` 仿 `thirdparty/xymodem/CMakeLists.txt`（`idf_component_register(SRCS ... INCLUDE_DIRS ...)`）。`thirdparty/SX1280X/` 空目录为旧方案残留，不占用，可另行清理。

### 9.2 app_main.c 初始化插入点

在 `cmdmgr_init()` 之后、`bsp_wifimgr_init()` 之前：

```c
#if CONFIG_APP_MESH_ENABLE
    mesh_store_init();                              /* NVS 读 identity，无则生成落 NVS */
    radio_lr2021_init(&spi_cfg, &pin_cfg);          /* SPI/reset/DIO ISR */
    mesh_app_init(&mesh_cfg);                       /* 组装 port/radio/pool/tables/identity */
    mesh_cmd_register();                            /* 控制台命令 */
    mesh_task_start();                              /* 钉 Core 0, prio 23, stack 8K */
#endif
```

依赖：`nvs_flash_init()`（已有）在前；SNTP 未同步时首个 advert 延迟（任务内等待）。

### 9.3 Kconfig（main/app/Kconfig，由 main/Kconfig source 引入）

```
menu "app"
    config APP_MESH_ENABLE        bool  "Enable MeshCore routing core"   default n
    if APP_MESH_ENABLE
    config MESH_RADIO_CHIP        choice "Radio chip"  (LR2021 / LR2022)
    config MESH_LORA_FREQ_HZ      int   default 869618000
    config MESH_LORA_BW_KHZ       int   default 62      (62.5kHz)
    config MESH_LORA_SF           int   default 8
    config MESH_LORA_CR           int   default 5       (4/5)
    config MESH_LORA_TX_DBM       int   default 14
    config MESH_SPI_HOST          int   default 2
    config MESH_PIN_CS/SCK/MISO/MOSI/RST/BUSY/DIO1  int
    config MESH_PACKET_POOL_SIZE  int   default 24      (中继 32)
    config MESH_DEDUP_TABLE_SIZE  int   default 160
    config MESH_ROLE_FORWARD      bool  "Allow packet forwarding (repeater)" default y
    config MESH_FLOOD_RAW_CUSTOM  bool  "Flood-route RAW_CUSTOM payloads"    default y
    config MESH_AIRTIME_BUDGET_FACTOR int default 1     (EU 868 1% 子带设 99)
    config MESH_TASK_PRIO/_CORE/_STACK  int default 23/0/8192
    config MESH_ENABLE_TRACE      bool  default y
    config MESH_ENABLE_ANON_REQ   bool  default n
    config MESH_AES_USE_HARDWARE  bool  default y
    endif
endmenu
```

### 9.4 任务/中断模型（沿用旧设计文档 D4 纪律）

- **DIO1 ISR**：`gpio_isr_handler`，IRAM_ATTR，最高优先级；只做 `state |= INT_READY; xTaskNotifyFromISR(mesh_task)`。**SPI 读 FIFO、清 IRQ、包处理全在 mesh_task 上下文**；BUSY 仅 µs 级轮询；ISR 绝不碰 SPI。
- **mesh_task**：`xTaskCreatePinnedToCore(core=0, prio=23, stack=8K)`；主循环 `ulTaskNotifyTake(timeout=mc_dispatcher_next_wakeup_ms())` → `mc_mesh_loop()` 单次迭代（无阻塞、<1ms）。
- **业务接口**：业务任务调 `mesh_app_send(dest_hash|broadcast, payload, len)` 投 xQueue，mesh_task 统一 alloc→send；收包经 cfg 回调派发，回调内不做长操作（RTCM 字节拷入环形缓冲后立即返回）。
- 共存：mesh_task 不持 SPI 总线锁超过一次事务；OTA 写 flash 期间 LoRa RX 可能丢包，靠 ACK 重传/上层容错，接受。

### 9.5 控制台命令（mesh_cmd.c，走 cmdmgr 注册模式，参照 cmd_nvs.c）

| 命令 | 说明 |
|---|---|
| `mesh status` | 池水位、队列深度、TX/RX/dup 计数、airtime 余量、噪声地板、err_flags |
| `mesh id show` / `mesh id new` / `mesh id set <prv_hex>` | NVS identity 管理 |
| `mesh advert` | 立即 flood 一次 advert |
| `mesh send <dst_hash\|-> <hex\|text>` | 发 RAW_CUSTOM（`-`=flood 广播） |
| `mesh peers` | 最近 advert 见过的节点（hash, rssi, snr, age） |
| `mesh radio [freq\|bw\|sf\|cr\|dbm <v>]` | 查询/临时改射频参数 |
| `mesh trace <path>` | M3 后 |
| `lr2021 pingpong <role>` | M0 测试夹具（临时命令，可不并入正式命令表） |

---

## 10. 业务数据面设计（指令下发 / 状态上传）

分工原则：**核心只提供单包级原语**（direct 路由、`create_ack`/`on_ack_recv`、path return、ECDH 数据面）；「指令必须到达」「状态周期上传」的端到端语义由业务层实现。MeshCore 官方同样如此——`createAck()` 在核心（Mesh.cpp:560、Mesh.h:189），但「收到数据回 ACK」「未收 ACK 重发」在应用层（BaseChatMesh.cpp:43 `sendAckTo`）。

### 10.1 下行指令（root→leaf，必须到达）

新模块 `app/dl_cmd.c/h`（root 侧重传队列 + leaf 侧幂等，~200 行）。

root 侧命令队列项：

```c
typedef struct {
    uint8_t  dest_hash;
    uint8_t  cmd_seq;         /* u8 回绕，leaf 幂等判重依据 */
    uint8_t  retry;
    uint8_t  acked;
    uint32_t expected_ack;    /* SHA256(明文‖发送方公钥)截4B，on_ack_recv 匹配依据；attempt 变则重算 */
    uint32_t next_retry_ms;
    uint16_t payload_len;
    uint8_t  payload[64];
} dl_cmd_t;
```

流程：业务调 `dl_cmd_send(dest, payload)` → 组 direct 包（path 来自核心 path return 学习；无 path 时先 flood 寻路）→ 算 `expected_ack` 入 pending 队列 → `on_ack_recv` 匹配 → 出队。

**重传机制**（核心需求）：
- **背景**：MeshCore 原版固件**不做自动重传**——`BaseChatMesh` 只维护单个 `txt_send_timeout`，超时仅回调 `onSendTimeout()`（companion_radio 里是空实现），重试由手机 App 以 `attempt+1` 重新下发（MyMesh.cpp:898/1135）。RTCM 场景无人值守，**自动重传队列是本方案相对蓝本的必要增强**，但以下三个机制照抄蓝本：
- **attempt 防去重表吞包**（BaseChatMesh.cpp:451）：重发时把 `attempt` 编进 payload，使每次重发的包哈希不同，否则重发包被中继去重表丢弃
- **expected_ack 算法**（BaseChatMesh.cpp:462）：`expected_ack = SHA256(明文payload ‖ 发送方公钥) 截断 4B`——收发双方各自独立可算，ACK 只带 4B 即匹配；**替代此前设计的「crc32(原包哈希)」**（接收方无需缓存原包）
- **超时公式**（companion MyMesh.cpp:888-896，常量 108-111）：flood = `500 + 16×airtime`；direct = `500 + (6×airtime + 250) × (hop数+1)`；M0 实测校准后可调
- 重发上限 8 次，耗尽后 `on_event(DL_CMD_FAIL)` 上报告警
- 重传走正常 TX 队列，受 Dispatcher airtime 预算约束（自动限速，不冲爆占空比）

leaf 侧幂等执行：
- 每 root 维护 `last_cmd_seq`（u8 回绕按 `(int8_t)(seq - last) > 0` 比较）
- 重复 seq：只回 ACK，不执行
- ACK payload = expected_ack(4B) + 捎带 cmd_seq；**收到重复包也必须回 ACK**（否则 root 重发到上限）

广播指令（flood 无 ACK，如全网切源）：leaf 在上行状态包捎带 `last_executed_cmd_seq`；root 对账，超时未收齐确认的节点改 direct 单点补发。

### 10.2 上行状态（leaf→root）

新模块 `app/ul_stat.c/h`（~120 行）：

| 业务 | 机制 | 说明 |
|---|---|---|
| 周期状态（1Hz 定位，容忍丢包） | RAW_CUSTOM flood / zero-hop | 免确认最省空口；10 叶子 × 1Hz 小包占用极低 |
| 关键告警（必须到达） | RAW_CUSTOM direct + ACK + 重发 | 复用 dl_cmd 重发队列（方向相反）；path 来自 advert/path return |
| 大块数据（基站 RTCM 隧道） | frame.c 分片成多个 direct 包 | MULTIPART 通用分片已裁，业务自做 |

### 10.3 对核心回调表的补充

§4.8 `mc_mesh_cfg_t` 增补 `on_ack_recv(const mc_packet_t *pkt, uint32_t ack_crc, void *ctx)`（原语核心已有，裁剪不清除：Mesh.cpp:394/560、Mesh.h:167/189）。

## 11. 里程碑与验收（全程硬件实测，无 PC 仿真）

### M0 — radio 裸通（thirdparty/lr2021 + hal_esp + 去阻塞化）

交付：`thirdparty/lr2021/`、`thirdparty/ed25519/`、`lr2021 pingpong` 命令。
验收：① 两板互发 24/128/255B，实测 airtime 与公式偏差 <5%；② RX 全程 IRQ 驱动无轮询（GPIO 翻转 + 逻辑分析仪佐证）；③ RSSI/SNR 读数合理；④ 24h ping-pong 无 -706/卡死（standby 怪癖处理生效）。

### M1 — 核心编译 + 单跳 flood

交付：core/ 全部 C99 化；host 单测（包编解码往返、去重表、优先级队列、**RFC 8032 KAT**、AES 硬软互验、key_exchange 双向一致性）；两板 advert 互通。
验收：① host 测试全绿；② 板 B `mesh peers` 出现板 A hash（验签通过）；③ 重复包 dup 计数++ 不处理；④ `mesh status` 计数正确；⑤ **ed25519 verify 风险在此关闭**。

### M2 — 多跳 flood

交付：三板 A-B-C 链（A、C 互不可达），B 开 `MESH_ROLE_FORWARD`。
验收：① C 收到 A 的 advert，path 含 B 的 hash（len=2）；② 无广播风暴：dup 计数收敛、airtime 预算不超；③ score 延迟下 B 转发延迟分布合理。

### M3 — path/direct 路由

交付：path discovery + path return + direct REQ/ACK + TRACE。
验收：① A 学到 C 的 path 后 direct 单播送达；② ACK 沿反向 path 返回；③ `mesh trace` 各跳 SNR 正确；④ 拔天线断径后回退 flood 重发现。

### M4 — 业务接入

交付：RTCM payload 经 RAW_CUSTOM flood（依 §5.4 patch）接入、frame.c 分片重组、dl_cmd/ul_stat 业务链路（§10）、与 WiFi/Ethernet/OTA/SNTP 共存。
验收：① RTCM 端到端吞吐/延迟/丢包达业务指标（250m 视距 PER 记录）；② 指令链路：注入丢包（拔天线）→ 重发后到达且叶子**不重复执行**（cmd_seq 幂等生效）；连续 8 次未确认 → root 上报告警；u8 cmd_seq 回绕点用例通过；③ 上行状态：周期包到达率统计、告警包 direct+ACK 必达；④ 24h soak：heap 无泄漏（池水位恒定）、无 err_flags、OTA 升级后 mesh 自恢复；⑤ `mesh status` 长期计数健康。

---

## 12. 风险清单

| # | 风险 | 缓解 |
|---|---|---|
| R1 | **ed25519 verify 已知 bug 注释**（最高优先级正确性风险） | M1 RFC 8032 KAT + rweather 交叉验证；备胎 monocypher |
| R2 | LR2021 官方驱动成熟度（API 演进中） | pin commit vendor 不追新；M0 ping-pong 即第一道筛子；-706/standby/readData-stays-in-RX 怪癖已列处置（§6.2-4） |
| R3 | esp32s31 预览平台（手册 pre-release） | 时序参数 M0 实测不抄手册；冻结验证过的 IDF 版本进 vendor.lock.json/README；只用官方 API（RISC-V 双核，钉核/IRAM_ATTR 语义同 Xtensa） |
| R4 | C++→C 移植陷阱 | 包池手动释放无析构兜底（评审重点查释放路径）；path_len 编码与 TRACE 16 位 offset 防回绕照抄；millis 补码比较；calcRxDelay 的 powf() 链接 libm；RTCClock unique 语义保留 |
| R5 | 1 字节 path hash 碰撞 | 小规模（<20 节点）可接受；sendFlood 支持 hash_size 1-3，文档写明上限 |
| R6 | RAW_CUSTOM 不 flood 的协议限制 | `CONFIG_MESH_FLOOD_RAW_CUSTOM` patch，M4 前定案 |
| R7 | SPI 总线竞争（屏/codec 共总线） | radio_hal 内嵌总线 mutex；ISR 绝不碰 SPI |
| R8 | sub-GHz 合规（868 占空比/915 跳频） | airtime budget 兜底 + 区域化 Kconfig 默认值；商务侧确认目标市场频段/功率 |
| R9 | MeshCore 上游大版本演进导致蓝本漂移 | 蓝本 pin 在 v1.18.0（`D:\project\MeshCore` 当前 checkout），移植完成后不再跟踪上游，按需手动挑 patch |

---

## 13. 验证总策略

1. **host 单元测试**（core/ 零依赖模块）：包编解码往返、去重表、优先级队列、RFC 8032 KAT、AES 硬软互验——M1 一次性全绿。
2. **物理量测**（M0/M2）：GPIO 翻转 + 逻辑分析仪（airtime、IRQ 驱动、转发延迟、抖动）。
3. **行为验收**（M1-M4）：两板/三板实测；故障注入 = 拔天线、重启、注入重复包。
4. **端到端 soak**（M4）：24h 计数器审计（heap、池水位、dup、err_flags）+ OTA 回归。

---

## 14. 参考蓝本（只读）

| 用途 | 路径 |
|---|---|
| Dispatcher/pool/mesh C99 化直接蓝本 | `D:\project\MeshCore\src\{Dispatcher,Mesh,Packet,Identity,Utils}.*` |
| 包池/去重表 | `D:\project\MeshCore\src\helpers\{StaticPoolPacketManager.*,SimpleMeshTables.h}` |
| radio 接口语义 + LR2021 怪癖清单 | `D:\project\MeshCore\src\helpers\radiolib\{RadioLibWrappers.cpp,CustomLR2021.h,CustomLR2021Wrapper.h}` |
| ed25519 vendor 源 | `D:\project\MeshCore\lib\ed25519\` |
| 默认射频参数 | `D:\project\MeshCore\platformio.ini` |
| proj_s31 集成模式参照 | `proj_s31/main/common/CMakeLists.txt`、`proj_s31/thirdparty/xymodem/CMakeLists.txt`、`proj_s31/main/common/cmd_system/cmd_nvs.c` |
| 平台纪律继承 | `proj_s31/docs/mesh-4-msm4-rtcm-rtcm-lora-sx128x-esp32-sharded-seal.md`（D4 中断边界、R12 平台风险） |
