| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# "G1"-components-only app (build system v2)

Build system v2 variant of `../g1_components`. The component lists (`g1_setup.cmake`), the closure and dependency checks (`g1_check.cmake`, `check_dependencies.py`) and the `main` component are shared with the v1 app, so the two cannot drift apart.

Having both apps in the tree means every pipeline that builds test apps checks the G1 closure under both build systems, without a dedicated build system v2 job.

The expected component lists are not identical between the two build systems. Build system v2 can make a dependency conditional on a Kconfig option, so some components that v1 always pulls into the G1 build are absent here. `esp_usb_cdc_rom_console` is one such case: `esp_system` only needs it when the console is set to USB CDC.

For the same reason the v2 list is target dependent: v1 builds everything listed in `COMPONENTS`, while v2 only links what something actually depends on. `esp_hal_cam` (needed by `esp_hw_support` on ESP32-P4 only) and `esp_hal_i2s` (needed by `esp_hal_ana_conv` on ESP32 only) are therefore expected on those targets alone; `g1_check.cmake` drops them from the expected list elsewhere.

See `../g1_components/README.md` for how to use the app and how to interpret failures.
