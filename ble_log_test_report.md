# BLE Log 流量与 Buffer 延迟测试记录

## 1. 测试目标

本轮测试主要关注 BLE 协议栈在典型场景下产生的 BLE log 流量，以及 BLE log buffer 从提交到 SPI DMA 到回收之间的延迟。

主要问题包括：

- 典型 BLE 场景下，协议栈平均会产生多少 log 流量。
- 瞬时或峰值 log 压力是否可能超过当前 SPI 输出能力。
- Host 与 Controller/LL 层分别贡献多少 log 流量。
- BLE log buffer 从不可写入到可再次写入的等待时间是多少。
- SPI DMA latency 中，真正 SPI 传输时间和 SPI driver queue 等待时间各占多少。

暂不考虑 ESP BLE Mesh Profile。后续如需覆盖 Mesh，可单独补充 Mesh provisioning、proxy、relay、large composition data、high traffic model publish 等场景。

## 2. 测试矩阵

当前计划关注以下组合：

| 维度 | 组合 |
| --- | --- |
| Controller | ESP IP / Ceva IP / 自研 Controller |
| SoC | C3 / S3 / C6 |
| Host | NimBLE / Bluedroid |
| 场景 | 2M PHY Throughput / 连接建立 / 连接断开 |
| 输出方式 | BLE Log over SPI DMA |

当前已有数据主要来自：

- 自研 Controller，C6。
- NimBLE Host。
- Bluedroid Host。
- 2M PHY throughput。
- client 侧连接建立与断开窗口。
- SPI DMA buffer latency 拆分测试。

其他 Controller 组合待补充。C3、S3 使用别人的 controller 时，需要打开 controller 的 flash-only 配置，具体打开方式需要向 Jianhua 或 Haipeng 确认。

## 3. 测试工程

### 3.1 NimBLE Host

使用工程：

```text
examples/bluetooth/nimble/throughput_app/gatt/blecent_throughput
examples/bluetooth/nimble/throughput_app/gatt/bleprph_throughput
```

说明：

- `blecent_throughput` 是 Central / GATT Client。
- `bleprph_throughput` 是 Peripheral / GATT Server。
- 两个工程配合运行，通过 CLI 发起 read / write / notify throughput 测试。

关键配置：

```text
Example Configuration -> Enable Extended Adv

Component config -> Bluetooth -> NimBLE Options -> BLE 5.x Features
    Enable BLE 5 feature
    Enable 2M Phy
    Enable extended advertising
```

等价关键配置：

```text
CONFIG_EXAMPLE_EXTENDED_ADV=y
CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT=y
CONFIG_BT_NIMBLE_LL_CFG_FEAT_LE_2M_PHY=y
CONFIG_BT_NIMBLE_EXT_ADV=y
```

测试命令示例：

```text
throughput read 60 1
throughput write 60 1
throughput notify 60 1
```

其中最后一个参数为 PHY：

```text
0 = 1M
1 = 2M
2 = Coded S2
3 = Coded S8
```

### 3.2 Bluedroid Host

使用工程：

```text
examples/bluetooth/bluedroid/ble_50/ble50_throughput/throughput_client
examples/bluetooth/bluedroid/ble_50/ble50_throughput/throughput_server
```

当前主要使用 write 模式：

```text
client -> server
```

server 侧打印 `GATTC write Bit rate`，表示 GATT Client 写入到 GATT Server 的应用层吞吐。

### 3.3 连接建立与连接断开

使用工程：

```text
examples/bluetooth/bluedroid/ble/gatt_client
examples/bluetooth/bluedroid/ble/gatt_server
```

当前只统计 client 侧：

- 连接建立窗口：`esp_ble_gattc_enh_open()` 前 reset，到 `ESP_GATTC_OPEN_EVT` 成功后 dump。
- 连接断开窗口：`esp_ble_gattc_close()` 前 reset，到 `ESP_GATTC_DISCONNECT_EVT` 后 dump。

## 4. BLE Log 统计口径

### 4.1 Log 流量统计

通过 BLE log 写入接口统计：

```text
ble_log_write_hex()
ble_log_write_hex_ll()
```

输出示例：

```text
BLE log 60s write attempt bytes: host=..., ll=..., total=...
```

含义：

- `host`：Host/HCI 侧尝试进入 BLE log 的 payload 字节数。
- `ll`：Controller/LL 侧尝试进入 BLE log 的 payload 字节数。
- `total`：两者合计。

注意：该值是 BLE log 记录量，不是 BLE 应用数据量。

### 4.2 Buffer latency 统计

当前统计 BLE log buffer 生命周期：

```text
ble_log_rt_queue_trans()
  -> prph_owned = true
  -> 记录 prph_owned_start_us
  -> xQueueSend 或 direct send
  -> ble_log_prph_send_trans()
  -> spi_device_queue_trans()
  -> SPI DMA 发送
  -> tx_done_cb()
  -> trans->pos = 0
  -> inflight--
  -> prph_owned = false
```

输出：

```text
BLE log buffer owned latency
```

含义：对 LBM 来说，一个 buffer 从不可写入到可以重新写入的总等待时间。

它包含：

- runtime queue 等待。
- `ble_log` task 调度。
- SPI driver queue 等待。
- SPI DMA 发送。
- tx_done callback 延迟。

### 4.3 SPI DMA latency 拆分

进一步拆分：

```text
SPI DMA latency = queue_wait + dma_time
```

其中：

```text
queue_wait = pre_cb_us - spi_queue_start_us
dma_time   = done_us - pre_cb_us
```

实现位置：

- `spi_device_queue_trans()` 前记录 `spi_queue_start_us`。
- `spi_master_dma_pre_tx_cb()` 中记录 `spi_pre_cb_us`。
- `spi_master_dma_tx_done_cb()` 中计算并统计。

输出：

```text
BLE log SPI queue wait
BLE log SPI DMA time
```

`queue_wait` 用于判断 SPI driver queue 是否积压。  
`dma_time` 更接近真实 SPI 传输加 tx_done 回调路径耗时。

## 5. 已有测试结果

### 5.1 NimBLE Host，屏蔽箱内，2M PHY

#### Read 60s

```text
Application Read throughput = 272000 bps
BLE log attempt bytes: host=15600, ll=13937696, total=13953296
```

计算：

```text
平均 log 流量 = 13,953,296 / 60 = 232,555 B/s
平均 log bit rate = 232,555 * 8 = 1.86 Mbps
```

#### Write 60s

```text
Application Write throughput = 815621 bps
BLE log attempt bytes: host=15600, ll=27220599, total=27236199
```

计算：

```text
平均 log 流量 = 27,236,199 / 60 = 453,937 B/s
平均 log bit rate = 453,937 * 8 = 3.63 Mbps
```

#### Notify 60s

```text
BLE log attempt bytes: host=15600, ll=29023259, total=29038859
```

计算：

```text
平均 log 流量 = 29,038,859 / 60 = 483,981 B/s
平均 log bit rate = 483,981 * 8 = 3.87 Mbps
```

#### 观察

NimBLE Host 下，2M PHY read 的应用吞吐较低，约 `272 kbps`。write 和 notify 的 BLE log 流量明显更高，主要来自 LL 层。

### 5.2 Bluedroid Host，屏蔽箱内，2M PHY write

#### Client 侧，发送侧

```text
BLE log 60s write attempt bytes: host=3247263, ll=31909107, total=35156370
```

计算：

```text
平均 log 流量 = 35,156,370 / 60 = 585,940 B/s
平均 log bit rate = 585,940 * 8 = 4.69 Mbps
```

#### Server 侧，接收侧

```text
GATTC write Bit rate ≈ 173,260 B/s = 1.386 Mbps
BLE log 60s write attempt bytes: host=4171634, ll=25541394, total=29713028
```

计算：

```text
平均 log 流量 = 29,713,028 / 60 = 495,217 B/s
平均 log bit rate = 495,217 * 8 = 3.96 Mbps
```

#### 观察

- Bluedroid 2M write 的应用吞吐约 `1.386 Mbps`。
- BLE log 量明显大于应用数据量。
- client 侧和 server 侧统计的是不同设备本地的 log 输出，因此数值不同是正常的。
- client 侧是发送 write 的一侧。
- server 侧是接收 write 的一侧。
- 两侧 LL log 都是主要流量来源。

## 6. 连接建立与连接断开

### 6.1 连接建立

统计窗口：

```text
client: esp_ble_gattc_enh_open() 前 reset
到
client: ESP_GATTC_OPEN_EVT success 后 dump
```

测试结果：

```text
BLE log connect attempt bytes: host=1222, ll=9244, total=10466, elapsed=117127 us (117 ms)
BLE log connect attempt bytes: host=1222, ll=9244, total=10466, elapsed=118448 us (118 ms)
```

结论：

- 连接建立窗口 log 总量约 `10.5 KB`。
- 连接建立耗时约 `117-118 ms`。
- LL 层 log 占主要部分。
- 当前窗口不包含后续 MTU exchange、service discovery、descriptor write、notification 等 GATT 业务流程。

### 6.2 连接断开

统计窗口：

```text
client: esp_ble_gattc_close() 前 reset
到
client: ESP_GATTC_DISCONNECT_EVT 后 dump
```

测试结果：

```text
BLE log disconnect attempt bytes: host=405, ll=290, total=695, elapsed=9959 us (9 ms)
```

结论：

- 连接断开窗口 log 总量约 `695 B`。
- 断开耗时约 `9.959 ms`。
- 断开过程 log 量远小于连接建立过程。

## 7. SPI DMA Buffer 延迟测试

### 7.1 背景

一个 log transport buffer 通常为 512 bytes：

```text
CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE / BLE_LOG_TRANS_BUF_CNT
CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE / BLE_LOG_TRANS_BUF_CNT
```

若单个 buffer 为 512 bytes，SPI 频率为 20 MHz，则理论传输时间为：

```text
512 * 8 / 20 MHz = 204.8 us
```

因此单笔真实 SPI 传输时间理论上应在约 `200 us` 量级。

### 7.2 未拆分 queue/dma 前的现象

典型数据：

```text
server:
BLE log buffer owned latency: count=102384, avg=268 us, min=208 us, max=1885 us
BLE log SPI DMA latency: count=102384, avg=258 us, min=199 us, max=699 us

client:
BLE log buffer owned latency: count=69694, avg=219 us, min=133 us, max=627 us
BLE log SPI DMA latency: count=69694, avg=208 us, min=124 us, max=615 us
```

观察：

- `buffer owned latency` 和 `SPI DMA latency` 平均值很接近。
- 差值通常只有约 `10 us`。
- 说明 runtime queue/task 调度不是主要耗时来源。
- 主要耗时发生在 SPI driver queue + DMA + post_cb 这段路径。

### 7.3 Direct-send 测试

测试目的：绕过 `ble_log_rt_task()`，在 task 上下文中直接调用 SPI DMA 后端。

测试路径：

```text
ble_log_rt_queue_trans()
  -> 非 ISR 时直接 ble_log_prph_send_trans(*trans)
```

保留逻辑：

- ISR 仍走原 `xQueueSendFromISR()`。
- scheduler suspended 仍走原 non-blocking queue。
- 可选地打开 `CONFIG_BLE_LOG_DROP_ISR_LOG_TEST`，在写 buffer 前直接丢弃 ISR log。

典型结果：

```text
server:
BLE log buffer owned latency: count=100116, avg=274 us, min=201 us, max=1863 us
BLE log SPI DMA latency: count=100116, avg=273 us, min=199 us, max=1862 us

client:
BLE log buffer owned latency: count=68447, avg=210 us, min=125 us, max=914 us
BLE log SPI DMA latency: count=68447, avg=208 us, min=124 us, max=913 us
```

结论：

- direct-send 后，`owned latency` 与 `SPI DMA latency` 基本重合。
- 说明 runtime queue/task 调度开销被消除后，主要耗时仍在 SPI driver/DMA 路径。
- server 侧 max 仍可达到约 `1.8 ms`，说明峰值并非 runtime task 调度导致。

### 7.4 拆分 SPI queue wait 与 DMA time

新增统计：

```text
queue_wait = pre_cb_us - spi_queue_start_us
dma_time   = done_us - pre_cb_us
```

典型结果：

```text
server:
BLE log buffer owned latency: count=101098, avg=276 us, min=202 us, max=1922 us, last=458 us
BLE log SPI DMA latency: count=101098, avg=274 us, min=200 us, max=1921 us, last=457 us
BLE log SPI queue wait: count=101098, avg=82 us, min=24 us, max=1716 us, last=281 us
BLE log SPI DMA time: count=101098, avg=192 us, min=176 us, max=226 us, last=176 us

client:
BLE log buffer owned latency: count=68456, avg=212 us, min=127 us, max=664 us, last=226 us
BLE log SPI DMA latency: count=68456, avg=210 us, min=125 us, max=662 us, last=224 us
BLE log SPI queue wait: count=68456, avg=35 us, min=24 us, max=453 us, last=25 us
BLE log SPI DMA time: count=68456, avg=175 us, min=100 us, max=229 us, last=199 us
```

结论：

- 真实 SPI DMA time 基本在理论预期附近。
  - server 平均约 `192 us`，max `226 us`。
  - client 平均约 `175 us`，max `229 us`。
- 大的 max 主要来自 `SPI queue wait`。
  - server queue wait max `1716 us`。
  - client queue wait max `453 us`。
- server 侧 queue wait 明显更高，说明 server 接收侧 log 产生更密集，SPI driver queue 更容易积压。
- 因此 `SPI DMA latency max` 偶发到 1.8 ms，并不是单笔 512B SPI 传输真的用了 1.8 ms，而是 transaction 在 SPI driver queue 中等待了较久。

## 8. 综合分析

### 8.1 Log 流量上限压力

从已有数据看，2M throughput 场景下 BLE log 流量可以达到：

```text
约 3.6 Mbps 到 4.7 Mbps
```

当前 SPI 理论能力：

```text
20 MHz / 8 = 2.5 MB/s = 20 Mbps
```

若考虑 512B buffer 和约 200us 传输时间，理论输出能力约：

```text
512 B / 204.8 us = 2.5 MB/s ≈ 20 Mbps
```

你给出的系统估算上限约：

```text
15.28 Mbps
```

无论按 15.28 Mbps 还是理论 20 Mbps 看，平均 log 流量暂未超过 SPI 输出能力。但实际系统仍存在 queue wait 峰值，说明问题不是平均带宽不够，而是瞬时 burst 和 buffer/queue 管理导致的排队。

### 8.2 Host 与 LL 流量占比

已有数据中，LL log 是主要来源。

例如 Bluedroid write 发送侧：

```text
host = 3,247,263 B
ll   = 31,909,107 B
total = 35,156,370 B
```

LL 占比约：

```text
31,909,107 / 35,156,370 ≈ 90.8%
```

接收侧：

```text
host = 4,171,634 B
ll   = 25,541,394 B
total = 29,713,028 B
```

LL 占比约：

```text
25,541,394 / 29,713,028 ≈ 86.0%
```

因此，若要降低 log 流量，应优先关注 Controller/LL 层日志，而不是 Host 层日志。

### 8.3 Buffer 管理问题

延迟拆分说明：

- runtime task 调度不是主要瓶颈。
- SPI 真实 DMA time 接近理论值。
- 峰值主要来自 SPI driver queue wait。

因此当前更像是 buffer 管理和 SPI transaction 排队问题：

- 瞬时 log burst 会提交多个 buffer 到 SPI driver queue。
- 后续 buffer 的 `queue_wait` 会累积前面多个 transaction 的发送时间。
- server 接收侧 log 更多，因此 queue wait max 更高。

## 9. 后续建议

### 9.1 补齐测试矩阵

后续需要补充：

- ESP IP controller。
- Ceva IP controller。
- 自研 C6 controller。
- C3 / S3 controller flash-only 模式。
- NimBLE / Bluedroid 在同一场景下的对比。

C3、S3 的 controller flash-only 配置方式需要向 Jianhua 或 Haipeng 确认。

### 9.2 继续确认高 log 场景

建议向以下同事确认高 log 场景：

- Shen Weilong
- Zhang Haipeng
- Luo Xu

可能需要构造的场景包括：

- 高频 scan / extended scan。
- 大量连接建立与断开。
- PHY update / connection parameter update。
- 高速 write without response。
- 高速 notify。
- pairing / encryption / security 相关流程。
- controller 异常或重传较多的弱信号场景。

### 9.3 优化方向

如果目标是降低丢 log 或降低 buffer 等待时间，可以考虑：

1. 降低 LL log 量。
   - 优先减少高频 controller/LL log。
   - 增加分级过滤或采样。

2. 控制 SPI driver queue 深度。
   - 当前 queue wait 峰值说明 SPI driver queue 会积压。
   - 可以评估减小 queue depth 是否能降低 max latency，但可能增加丢 log 风险。

3. 增大 buffer 或调整 buffer 数。
   - 增大单 buffer 可以降低 transaction 次数。
   - 但单次 SPI DMA 时间会变长，需要平衡 latency 与 throughput。

4. 保留 queue_wait / dma_time 统计。
   - 后续调参时，这两个指标能直接判断问题发生在排队还是传输。

## 10. 当前结论

基于已有测试数据，可以得到以下结论：

1. 2M throughput 场景下，BLE log 平均流量可达到约 `3.6-4.7 Mbps`。
2. LL / Controller 层是主要 log 来源，通常占总 log 量 85% 以上。
3. 连接建立窗口 log 约 `10.5 KB`，耗时约 `117-118 ms`。
4. 连接断开窗口 log 约 `695 B`，耗时约 `10 ms`。
5. 真实 SPI DMA 传输时间接近理论值，约 `175-192 us` 平均，max 约 `226-229 us`。
6. `SPI DMA latency max` 偶发到毫秒级，主要来自 SPI driver queue wait，而不是单笔 SPI 传输变慢。
7. direct-send 绕过 runtime task 后，max 仍然存在，说明 runtime task 调度不是主要瓶颈。
8. 后续优化重点应放在 LL log 降量、SPI queue 积压控制、buffer sizing 和 burst 管理上。
