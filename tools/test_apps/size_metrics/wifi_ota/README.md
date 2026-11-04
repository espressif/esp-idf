| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | --------- |

# Wi-Fi OTA Size Metrics App

This build-only app tracks the size of a minimal application using Wi-Fi and
HTTPS OTA. It calls the production APIs so their implementations are retained
by the linker, but it is intentionally skipped by the target-test pipeline.

Build configs:
- `default`: Wi-Fi station with default modem sleep, no power management
- `pm`: adds `CONFIG_PM_ENABLE` with tickless idle, so automatic light sleep code (`esp_pm`, `esp_sleep`) is linked and tracked
- `pm_pd`: as `pm`, plus `CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP` and `CONFIG_ESP_PHY_MAC_BB_PD`, so peripheral and Wi-Fi MAC/BB retention code is tracked. Only built for targets that can power down the TOP domain
