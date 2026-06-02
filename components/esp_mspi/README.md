# `esp_mspi`

The `esp_mspi` component provides shared low-level support for flash and PSRAM connected through the MSPI peripheral.

- Coordinates flash operations, cache handling, and memory access.
- Provides MSPI timing tuning, high-performance mode, and power management support.
- Provides shared alignment and memory-barrier utilities.
- Serves as the common MSPI layer used by `spi_flash` for flash access and by `esp_psram` for external RAM support.
