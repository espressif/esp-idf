| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Configdep test application

Minimal ESP-IDF project used by ``tools/test_build_system/test_configdep.py`` to exercise ``esp-idf-configdep`` selective rebuilds and to assert that the built firmware reflects Kconfig toggles (via strings embedded in the ELF).
