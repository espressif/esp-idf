| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Light Sleep Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates usage of light sleep mode. Unlike deep sleep mode, light sleep preserves the state of the memory, CPU, and peripherals. Execution of code on both CPUs is stopped when `esp_light_sleep_start()` function is called. When the chip exits light sleep mode, execution continues at the point where it was stopped, and `esp_light_sleep_start()` function returns.

The example enables the following wakeup sources:

- Timer: wake up the chip in 2 seconds
- EXT0: wake up the chip if a button attached to GPIO0 is pressed (i.e. if GPIO0 goes low)
- UART0: wake up the chip when the uart rx pin receive more than or equal to 3 rising edges.

The example also prints time spent in light sleep mode to illustrate that timekeeping continues while the chip is in light sleep.

Note: If you find that the bottom current measured by running this example is larger than what is declared on the datasheet, you can try the following methods:

- configure the CPU to be powered down via menuconfig (not all esp series support this feature)
- configure the SPI Flash to be powered down via menuconfig

## How to Use Example

### Hardware Required

This example can be used with any ESP32 development board. Most boards have a button attached to GPIO0, often labelled `BOOT`. If the board does not have such button, an external button can be connected, along with a 10k pull-up resistor, and a 100nF capacitor to ground for debouncing.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Wake-up by Timer

If do nothing to the example, the chip will wake-up every 2000 ms by timer, and fall into light sleep again after print some logs. We can see the wake-up reason is `timer` in this case.

### Wake-up by GPIO

For this example, the wake-up GPIO is bound to the 'BOOT' button on the board, we can wake-up the chip from light sleep by pressing the 'BOOT' button, the chip will wake-up immediately after we pressed the button. We can see the wake-up reason is `pin` in this case and the chip will fall into light sleep again if we release the button.

### Wake-up by UART

For this example, the wake-up UART is bound to the default console port (UART_NUM_0), we can wake-up the chip from light sleep by inputting some keys on the key board. We can see the wake-up reason is `uart` in this case.

Note #1: the UART wake-up threshould is set to 3 in this example, which means the ascii code of the input character to wake-up the chip should has at least 3 rising edges (including the stop bit per character), for example, the ascii code of character `0` is `0011 0000` in binary, which only contains 2 rising edges, so the character `0` is not enough to wakeup the chip.

Note #2: only UART0 and UART1 (if has) are supported to be configured as wake up source. And for ESP32, we have to use iomux pin for RX signal (i.e. GPIO3 for UART0 & GPIO9 for UART1), otherwise it won't success.

Note #3: due to limitation of the HW, the bytes that received during light sleep is only used for waking up, and it will not be received by UART peripheral or passed to the driver.

Note #4: after waking-up from UART, you should send some extra data through the UART port, so that the internal wakeup indication signal can be cleared. Otherwises, the next UART wake-up would trigger with two less rising edges than the configured threshold value.

### Wake-up by Touch Pad

For this example, pressing any registered touch buttons can wake up the chip.

Note #1: For light sleep, all registered touch buttons can wake up the chip. But only the channel which is configured as wake up channel can wake up the chip from deep sleep.

Note #2: Waking-up by touch pad relies on 'touch_element' driver, which can only support ESP32-S2 and ESP32-S3 currently.

```
Entering light sleep
Returned from light sleep, reason: timer, t=2713 ms, slept for 1999 ms
Entering light sleep
Returned from light sleep, reason: timer, t=4722 ms, slept for 2000 ms
Entering light sleep
Returned from light sleep, reason: uart, t=5148 ms, slept for 418 ms
Entering light sleep
Returned from light sleep, reason: uart, t=6178 ms, slept for 1022 ms
Entering light sleep
Returned from light sleep, reason: timer, t=8187 ms, slept for 2000 ms
Entering light sleep
Returned from light sleep, reason: timer, t=10195 ms, slept for 2000 ms
Entering light sleep
Returned from light sleep, reason: timer, t=12203 ms, slept for 2000 ms
Entering light sleep
Returned from light sleep, reason: pin, t=12555 ms, slept for 342 ms
Waiting for GPIO9 to go high...
Entering light sleep
Returned from light sleep, reason: pin, t=12564 ms, slept for 1 ms
Waiting for GPIO9 to go high...
Entering light sleep
...
I (361) touch_wakeup: Button[1] Press
Returned from light sleep, reason: touch, t=14471 ms, slept for 467 ms
Entering light sleep

...
```

In the scenario above, the button attached to GPIO0 was pressed and held for about 3 seconds, after the 2nd wakeup from light sleep. The program has indicated the wakeup reason after each sleep iteration.

## Current Consumption

In this example, current consumption in light sleep mode is in the range of 0.8 — 1.1 mA. Current consumption in active mode is 28 — 32 mA. Average current consumption is 1.1 - 1.3 mA.

![Current consumption overview graph](image/light_sleep_scope.png)

![Current consumption in active mode](image/light_sleep_scope_zoom.png)

## Troubleshooting

If pressing the button attached to GPIO0 does not affect program behavior, check DTR/RTS configuration in the serial monitor. This is not necessary for IDF monitor, but for other tools it might be necessary to set DTR and RTS line state to "disabled" or "de-asserted".
