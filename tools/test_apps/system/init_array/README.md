| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | ----- |

# Historical note

Initially, ESP-IDF used the `do_global_ctors()` function to run global constructors.
This was done to accommodate Xtensa targets that emit `.ctors.*` sections, which are ordered in descending order.

For RISC-V, compilation used `.init_array.*` sections, which are designed to have ascending order.
Priority constructors in `.init_array.*` sections were correctly processed in ascending order.
However, non-priority `.init_array` section was processed in descending order (as it was done for Xtensa `.ctors`).

Starting with ESP-IDF v6.0, the implementation switched to the standard LibC behavior (`__libc_init_array()`),
which processes both priority and non-priority constructors in ascending order.

To achieve this, a breaking changes were introduced:
  - Xtensa `.ctors.*` entries converted to `.init_array.*` format (ascending), to be passed to `__libc_init_array()`.
  - Processing order of non-priority `.init_array` and `.ctors` sections was changed from descending to ascending.

This test ensures that the initialization order is correct and consistent between ESP-IDF and Linux targets.
