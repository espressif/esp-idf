# Throughput demo Examples

There are two different example folders inside this `throughput_app`, `bleprph_throughput` and `blecent_throughput`. As the names suggest, both of them play role of `peripheral` and `central` respectively. These example demonstrate application throughput for NimBLE on ESP32. Two ESP32 boards are needed to run this demo. The `blecent_throughput` example has CLI support to select GATT operation from READ/WRITE/NOTIFY. It can also accept connection parameters at start of program, more details can be found in respective READMEs.

## Using the examples

First build and flash two ESP32 boards with `bleprph_throughput` and `blecent_throughput` examples. The central automatically scans and connects to peripheral based on peripheral name string (`nimble_prph`). In the next step, user may choose to configure connection parameters (`MTU`, `connection interval`, `latency`, `supervision timeout`, `connection event length`). In the next step, user needs to specify throughput test name (`read`, `write` or `notify`) and test time in seconds. Below are
sample numbers of different throughput test runs for 60 seconds (MTU = 512, conn itvl = 7.5msec, conn event length = 7.5msec)

|GATT method | Measurement time | Application Throughput|
|--- | --- | ---|
|NOTIFY	| 60 seconds | ~340Kbps|
|READ	| 60 seconds | ~200kbps|
|WRITE	| 60 seconds | ~500kbps|

The notify output is seen on `bleprph_throughput` console and read/write throughput are seen on `blecent_throughput` console.
