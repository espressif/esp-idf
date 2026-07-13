BLE Log 统一 Buffer 池设计

已确定约束





每个 transport buffer 仍保持 512 字节。



将 QUEUED 和 SENDING 合并为一个 SENDING 状态。SENDING 覆盖 runtime queue 等待、SPI driver queue 等待、DMA 传输以及 tx-done callback 等待。



采用方案 B：通过 open_bitmap 扫描多个 OPEN buffer；不要求保证发送顺序。



只区分 task 和 ISR；LL 日志不再单独分配池，而是根据调用上下文/source path 归入 task 或 ISR。



task 路径在没有可用 buffer 时通过阻塞/背压尽量避免丢数据。



ISR 路径绝不能阻塞。ISR 优先使用 ISR reserve buffer，再尝试 shared buffer；两者都没有时才丢日志。

核心状态模型

在 [components/bt/common/ble_log/src/ble_log_lbm.c](components/bt/common/ble_log/src/ble_log_lbm.c) 中维护一个全局 pool，继续使用 [components/bt/common/ble_log/src/internal_include/ble_log_prph.h](components/bt/common/ble_log/src/internal_include/ble_log_prph.h) 里的 ble_log_prph_trans_t 作为 DMA transport 对象。

FREE
  buffer 为空，并且在 free_bitmap 中可见

WRITING
  某个 writer 独占该 buffer；不在 free_bitmap/open_bitmap 中可见

OPEN
  buffer 中已有一条或多条 frame，后续仍可能追加；在 open_bitmap 中可见

SENDING
  buffer 已 seal/提交；prph_owned=true；直到 tx_done 前都不可复用

SENDING 有意把软件 queue 和外设传输阶段合并在一起。现有 prph_owned 可以继续作为“该 buffer 已交给外设链路、不可写”的标志。

Buffer 分区

统一 pool 内部通过 domain mask 做逻辑分区，不再创建多个独立 LBM 对象：

total buffers: N，每个 512 B
isr_reserve_mask       ISR 优先分配的 reserve buffer
shared_mask            task 和 ISR 都可使用
task_blocking_mask     最后 1 或 2 个专门用于 task 背压的 buffer

推荐第一版配置：

N = 16
ISR reserve = 2 个 buffer
Task blocking reserve = 2 个 buffer
Shared = 12 个 buffer

访问策略：

Task:
  扫描 shared | task_blocking 中的 OPEN buffer
  优先从 shared 分配 FREE buffer
  shared 为空时再从 task_blocking 分配 FREE buffer
  仍没有可用 buffer 时，阻塞并重试
  默认不消耗 ISR reserve

ISR:
  扫描 isr_reserve | shared 中的 OPEN buffer
  优先从 isr_reserve 分配 FREE buffer
  isr_reserve 为空时再从 shared 分配 FREE buffer
  仍没有可用 buffer 时，丢日志并更新 lost 统计
  不消耗 task_blocking reserve

这样 task 在普通压力下可以通过背压避免丢数据，同时 ISR 仍有最低保障的快速路径。

如何查找 OPEN Buffer

OPEN 查找基于 bitmap。除了单个 buffer 的 CAS(state, OPEN, WRITING) 外，不需要全局锁：

allowed_open = open_bitmap & allowed_domain_mask
start = per-context round-robin cursor
从 cursor 开始遍历 allowed_open 中的 set bit:
    trans = pool[id]
    if CAS(trans.state, OPEN, WRITING) fails:
        continue

    atomic_clear(open_bitmap, id)

    if trans.free_space >= frame_len:
        return trans

    if trans.pos > 0:
        seal_and_send(trans)       // state 变为 SENDING
        continue

    trans.state = FREE             // 空 buffer 的防御性回退
    atomic_set(free_bitmap, id)

关键细节：





open_bitmap 里的 bit 只是 hint，真正的独占由 CAS(OPEN -> WRITING) 决定。



writer 成功占有 buffer 后立即清掉 open bit，避免其他 writer 继续尝试同一个 buffer。



如果占有后发现剩余空间不足，writer 负责把该 buffer seal 并提交发送，然后继续扫描其他 OPEN buffer。



round-robin cursor 用来避免所有 writer 都从低编号 buffer 开始竞争。



当 N=16 时 ISR 可以扫描所有允许的 bit；如果后续需要更严格限制 ISR 耗时，可以增加 CONFIG_BLE_LOG_ISR_OPEN_SCAN_MAX。

如何分配 FREE Buffer

FREE 分配是在 domain mask 过滤后的 free_bitmap 上做原子 test-and-clear：

candidate = free_bitmap & allocation_mask
while candidate != 0:
    id = next_set_bit(candidate, cursor)
    bit = BIT(id)
    old = atomic_load(free_bitmap)
    if (old & bit) == 0:
        candidate &= ~bit
        continue
    if CAS(free_bitmap, old, old & ~bit) succeeds:
        trans = pool[id]
        trans.state = WRITING
        trans.pos = 0
        return trans

分配顺序：

Task:
  尝试 shared_mask
  尝试 task_blocking_mask
  然后阻塞/重试

ISR:
  尝试 isr_reserve_mask
  尝试 shared_mask
  然后丢日志

如果 frame_len > 512，单个 transport 无法容纳该 frame。第一版建议保持当前语义，直接按 lost 统计；除非后续决定增加分片机制。

Task 阻塞策略

当没有 task 可用的 OPEN 或 FREE buffer 时，task writer 进入背压等待，而不是直接丢日志：

while pool enabled:
    trans = try_find_open(task_mask)
    if trans: return trans

    trans = try_alloc_free(shared_mask)
    if trans: return trans

    trans = try_alloc_free(task_blocking_mask)
    if trans: return trans

    if cannot_block_now:
        count lost and return false

    wait on task_buffer_available semaphore

以下情况 cannot_block_now 应为 true，不能阻塞：





ISR 上下文。



scheduler suspended。



BLE Log deinit/disabled 路径。



runtime task 内部维护日志，如果阻塞会影响 runtime queue drain。



panic/critical 路径，如果后续有这类日志接入 BLE Log。

唤醒规则：

tx_done:
  state = FREE
  pos = 0
  prph_owned = false
  atomic_set(free_bitmap, id)
  if id is task-usable:
      give task_buffer_available semaphore

这个机制通过对 producer 施加背压，让 task 路径尽量不丢数据。如果 peripheral/DMA 路径永久卡死，它无法保证系统继续前进，所以可以保留 debug timeout；但如果目标明确是“宁愿阻塞也不丢”，生产默认可以用 portMAX_DELAY。

写入完成规则

writer 追加一帧后：

if free_space <= BLE_LOG_FRAME_OVERHEAD:
    seal_and_send(trans)
else:
    trans.state = OPEN
    atomic_set(open_bitmap, id)

seal_and_send() 负责：

trans.prph_owned = true
trans.state = SENDING
从 free/open bitmap 中清除
更新 inflight 计数
将 trans 指针入 runtime queue 或 direct SPI path

现有 [components/bt/common/ble_log/src/prph/ble_log_prph_spi_master_dma.c](components/bt/common/ble_log/src/prph/ble_log_prph_spi_master_dma.c) 中的 SPI callback 仍作为回收点。它负责 reset pos、清理 timing 字段、更新 latency 统计，并把对应 bit 放回 free_bitmap。

顺序与统计

由于不要求保证发送顺序：





多个 OPEN buffer 可以并发填充。



buffer 可以按“先满/先 flush”的顺序提交。



跨 buffer 的 frame 顺序不保证。



per-source frame_sn 应继续保留，便于接收端/parser 检测 gap，必要时也能恢复近似顺序。

统计建议从 LBM-local 计数迁移到 pool/domain 计数：

pool_inflight
pool_inflight_peak
isr_reserve_peak
task_blocking_wait_count
task_blocking_wait_max_us
task_blocking_wait_total_us
lost_by_no_buffer_isr
lost_by_no_buffer_task_no_block

实现改动点





在 [components/bt/common/ble_log/src/internal_include/ble_log_lbm.h](components/bt/common/ble_log/src/internal_include/ble_log_lbm.h) 中将 LBM pool 结构替换为全局 pool context、bitmaps、domain masks、semaphore 和 per-buffer state metadata。



在 [components/bt/common/ble_log/src/ble_log_lbm.c](components/bt/common/ble_log/src/ble_log_lbm.c) 中围绕 try_find_open()、try_alloc_free() 和 task blocking wait/retry 重写 acquire/write/flush 路径。



保留 [components/bt/common/ble_log/src/ble_log_rt.c](components/bt/common/ble_log/src/ble_log_rt.c) 中 runtime pointer queue 的行为，但 inflight accounting 改为基于 pool，而不是基于 LBM。



保留 [components/bt/common/ble_log/src/prph/ble_log_prph_spi_master_dma.c](components/bt/common/ble_log/src/prph/ble_log_prph_spi_master_dma.c) 中 SPI DMA 发送/回收行为，只需要把 recycle callback 改为回收到全局 pool，并唤醒 task waiter。



在 [components/bt/common/ble_log/Kconfig.in](components/bt/common/ble_log/Kconfig.in) 中增加 total buffer count、ISR reserve count、task blocking reserve count，以及可选 task wait timeout/debug stats 配置。

数据流

flowchart TD
    writeCall["ble_log_write_hex / ble_log_write_hex_ll"] --> contextCheck["判断 task 或 ISR"]
    contextCheck --> openScan["扫描允许的 OPEN bitmap"]
    openScan -->|"找到"| writeFrame["CAS OPEN 到 WRITING 并追加 frame"]
    openScan -->|"未找到"| freeAlloc["从允许的 FREE bitmap 原子分配"]
    freeAlloc -->|"找到"| writeFrame
    freeAlloc -->|"task 为空"| taskWait["Task 等待 buffer_available"]
    freeAlloc -->|"ISR 为空"| dropISR["丢日志并更新 lost 统计"]
    taskWait --> openScan
    writeFrame --> spaceCheck["检查剩余空间"]
    spaceCheck -->|"足够"| markOpen["状态 OPEN，设置 open_bitmap"]
    spaceCheck -->|"已满"| sendBuffer["状态 SENDING，入队 trans 指针"]
    sendBuffer --> rtTask["Runtime / direct SPI"]
    rtTask --> spiDma["SPI DMA"]
    spiDma --> txDone["tx_done 回收"]
    txDone --> freeBit["状态 FREE，设置 free_bitmap，唤醒 task"]

推荐第一版默认值





CONFIG_BLE_LOG_POOL_TRANS_CNT = 16



CONFIG_BLE_LOG_POOL_ISR_RESERVE_CNT = 2



CONFIG_BLE_LOG_POOL_TASK_BLOCKING_RESERVE_CNT = 2



CONFIG_BLE_LOG_POOL_TRANS_SIZE = 512



Task wait 默认：为了 no-loss 测试使用 portMAX_DELAY，同时保留 debug 选项打印 wait max/avg。



ISR 行为：不阻塞、不走 spin fallback；ISR reserve 和 shared 都耗尽时直接丢日志。

