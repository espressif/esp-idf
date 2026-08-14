| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | --------- |

# Wi-Fi OTA Size Metrics App

This build-only app tracks the size of a minimal application using Wi-Fi and
HTTPS OTA. It calls the production APIs so their implementations are retained
by the linker, but it is intentionally skipped by the target-test pipeline.
