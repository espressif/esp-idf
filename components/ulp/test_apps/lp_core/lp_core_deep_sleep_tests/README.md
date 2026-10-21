| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- |

# LP Core Deep Sleep Tests

Multi-device tests for LP-UART and LP-GPIO while the HP core is in deep sleep.

The LP core is stalled across sleep entry, so each case arms an LP wakeup source and does the work in LP `main()` after HP is asleep:

- **LP timer** (about 500 ms) for transfers that must happen during sleep: UART TX/RX/print/FIFO, GPIO input/output, GPIO interrupt
- **LP UART character sequence** (`he**o`, stimulus `hello`) to wake the LP core from deep sleep
- **LP IO falling edge** to wake the LP core from deep sleep

Each transfer case runs a second sleep cycle without calling `lp_core_uart_init` or `rtc_gpio_init` again, and checks that the programmed register value is unchanged.

DUT 0 is the sleeper. DUT 1 is the peer. Both boards run this app.

## Hardware setup

Connect the same GPIO number on both boards, plus a common ground. UART TX/RX is crossed in software, so the cable itself is straight (GPIO5 to GPIO5, not TX to RX).

| Target | LP UART (straight wires) | LP GPIO |
| ------ | ------------------------ | ------- |
| ESP32-C5 | GPIO4, GPIO5 | GPIO2 |
| ESP32-C6 | GPIO4, GPIO5 | GPIO2 |
| ESP32-P4 | GPIO14, GPIO15 | GPIO2 |
| ESP32-S31 | GPIO6, GPIO7 | GPIO2 |

The edge-wakeup case is built only when `SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED` is set. That macro is not declared for ESP32-S31.

## Running locally

```bash
cd components/ulp/test_apps/lp_core/lp_core_deep_sleep_tests
idf.py set-target esp32c5
pytest --target esp32c5,esp32c5 -m generic_multi_device --port '/dev/cu.usbserial-*'
```

`--target` lists one chip per DUT. The first port pytest opens is the sleeper.

## How a case is split across the two boards

Each scenario has two Unity cases that share one group tag.

`TEST_CASE_MULTIPLE_STAGES` runs on DUT 0, the sleeper. The HP core enters deep sleep, and the LP core runs `main()` after it wakes. A later stage checks the result once the HP core is awake again.

`TEST_CASE` runs on DUT 1, the peer. That board stays awake and sets up the pins. Pytest matches the two cases by group and starts the multi-stage case on DUT 0 and the normal case on DUT 1.

To add another scenario, copy an existing pair. Keep the same group tag on both cases, call `hp_core_deep_sleep_enter()` from the sleeper stage, and add a pytest function that passes that group to `TwoBoardSleep`.
