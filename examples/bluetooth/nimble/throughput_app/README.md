# Throughput Demo Examples

There are two example folders inside this `throughput_app`: `bleprph_throughput` (peripheral) and `blecent_throughput` (central). These examples demonstrate BLE GATT throughput measurement using NimBLE on ESP32. Two ESP32 boards are needed to run this demo. The `blecent_throughput` example has CLI support to select GATT operation from READ/WRITE/NOTIFY and configure connection parameters at runtime. More details can be found in respective READMEs.

## Using the Examples

Build and flash two ESP32 boards with `bleprph_throughput` and `blecent_throughput` examples. The central automatically scans and connects to the peripheral based on device name (`nimble_prph`). After connection, the user may optionally configure connection parameters (`MTU`, `connection interval`, `latency`, `supervision timeout`, `connection event length`). Then the user specifies the throughput test type (`read`, `write` or `notify`) and test duration in seconds.

Below are sample throughput numbers for a 60-second test run (MTU = 512, conn itvl = 7.5ms, DLE = 251 bytes, 1M PHY):

|GATT Method | Measurement Time | Application Throughput|
|--- | --- | ---|
|NOTIFY	| 60 seconds | ~340 Kbps|
|READ	| 60 seconds | ~200 Kbps|
|WRITE	| 60 seconds | ~500 Kbps|

The notify throughput output is displayed on the `bleprph_throughput` console, while read/write throughput results are shown on the `blecent_throughput` console.

## Throughput Optimization

The following parameters have the most significant impact on throughput:

1. **Connection Interval**: The default is 7.5ms (minimum allowed by BLE spec). Shorter intervals mean more connection events per second and higher throughput.

2. **Connection Event Length**: Controls how long a single connection event can last. The default maximum CE length is 15ms, allowing the controller to extend events and transmit more PDUs per event when possible.

3. **Notification Pipelining**: The peripheral queues multiple notifications simultaneously (pipeline depth of 15) rather than waiting for each one to complete before sending the next. This allows the controller to pack multiple PDUs into each connection event.

4. **Payload Size**: Payload sizes are set to the maximum ATT capacity for each operation (509 bytes for notify/write, 510 bytes for read) to maximize data per PDU.

5. **Data Length Extension (DLE)**: LL packet length is set to 251 bytes to use the maximum Link Layer PDU size.

6. **MTU**: Set to 512 bytes to allow large ATT payloads and reduce protocol overhead.

7. **MSYS Buffer Count**: Both peripheral and central are configured with 50 MSYS blocks (`CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT=50`) to provide sufficient buffer space for high-throughput operations.

8. **PHY**: On BLE 5.0 supported chipsets, 2M PHY can be selected to double the air data rate. Use Extended Advertising mode and specify PHY in the throughput CLI command.
