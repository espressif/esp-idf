Security
========

:link_to_translation:`zh_CN:[中文]`

Memory Protection
-----------------

Starting from **ESP-IDF v6.2**, on ESP32-C5, ESP32-C6, ESP32-C61, ESP32-H2, and ESP32-P4 the second stage bootloader no longer configures or locks any RISC-V PMP entries. The complete PMP configuration is now programmed and locked by the application during early startup.

A PMP entry locked by the bootloader cannot be reconfigured until the next CPU reset. Since the bootloader on a deployed device is generally never updated, entries locked by an older bootloader could conflict with the PMP layout expected by an application built with a newer ESP-IDF version, leading to boot failures or to memory protection being silently inactive.

Applications built with ESP-IDF are not affected, as they program and lock the full PMP configuration themselves during startup, before any application code runs.

Custom (non-ESP-IDF) applications launched by the ESP-IDF second stage bootloader must not assume that any PMP entries are pre-configured or locked at handoff. Previously, the bootloader configured and locked entries covering, e.g., the ROM and the peripheral address spaces; such applications must now program their own PMP configuration.
