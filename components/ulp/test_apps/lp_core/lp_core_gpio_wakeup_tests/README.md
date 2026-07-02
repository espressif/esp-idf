| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# LP Core GPIO Wakeup Tests

This test app verifies LP core GPIO wakeup from deep sleep using an **external GPIO toggle** provided by a second board.

It exercises the same flow as the `gpio_wakeup` example:

1. HP configures the wakeup GPIO, loads and starts the LP program, and enters deep sleep
2. The waker holds the GPIO at the idle high level long enough to catch spurious wakeups
3. An external falling edge wakes LP, which records the LP wakeup cause, wakes HP, and clears GPIO interrupt status
4. HP verifies the ULP wakeup cause and the LP GPIO wakeup cause

## Hardware setup

This is a `generic_multi_device` test. Two boards of the same target must have the configured wakeup GPIO pin wired together (default: GPIO2).

- **Wakee (DUT 0):** multi-stage test that enters deep sleep and verifies ULP wakeup after an external falling edge
- **Waker (DUT 1):** provides separate Unity test cases to drive the shared GPIO high or low

The wakee side uses `TEST_CASE_MULTIPLE_STAGES` because deep sleep reboots the chip back to the Unity menu; pytest reruns the after-wake stage once the board wakes up. Pytest drives the waker GPIO high before the wakee enters deep sleep, verifies that the wakee does not reboot early, and then drives the falling edge.

## Running locally

```bash
cd components/ulp/test_apps/lp_core/lp_core_gpio_wakeup_tests
idf.py set-target esp32p4
idf.py build
pytest --target esp32p4 -m generic_multi_device
```
