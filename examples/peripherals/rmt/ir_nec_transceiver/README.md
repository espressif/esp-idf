| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |
# IR NEC Encoding and Decoding Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

[NEC](https://www.sbprojects.net/knowledge/ir/nec.php) is a common use IR protocol, this example creates a TX channel and sends out the IR NEC signals periodically. The signal is modulated with a 38KHz carrier. The example also creates an RX channel, to receive and parse the IR NEC signals into scan codes.

## How to Use Example

### Hardware Required

* A development board with supported SoC mentioned in the above `Supported Targets` table
* An USB cable for power supply and programming
* A 5mm infrared LED (e.g. IR333C) used to transmit encoded IR signals
* An infrared receiver module (e.g. IRM-3638T), which integrates a demodulator and AGC circuit

### Hardware Connection

```
   IR Receiver (IRM-3638T)                 ESP Board                        IR Transmitter (IR333C)
+--------------------------+       +----------------------+              +---------------------------+
|                        RX+-------+IR_RX_GPIO  IR_TX_GPIO+--------------+TX                         |
|                          |       |                      |              |                           |
|                       3V3+-------+3V3                 5V+--------------+VCC                        |
|                          |       |                      |              |                           |
|                       GND+-------+GND                GND+--------------+GND                        |
+--------------------------+       +----------------------+              +---------------------------+
```

The TX and RX GPIO number used by this example can be changed in [the source file](main/ir_nec_transceiver_main.c) via `EXAMPLE_IR_TX_GPIO_NUM` and `EXAMPLE_IR_RX_GPIO_NUM`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Run this example, you might see the following output log:

```
...
I (0) cpu_start: Starting scheduler on APP CPU.
I (306) example: create RMT RX channel
I (306) gpio: GPIO[19]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (316) example: register RX done callback
I (316) example: create RMT TX channel
I (326) gpio: GPIO[18]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (336) example: modulate carrier to TX channel
I (336) example: install IR NEC encoder
I (346) example: start RMT TX and RX channel

NEC frame start---
{0:9020},{1:4461}
{0:577},{1:577}
{0:577},{1:576}
{0:552},{1:601}
{0:552},{1:601}
{0:577},{1:576}
{0:578},{1:575}
{0:583},{1:572}
{0:579},{1:574}
{0:576},{1:1648}
{0:553},{1:1673}
{0:579},{1:1647}
{0:580},{1:1645}
{0:577},{1:1649}
{0:554},{1:1673}
{0:578},{1:1648}
{0:553},{1:1673}
{0:555},{1:1671}
{0:578},{1:577}
{0:553},{1:1673}
{0:554},{1:1671}
{0:555},{1:601}
{0:580},{1:574}
{0:551},{1:603}
{0:580},{1:574}
{0:553},{1:601}
{0:553},{1:1672}
{0:554},{1:602}
{0:552},{1:603}
{0:579},{1:1646}
{0:554},{1:1672}
{0:555},{1:1672}
{0:580},{1:1646}
{0:555},{1:0}
---NEC frame end: Address=FF00, Command=F20D

NEC frame start---
{0:9024},{1:2213}
{0:583},{1:0}
---NEC frame end: Address=FF00, Command=F20D, repeat

NEC frame start---
{0:584},{1:0}
---NEC frame end: Unknown NEC frame

...
```

In the example's main loop, the RX channel waits for any NEC frames, if nothing received within 1 second, the TX channel will send out a predefined NEC frame (address=0x0440, command=0x3003).

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
