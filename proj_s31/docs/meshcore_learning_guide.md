# MeshCore 移植前学习计划（proj_s31）

> 版本：v1.0（2026-10-22）
> 目的：动手移植前系统学习 MeshCore 蓝本与 mesh 路由相关代码，学完后能独立完成 `meshcore_porting_plan.md` 的 M0–M4
> 蓝本：`D:\project\MeshCore\`（WSL: `/mnt/d/project/MeshCore/`），v1.18.0（tag `companion-v1.18.0`）
> 配套阅读：`docs/meshcore_porting_plan.md`（下称「移植计划」，引用其章节号）

## 学习方法（先读这条）

1. **主线贯穿法**：始终以「一个包从节点 A 到节点 B 的完整旅程」为主线——每个阶段学完都回到这条主线上补全一段。最终验收：能脱稿画出 direct 单播（含 ACK 返回）和 flood 广播两条完整时序图。
2. **先头文件后实现**：每个模块先读 `.h` 建立接口全景，再读 `.cpp` 验证实现。读实现时对照移植计划 §4 的模块表，标注每段代码落到哪个 `mc_*` 文件。
3. **带问题读**：每阶段开头的问题清单是导读，读完必须能回答。
4. **笔记产出**：每阶段一份笔记（时序图/状态机/表格，mermaid 或手绘拍照均可），存 `docs/notes/`。笔记是移植期的速查手册。
5. **遇到卡点直接问**：随时把具体文件:行号丢给 Claude 讨论，不要在一处卡超过半天。
6. **不需要跑起来**：本阶段纯阅读；官方固件与我们的 LR2021 硬件不兼容，跑真机不是本阶段目标（跑硬件从 M0 开始）。

## 阶段总览

| 阶段 | 主题 | 对应移植里程碑 | 预估 |
|---|---|---|---|
| S0 | mesh 网络与 LoRa 背景概念 | 全局 | 0.5~1 天 |
| S1 | 线格式与协议常量 | M1 | 0.5 天 |
| S2 | 基础设施：包池 / 去重表 / 工具 / 身份 | M1 | 1 天 |
| S3 | Dispatcher 事件循环 | M1~M2 | 1.5 天 |
| S4 | Mesh 路由逻辑（flood/direct/advert/path/ACK） | M2~M3 | 2 天 |
| S5 | Radio 抽象与 LR2021 怪癖 | M0 | 1.5 天 |
| S6 | 应用层如何组装核心（看我们要裁掉什么） | M4 | 1 天 |
| S7 | 回归移植计划 + 业务链路设计 | M4 | 0.5~1 天 |

合计约 **8~9 个工作日**（可按熟悉程度压缩 S0/S1/S6）。

---

## S0 — mesh 网络与 LoRa 背景概念（0.5~1 天）

### 目标
建立词汇表与直觉，避免读代码时被概念卡住。

### 读什么
- MeshCore 官方文档（`/mnt/d/project/MeshCore/docs/`，均有 `zh/` 中文版；文档对应 v1.12.0 略滞后于 v1.18.0 代码，**细节以代码为准**）：
  - **必读**：`packet_format.md`（152 行，线格式契约，S1 前置）、`payloads.md`（324 行，各包型 payload 结构，S4 对照表）、`radio_presets.md`（65 行，官方射频预设，S5 参照）
  - 选读：`faq.md`（854 行，routing/duty cycle 章节挑读）、`number_allocations.md`（25 行，M4 前扫一眼）
  - **跳过**：companion_protocol / kiss_modem / stats_binary_frames / terminal_chat_cli / cli_commands / nrf52_power_management / qr_codes（应用层或平台无关，我们不移植）
- `README.md`（仓库根，10 分钟）
- 概念清单（不懂的逐个查清楚）：
  - flood 路由 vs direct（source-routed）路由，各自适用场景
  - LoRa airtime 公式各项含义：SF / BW / CR / preamble / symbol time
  - duty cycle 法规（868 EU 0.1%/1%/10% 子带）与 LBT/CAD——**MeshCore 文档只有使用说明没有概念讲解**（`docs/cli_commands.md:560-593` 的 `set dutycycle` 命令 + af→占空比换算表；实现代码在 `Dispatcher.cpp:25-44,289-304`，S3 精读），概念出处：① ETSI EN 300 220-2 原文（ETSI 官网免费，子带划分与限值；注意 MeshCore 默认 869.618MHz 落在 10% 子带）② The Things Network Learn「Duty Cycle」篇（通俗入门；注意区分 ETSI 法规 vs LoRaWAN Fair Access Policy，我们是私有 mesh 只适用法规）③ CAD 原理见 Semtech SX1262/LR2021 数据手册 CAD 章节（S5 读）；LBT+AFA 是 EN 300 220 里占空比的替代合规路径，MeshCore 的 CAD 判忙本质上是半个 LBT
  - RSSI vs SNR，为什么 LoRa 解调能低于噪声地板
  - ed25519 签名 / X25519 ECDH 的用途区别（认证 vs 密钥协商）

### 自测问题
- 为什么 flood 网络必须有去重表？没有去重会发生什么？
- 2 KB/s 的 RTCM 流在 SF8/BW62.5 下大约占多少空口？（用 airtime 公式估算 255B 包）

### 产出
一页词汇表 + airtime 计算草稿。

---

## S1 — 线格式与协议常量（0.5 天）

### 目标
吃透空中字节的每一位。这是与 MeshCore 互通的契约，也是 `mc_packet.c/h` 的全部内容（移植计划 §4.1/§4.2）。

### 读什么（顺序）
1. `src/Packet.h`（114 行，全文）——header 位域、path_len 编码（高 2bit = hash 尺寸-1，低 6bit = 个数）、包型常量（19-32 行）
2. `src/Packet.cpp`（86 行，全文）——`writeTo/readFrom` 线格式、`calcHash`
3. `src/MeshCore.h`（128 行）——常量全集、`MainBoard` 接口（顺便看移植计划 §7 怎么替代它）

### 带问题读
- `path_len` 为什么用「高 2bit 编码 hash 尺寸」而不是固定 1 字节 hash？（提示：1B hash 碰撞 vs 包长开销的权衡，见移植计划 R5）
- 包哈希为什么是 SHA256(type‖payload) 截 8B 而不含 path？（提示：转发时 path 会变）
- transport_codes 那 4 字节是干什么的？（提示：多网络隔离，对应之前讨论的 TransportKeyStore）

### 动手
纸面编码练习：手算一个「flood ADVERT，path 含 2 个 1B hash，payload 30B」的完整线格式字节流，再手算解码。

### 产出
线格式速查表（贴进笔记首页，移植期高频翻阅）。

---

## S2 — 基础设施：包池 / 去重表 / 工具 / 身份（1 天）

### 目标
掌握三个支撑件，对应 `mc_pool/mc_tables/mc_crypto/mc_identity/mc_utils`（移植计划 §4.3~§4.6）。

### 读什么（顺序）
1. `src/helpers/StaticPoolPacketManager.h/.cpp`（~170 行，全文）——三队列 + 静态池 + (pri, scheduled_for) 延迟队列
2. `src/helpers/SimpleMeshTables.h`（76 行，全文）——160×8B 环形去重表
3. `src/Utils.h/.cpp`（338 行）——重点 `encryptThenMAC/MACThenDecrypt`（2B 截断 HMAC-SHA256）、`sha256`、hex 工具；AES 部分浏览即可（我们要换 esp_aes）
4. `src/Identity.h/.cpp`（256 行）——`LocalIdentity` 与 `Identity` 的派生关系；**特别读 34-37 行附近的注释**：orlp 纯 C 版 `ed25519_verify` 的 memory corruption bug 说明（移植计划 §8.1 的最高风险）
5. `lib/ed25519/` 目录浏览（10 分钟，不用读实现）——确认是纯 C、有哪些文件、license

### 带问题读
- 池的 `get()` 为什么取「已到期中 pri 最小」而不是严格 FIFO？（提示：direct 转发 vs flood 转发的优先级差）
- C++ 版用 new/delete，C 版静态数组 + 手动 free——找出蓝本里所有 alloc 点，逐个确认配对的释放路径（这是移植评审重点，提前建立敏感性）
- `getCurrentTimeUnique()` 为什么要保证单调不重复？（提示：advert 时间戳去重）

### 产出
- 包池状态转换图（unused → send_queue → 发射 → free / rx_queue → 处理 → free）
- 一页「C++ 对象生命周期 → C 手动管理」对照笔记

---

## S3 — Dispatcher 事件循环（1.5 天，核心中的核心）

### 目标
吃透事件循环骨架，对应 `mc_dispatcher`（移植计划 §4.7）。这是整个系统的心跳，移植保真度要求最高的文件。

### 读什么
`src/Dispatcher.h/.cpp`（595 行，**全文精读**），按功能块拆：

| 行区间（约） | 功能块 | 重点 |
|---|---|---|
| loop() 主体（:66 起） | 单次迭代顺序 | 为什么是「noise floor 采样 → TX 完成 → RX 队列 → checkRecv → checkSend」这个顺序 |
| :86-131 | TX 完成即取下一包 = burst | 与 IFS 的关系 |
| score/calcRxDelay（:55-57 附近） | RX 延迟入队 | snr 阈值表 + 碰撞罚的公式含义 |
| :289-304 | CAD 忙等待与 4s 强制发送 | 为什么忙 4s 还要发（死锁规避） |
| :327/:339 | TX 看门狗 1.5×airtime、radio 卡死 8s 报警 | 故障恢复设计 |
| duty cycle 部分 | tx_budget_ms 滑窗补充 | 868 法规的代码形态（移植计划 §6.3） |
| :381-384 | millis 回绕补码比较 | `(int32_t)(now - ts) > 0` 惯用法 |

### 带问题读
- Dispatcher 与 Radio 之间的状态机（IDLE/RX/TX_WAIT + INT_READY）有几个状态、什么事件触发迁移？（画状态机图）
- 中断里到底做了什么、没做什么？（对照移植计划 §9.4 的 DIO 纪律，理解为什么这么划）
- `is_receiving()` 判忙用哪几个信息源？各自漏检什么场景？
- 队列里「未到期」的包如何决定 task 睡多久？（对应我们 `mc_dispatcher_next_wakeup_ms()` 的设计）

### 产出
- Dispatcher 状态机图 + loop() 单次迭代时序图（本阶段最重要的两份笔记）
- 一页「移植陷阱清单」：回绕比较、看门狗、CAD 强制发送

---

## S4 — Mesh 路由逻辑（2 天，篇幅最大）

### 目标
吃透路由判定，对应 `mc_mesh`（移植计划 §4.8 + §5 裁剪清单）。

### 读什么
`src/Mesh.h/.cpp`（968 行，**全文精读**），按包型分块：

**第一天：接收路径**
1. `onRecvPacket()`（:41 起）总入口：route 类型分支（flood/direct/zero-hop）
2. :58 附近 direct 转发判定（「下一跳是我 && allowPacketForward && 未见过」）
3. :74-89 zero-hop 与「early ACK」
4. flood 分支各包型：ADVERT（:270-289 验签全过程）、REQ/RESPONSE（ECDH 数据面）、ACK（:117-126）、MULTIPART ACK（:300-322）、RAW_CUSTOM（:292-298，**注意 flood 不转发这个限制**，移植计划 §5.4）
5. `routeRecvPacket()`：去重 → 追加自身 hash → 按 hop 数定优先级转发

**第二天：发送路径与辅助**
6. `sendFlood/sendZeroHop/sendDirect` 系列 + `createDatagram/createAck`（:394/:560）
7. `removeSelfFromPath()` 与 direct 逐跳剥离
8. PATH/path return（寻路应答，`createPathReturn`）
9. TRACE 包型（注意 16 位 offset 防回绕——v1.11 修过的 bug，移植计划 R4）
10. `getRetransmitDelay/getDirectRetransmitDelay`（:17-22）：转发退避公式

### 带问题读
- flood 转发优先级为什么 = hop 数？（提示：离源越远的节点越晚发，减少碰撞）
- direct 包每一跳对 path 做什么操作？ACK 沿反向 path 返回时路径从哪来？
- advert 验签失败为什么直接丢而不进路由？伪造 advert 能造成什么攻击？
- ACK 为什么既能在 :80「搭车」在数据包里（early ACK），又能独立成包？
- 对照移植计划 §5.2 裁剪清单：每读到一个被裁功能（GRP_*/ANON_REQ/CONTROL/MULTIPART 通用分片），确认自己理解它原本解决什么问题、为什么我们不要

### 产出
- **direct 单播全程时序图**（A→R1→R2→B，含 ACK 原路返回）——主线任务的 direct 半幅
- **flood 广播全程时序图**（含三个中继的去重/转发/退避）——主线任务的 flood 半幅
- 裁剪确认清单（§5.2 每一项打勾 + 一句话理由）

---

## S5 — Radio 抽象与 LR2021 怪癖（1.5 天，M0 的直接准备）

### 目标
建立 radio 抽象层语义 + LR2021 移植需求清单，对应 `mesh_radio.h/radio_lr2021.c`（移植计划 §6）。

### 读什么
1. `src/helpers/radiolib/RadioLibWrappers.h/.cpp`（365 行，全文）——`mesh::Radio` 接口的实现语义：volatile 状态机、噪声地板 64 样本滑动平均、CAD、AGC 复位、`packet_score` 公式、`getEstAirtimeFor`
2. `src/helpers/radiolib/CustomLR2021.h`（130 行）+ `CustomLR2021Wrapper.h`（111 行，全文）——LR2021 特化：
   - preamble 规则（SF≤8 → 32 symbol，否则 16）
   - `calcMaxPacketMillis` 双超时
   - rx boost 档位
   - side detector（多 SF 并行监听，我们 P2 才做，理解概念即可）
3. `src/Dispatcher.cpp` 回头重读 radio 调用点（半小时）——这次从「ops 表每个函数何时被调」的视角看
4. Semtech LR2021 官方 C 驱动（Lora-net repo）——**确认 repo/commit，浏览 HAL 原语清单**（spi/reset/busy/irq），评估去阻塞化工作量（移植计划 §6.2）

### 带问题读
- LR2021 为什么 startReceive 前必须 standby？readData 后为什么不能再 startReceive（-706）？
- RadioLibWrapper 的中断模型：DIO 触发 → setFlag → loop 消费——与我们的「ISR 置标志 + notify task」模型逐项对应
- airtime 估算公式与 preamble 双超时的关系
- 噪声地板为什么用滑动平均而不是瞬时 RSSI？

### 产出
- `mesh_radio_ops_t` 每个函数的「蓝本语义一句话总结」表（移植时逐条实现）
- LR2021 怪癖清单（逐条标注处置方式，直接喂给 M0）

---

## S6 — 应用层组装（1 天，目的是理解我们裁掉什么）

### 目标
理解 MeshCore 应用层如何使用核心——不是为了移植它们，而是为了 ① 确认裁剪安全（移植计划 §5.2）② 学习 ACK/重发/寻路的应用层用法（我们的 dl_cmd/ul_stat 是同样位置的新代码）。

### 读什么
1. `src/helpers/BaseChatMesh.h/.cpp`（重点章节）：
   - :43 `sendAckTo` / :256,:306 收包回 ACK 的两个调用点
   - :372 `onAckRecv` / :385 `handleReturnPathRetry`
   - :451-490 `composeMsgPacket/sendMessage`：attempt 字段、expected_ack、超时设置
   - :985-992 loop 里的超时检测（**注意：固件不自动重发，重试靠手机 App**——见移植计划 §10.1）
2. `examples/simple_repeater/MyMesh.h`（浏览）——**最简应用层**，看 repeater 角色如何薄（几乎只有 allowPacketForward=true + ACL）
3. `examples/companion_radio/MyMesh.cpp`：:108-111 超时常量、:888-896 超时公式、:898 空的 `onSendTimeout()`（印证「重试在 App」）
4. `src/helpers/ClientACL.h`（浏览）——知道它管什么（权限/路径缓存/防重放），确认我们的固定 peer 表能替代（见之前讨论）

### 带问题读
- expected_ack = SHA256(明文‖发送方公钥) 截 4B——为什么接收方能独立算出来？
- attempt 字段为什么必须编进 payload？（提示：重发包不被去重表吞掉）
- repeater 和 companion 的分界线具体是哪几个函数？（`allowPacketForward` + advert 行为）

### 产出
- 「核心 vs 应用层」职责边界一页纸（移植计划 §10 的分工原则具象化）
- dl_cmd 设计checklist：attempt 字段、expected_ack、超时公式三个照抄点 + 自动重传队列这个增强点

---

## S7 — 回归移植计划 + 业务链路设计（0.5~1 天）

### 目标
学完验收 + 把知识落到我们的业务设计上。

### 做什么
1. **脱稿画两张图**：direct 单播全程（含 ACK 返回）、flood 广播全程——与 S4 笔记对照查漏
2. 重读 `meshcore_porting_plan.md` 全文，以新获得的理解评审它：每个 `mc_*` 模块是否知道怎么写？§10 的 dl_cmd/ul_stat 设计有没有要改的？
3. 列出「仍然模糊的点」清单——带着它进入 M0，或先找 Claude 逐条讨论清零
4. 检查 M0 前置条件：LR2021 驱动 repo 确认、两块板子 + 逻辑分析仪就绪、IDF 版本冻结决策

### 验收（全部能答 = 可以开工 M0）
- 一个 flood 包从产生到消亡的完整生命周期（含池、去重、转发、退避、看门狗）
- direct + ACK 往返的每个字节是谁在什么时刻放进包的
- Dispatcher loop 单次迭代的五件事及其顺序理由
- LR2021 的三个怪癖及处置
- 我们相对蓝本的全部改动点（RAW_CUSTOM flood patch、自动重传队列、NVS identity、esp_aes、C99 化手法）及各自理由

---

## 附录：代码体量速查（精读优先级排序）

| 优先级 | 文件 | 行数 | 读法 |
|---|---|---|---|
| ★★★ | `src/Dispatcher.h/.cpp` | 595 | 全文精读 |
| ★★★ | `src/Mesh.h/.cpp` | 968 | 全文精读 |
| ★★★ | `src/Packet.h/.cpp` | 200 | 全文精读 |
| ★★☆ | `src/helpers/StaticPoolPacketManager.*` | 170 | 全文 |
| ★★☆ | `src/helpers/SimpleMeshTables.h` | 76 | 全文 |
| ★★☆ | `src/helpers/radiolib/RadioLibWrappers.*` | 365 | 全文 |
| ★★☆ | `src/helpers/radiolib/CustomLR2021*.h` | 241 | 全文 |
| ★★☆ | `src/Identity.*` / `src/Utils.*` | 594 | 重点章节 |
| ★☆☆ | `src/helpers/BaseChatMesh.*` | ~1200 | 指定章节（S6） |
| ★☆☆ | `examples/simple_repeater`、`companion_radio` 部分 | — | 浏览 |
| ☆☆☆ | `lib/ed25519/` | ~4500 | 只浏览目录结构 |
