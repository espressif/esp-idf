| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# esp_hal_systimer ROM implementation test app

This test app verifies that the systimer HAL is correctly linked from either ROM or the component implementation, depending on `CONFIG_HAL_SYSTIMER_USE_ROM_IMPL`.

## Configurations

- **rom_impl**: Builds with `CONFIG_HAL_SYSTIMER_USE_ROM_IMPL=y`, tests that `systimer_hal_get_counter_value` is in ROM.
- **no_rom_impl**: Builds with `CONFIG_HAL_SYSTIMER_USE_ROM_IMPL=n`, tests that the function is not in ROM.

## Building and running

```bash
idf.py set-target <target>
idf.py build flash monitor
```

To run with pytest:

```bash
idf.py set-target <target>
idf.py build
pytest --target=<target>
```
