# SX128x 三跳 Mesh 协议规范 v0.2

> 状态：设计定稿，待实测校准（见 §14 待验证项）。
> 硬件：SX1280/81（2.4 GHz LoRa），最快档 SF5 / BW 1625 kHz / CR 4/5。
> 业务：网关下行 MSM4 四系统 RTCM 修正流（1~2 KB/s，全网共享广播，免重传）
> ＋ 少量控制指令（端到端确认）；叶子上行定位与状态（1 Hz）。

## 1. 角色与编址

| 角色 | node_id | 说明 |
|---|---|---|
| 网关 | 0x01 | hop 0，时间基准，分配 node_id，产生下行 |
| 中继 | 0x02–0x0F | hop 1 或 2，1.5 m 天线；hop 2 中继下只能挂叶子 |
| 叶子 | 0x10–0xFF | 0.3 m 天线，不转发；父节点 hop ≤ 2，自身 hop ≤ 3 |

0x00 无效，0xFF 在 `dst` 字段表示广播。`hw_id`（8 字节，芯片唯一 ID）
是入网和密钥派生的身份；网关维护 `hw_id → node_id` 映射（64 项，Flash，
掉线不删，重入复用原号）。

拓扑约束：同一碰撞域内每个父节点最多 1 个下一级中继（串行转发）；
确认分支互相听不见后可放宽到 8 个并行分支。每个父节点最多 8 个直接
子节点；网关固定预留 ≤3 个直挂叶子槽。

## 2. 物理层参数（全网常量）

```
调制: LoRa SF5 / BW 1625 kHz / CR 4/5 / 前导 12 / 显式头 / CRC-16
发射: +12.5 dBm；整包上限 255 B（含协议头与 MIC）
空中时间: 24 B ≈ 1.6 ms；130 B ≈ 5.6 ms；255 B ≈ 10.7 ms（待实测校准）
链路预算: 中继↔中继规划间距 250 m；叶子小区半径 50 m
信道: 51 个，中心 2400.8125 + k×1.625 MHz（k = 0..50）
```

全网同一时刻同一信道。只有网关开机扫描选信道（两轮 RSSI 峰值 + SF5 CAD，
避开 LoRa 占用，优选 WiFi 1/6/11 主瓣间缝隙），结果写 Flash，下次开机先
复查旧信道。中继/叶子按 0..50 逐信道听一个超帧找本网信标（最坏 ~51 s）。

## 3. 通用帧格式

整包 ≤ 255 B：

```
+--------+-----+-----+-----+------+-----+-----------+--------+
| net_id | t/f | src | dst | seq  | ttl | payload   | MIC    |
|   2    |  1  |  1  |  1  |  2   |  1  | 0..243 B  |  4     |
+--------+-----+-----+-----+------+-----+-----------+--------+
```

- `t/f`：高 4 bit 包类型，低 4 bit flags（bit0 = retry，bit1 = epoch 首块）。
- `seq`：每 `src` 单调递增 16 bit。
- `ttl`：下行初值 3，每转发减 1；上行包不使用（置 0）。
- `MIC`：见 §10 安全。

包类型：

| type | 名称 | 载荷 | 发射时机 |
|---|---|---|---|
| 0x1 | BEACON | ~18 B | 每个父节点 DL 窗口末尾 |
| 0x2 | DL_DATA | ≤243 B RTCM 流 | 网关 DL burst + 各级转发 |
| 0x3 | DL_CTRL | 2~16 B 控制 | DL 队列最前 |
| 0x4 | UL_STATUS | 16 B | 叶子的 6 ms slot |
| 0x5 | UL_BUNDLE | ≤243 B | 中继回程窗口 |
| 0x6 | JOIN_REQ | 12 B | 父节点 JOIN 时隙 |

## 4. 超帧与时序

超帧周期 1 s，以网关信标为逻辑 0 点。**调度只定义 TX 义务，节点默认
处于 RX**；义务完成立即回 RX，控制包插入和补发不需要重排时刻。

### 4.1 DL 窗口（背靠背 burst，无固定栅格）

- 窗口内只有一个发射者：网关或某级中继。包间隔 `IFS = 3 ms`
  （含对端 SPI 读取 + CMAC 校验时间，实测可调）。
- 窗口长度 = Σ(包空中时间 + IFS)，随流量自然伸缩；**窗口末尾的信标
  标记结束**，子节点据此对齐，无需预知包数。
- 中继转发窗口起点 = 收完父 DL burst（父信标）＋ 父 UL 窗口长度
  （父信标里 `n_leaf_slots` 可算）＋ GUARD。
- 容错：中继漏听父信标时，以 RX 空闲 >15 ms 判定 burst 结束，转发
  已收到的部分。

### 4.2 UL 时隙（固定栅格，多方发射）

- `TU = 6 ms`。父信标结束 = 本父节点 UL 窗口起点。
- 叶子 slot k 发射时刻 = `T_beacon_end + GUARD(1 ms) + k×6 ms`；
  JOIN 时隙紧跟最后一个叶子 slot。
- 网关 `n_leaf_slots = 3`，中继 `= 8`。

### 4.3 参考时间轴（2 KB/s，单链，3 直挂叶子）

```
0     122   146       268   322       444   498 516 534              1000
|------|-----|---------|-----|---------|-----|---|---|----------------|
RootDL  U0    R1        U1    R2        U2    B2  B1  命令补发 + 空闲
 ms
```

- 每跳 DL ≈ 9×(10.7+3) + 8 信标 ≈ 130 ms（9 个 243 B 块 ≈ 2.1 KB/s）
- B2/B1 回程窗口各 18 ms；业务部分 ~534 ms 结束，空闲 ~470 ms
  留给命令补发、未来 2 Hz epoch 或峰值 ~2.9 KB/s。

### 4.4 时钟同步

- 以父信标 RX-done 中断时刻校正本地超帧钟；±20 ppm 晶振 1 s 漂移
  ~20 µs，GUARD = 1~2 ms 足够。
- 连续丢信标则按漂移外推并逐步放宽接收窗；连续 3 次未校正判掉线（§9）。

## 5. 下行：RTCM 流（DL_DATA）

- 网关把 RTCM 字节流切成 ≤243 B 块，`seq` 单调递增；块边界**不对齐**
  RTCM 消息边界；每个 epoch 的第一块置 `flags.bit1`。
- **免重传**：任何一级丢了就丢了。叶子侧按序拼入环形缓冲直接喂
  RTCM 解析器——`0xD3` 前导 + 长度 + CRC 天然自同步，丢块只损失
  当前 epoch 的部分消息，解析器扫到下一个 `0xD3` 自动恢复。可选择
  跳过 `epoch 首块`标志缺失的不完整 epoch。
- 转发规则：中继在"下一跳是我且 ttl > 0 且 (src,seq,retry) 未见"时
  转发一次。已见表退化为 32 项滑动位图（单一源，无需哈希表）。
- 诊断：叶子在 UL_STATUS 里带 `dl_last_seq`，网关据此算每叶子的
  下行到达率，用于发现覆盖空洞，不触发补发。

## 6. 下行：控制指令（DL_CTRL，端到端确认）

### 6.1 滑动窗口命令确认

- 每条 DL_CMD 带 per-叶子 `cmd_seq`（8 bit，网关对每个叶子独立计数）。
- 叶子 UL_STATUS 带 `ack_base(1) + ack_map(1)`：bit i = 已收到
  `ack_base + i` 号命令，窗口覆盖最近 8 条。
- **去重与确认同构**：叶子执行命令前查 `ack_map`，bit 已是 1 则跳过
  执行、继续上报告 → at-least-once 传输 + 幂等执行 = 恰好一次。
- 网关看哪个 bit 是 0 就在下一超帧 DL 队首补发哪条（原 `cmd_seq`、
  `flags.retry=1`）；缺了 8 个序号未补上则放弃并上报业务层。
- `ack_base` 随最高已收序号滑动。

### 6.2 DL_CTRL 子类型

| 子类型 | 名称 | 载荷 |
|---|---|---|
| 0x01 | JOIN_ACK | hw_id(8) + assigned_id(1) + slot(1) + result(1) + parent_id(1) |
| 0x02 | CH_ANNOUNCE | new_channel(1) + switch_superframe(4)；切换前连续公告 8 超帧 |
| 0x03 | BUNDLE_ACK | relay_id(1) + bundle_seq(1) |
| 0x04 | DL_CMD | dst_leaf(1，0xFF=广播) + cmd_seq(1) + cmd_payload(n) |

全部插在 DL 队列最前，沿链正常转发（同 DL_DATA 的 ttl/已见表规则）。
广播命令使用独立广播序号空间，叶子侧对应 `bc_base/bc_map`（v1 可不做）。

## 7. 上行

### 7.1 UL_STATUS（叶子，16 B，1 Hz）

```
pos_seq(1) | lat(4, 度×1e7) | lon(4) | alt(2, m)
ack_base(1) | ack_map(1) | dl_last_seq(2) | batt(1)
```

无定位功能的叶子照常发送（承担命令确认与保活）。直挂网关的叶子在
U0 时隙发，网关直接收；中继下的叶子在父中继的 UL 窗口发。

### 7.2 UL_BUNDLE（中继回程）

```
bundle_seq(1) | new_parent(1, 0=不变) | n_joins(1) + join(12B × ≤2)
n_pos(1) + entry(17B × n)     entry = node_id(1) + UL_STATUS(16)
```

- **合并优先**：跳 1 中继把跳 2 上报告的条目与自己的合并为一包；
  溢出才拆第二个 fragment，同一 18 ms 窗口背靠背发（≤2 个）。
- 容量：单窗每跳 ~24 条；分支全开叶子数超容量时按新鲜度轮换上报，
  定位更新周期随之降为几秒一次（容量特性，非异常）。
- 确认：网关在下一超帧 DL 队首发 BUNDLE_ACK；中继未见确认则下轮
  原样重发一次（`bundle_seq` 不变，网关按 seq 去重）。
- 叶子不等逐条 ACK，下一轮用新 `pos_seq` 覆盖旧值。

## 8. 入网

### 8.1 选父判据（打分制）

```
收集: 听 ≥2 个超帧，收 MAC 验证通过的信标 {node_id, hop, free_slots, RSSI}
门槛: hop ≤ 2 且 free_slots > 0 且 信标接收率 ≥ 60%（近 5 超帧 ≥3 次）
打分: score = RSSI均值 − HOP_PENALTY(10 dB) × hop
选择: score 最大者；候选列表保留为故障转移清单
```

### 8.2 叶子入网（SCAN → JOIN → RUN，失败 RETRY 退避 0~3 超帧）

1. 在选中父节点的 JOIN 时隙发 `JOIN_REQ`：hw_id(8) + role(1) +
   nonce(2) + parent_rssi(1)，帧头 dst = 父节点。
2. 父是中继：不写子表，请求入 UL_BUNDLE 上报告网关（每 bundle ≤2 个）；
   父是网关：直接收下，占用直挂叶子槽，满则拒绝。
3. 只有网关裁决：按 hw_id 复用旧 node_id 或分配新号 + slot，
   JOIN_ACK 沿 DL 链下发，目标父节点在其 DL 窗口再发一次。
4. 叶子在父 DL 窗口按 hw_id 匹配 JOIN_ACK；统一 3 超帧超时。
5. 超时：从候选列表删该父节点本轮资格，退避后按新 score 重选
   （撞车则重选结果往往仍是它；链路不对称则第二候选顶上）。
6. JOIN 时隙撞车不可检测，双方超时退避；退避期间继续听信标维持同步。

### 8.3 中继入网

同一 JOIN_REQ（role=RELAY），扫 1.5 m 回程。网关拒绝会让子树超 3 跳
或父节点无空闲槽的申请。**入网成功前不发自己的信标、不接收叶子。**

## 9. 掉线、换父与漫游

判定：连续 3 超帧未收到父信标（或信标接收率 < 60% 持续 5 超帧）。

- **叶子**：SCAN 重选（§8.1），原 hw_id 入网；网关保留 node_id，
  只改 parent_id 和 slot。扫不到任何信标则按超帧间歇侦听。
- **中继**：重扫回程，先入网到新父（保留子树，叶子不集体重入），
  在 UL_BUNDLE 的 `new_parent` 上报告，网关只更新路由；超时找不到
  新父则停发信标，其叶子按上一条散开重入。
- **网关侧**：5 超帧未见的子节点释放槽位；hw_id 映射保留。
- **漫游**（旧父仍可用时的提前切换）：复用打分制——候选
  `score − 当前父 score ≥ 6 dB` 且连续 2 超帧，向新父发 JOIN；
  切换期间定位可停一轮。

## 10. 安全（NetKey + AES-CMAC）

目标：防伪造（含重放），不加密。模型参照 LoRaWAN 安全层。

### 10.1 密钥派生（根密钥 NetKey 128 bit，烧录预置，Flash 读保护）

```
K_dl     = CMAC(NetKey, "dl")             // DL_DATA / DL_CTRL / BEACON
K_join   = CMAC(NetKey, "join")           // JOIN_REQ / JOIN_ACK
K_node_i = CMAC(NetKey, "ul" ‖ hw_id_i)   // 叶子/中继上行；网关按 hw_id 现算
```

### 10.2 每包处理

```
MIC = CMAC(K_按包型, superframe(4) ‖ 帧头(8) ‖ payload) 取低 4 字节
```

- 接收方用当前超帧号重算比对；边界到达的包用 SF 与 SF−1 各试一次。
- 校验失败：丢包 + 计数（诊断）。
- 抗重放：superframe+seq 在 MAC 输入内，跨超帧重放必错；同超帧
  重复由已见表/序号窗口挡掉。无需任何 nonce 管理。
- 信标带 MIC：扫描中的节点即可验证，防假冒父节点劫持子树。
- JOIN 用 K_join：无密钥设备无法入网。

### 10.3 开销与算力

空口 ≈ +3%（MIC 1.6% + IFS 2→3 ms ≈ 1%）；UL 侧被 6 ms 时隙吸收。
255 B CMAC = 16 个 AES 分组，硬件 AES 微秒级，软件 ~0.5 ms < IFS。

### 10.4 边界与升级路径

- 不防物理拆解取 NetKey → v2 可考虑每节点独立密钥。
- 需要加密时在现有框架上加 AES-CTR（计数器 = superframe‖seq），
  零额外字节。
- "口令入网"（类 WPA3/SAE）：v2 在 JOIN 流程嵌 SPAKE2+，两个
  椭圆曲线点 ≈64 B，两次 JOIN 往返放得下；帧格式不变。

## 11. 信道迁移

1. 网关运行中检测到下行到达率持续过低 → 重扫 51 信道。
2. 决定迁移：DL_CTRL CH_ANNOUNCE 连续公告 8 超帧（含新信道号与
   switch_superframe）。
3. 所有节点在指定超帧边界切到公告信道；不做全扫描。
4. 错过全部公告的节点：丢信标 3 次后按 §2 逐信道重扫。

## 12. 节点侧状态表

**网关**：子表 8 项 {node_id, hw_id, role, slot, last_seen_sf}；
hw_id→node_id 映射 64 项（Flash）；每子中继 last bundle_seq；
DL 窗口基 seq；每叶子 {next_cmd_seq, 未确认窗口 8 槽}。

**中继**：父 {parent_id, parent_hop, 同步状态}；子表 8 项（叶子 +
下级中继混合）；转发滑动位图 32 项；本帧 UL 缓存（8×17 B）；
待转发 JOIN_REQ ≤2；自己的 bundle_seq。

**叶子**：hw_id, node_id, parent_id, slot；同步状态；候选信标表
4 项 {node_id, hop, RSSI均值, 接收计数}；cmd ack 窗口
{ack_base, ack_map}；last dl_seq。

## 13. 常量表（宏，实测后校准）

```
// 物理层
LORA_SF=5  LORA_BW=1625k  LORA_CR=4/5  PREAMBLE=12  TX_DBM=12.5
PKT_MAX=255  HDR_LEN=8  MIC_LEN=4  APP_MAX=243
CHANNELS=51  CH0_HZ=2400812500  CH_STEP_HZ=1625000

// 时序
SF_MS=1000  IFS_MS=3  TU_MS=6  GUARD_MS=1  BEACON_AIR_MS=8
BUNDLE_WIN_MS=18  DL_IDLE_END_MS=15

// 下行
DL_PKT_MAX=12  TTL_INIT=3  SEEN_WIN=32

// 命令
CMD_WIN=8  CMD_GIVEUP=8

// 入网/保活
MAX_RELAY_HOP=2  JOIN_TIMEOUT_SF=3  JOIN_BACKOFF_SF_MAX=3
BEACON_MISS_MAX=3  BEACON_PRR_MIN=60%  BEACON_PRR_WIN=5
SLOT_TIMEOUT_SF=5  HOP_PENALTY_DB=10  ROAM_HYST_DB=6  ROAM_HOLD_SF=2

// 合并包
BUNDLE_FRAG_MAX=2  JOIN_PER_BUNDLE=2

// 安全
NET_KEY=<烧录>  MIC_TRY_SF={0,-1}

// 信道迁移
CH_ANNOUNCE_SF=8
```

## 14. 待实测验证项（对应实现 todo 第一项）

1. 两块板实测 24 / 128 / 255 B 三档空中时间，校准 §2 表。
2. IFS=3 ms 下接收端能否完成 SPI 读取 + CMAC 校验（不够则 +1 ms）。
3. 255 B 包在 250 m 规划链路上的实际 PER，验证"大块不亏"推算。
4. 180→255 B 调整后 DL burst 末尾信标的对齐精度（GUARD 是否够）。
5. 信标接收率门槛 60% 与 HOP_PENALTY 10 dB 在真实重叠区的表现。

## 15. 与 v0.1（原计划）的差异备忘

- 包长 180 B → 255 B（含 4 B MIC），应用块 172→243 B。
- DL 窗口固定 16 ms 栅格 → 背靠背 burst + IFS 3 ms。
- 删：中继 16 bit 缺包位图、重传 dst 寻址（RTCM 免重传后失效）。
- 新增：DL_CMD 滑动窗口确认（ack_base/ack_map）；epoch 首块标志。
- 新增：全网 CMAC 认证（§10），空口 +3%。
- 选父：纯 RSSI → 打分制 + 接收率门槛（§8.1）。
- BUNDLE_ACK 从"下行头捎带"改为独立 DL_CTRL。
- UL_POS → UL_STATUS（+ack 窗口 +dl_last_seq，10→16 B）。
