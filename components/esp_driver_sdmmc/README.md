# SDMMC Host Driver

SD Host side related components are:
- `sdmmc`
- `esp_driver_sdmmc` (current component)
- `esp_driver_sdspi`

For relationship and dependency among these components, see [SD Host Side Related Component Architecture](../sdmmc/README.md).

`esp_driver_sdmmc` components holds SDMMC Host driver for ESP SDMMC peripheral, this driver provides APIs to help you:
- do SD transactions (under SD mode) via ESP SDMMC peripheral.
- tune ESP SDMMC hardware configurations, such as clock frequency, bus width, etc.
- ...

You can
- use this driver to implement `sdmmc` protocol interfaces
- directly use `esp_driver_sdmmc` APIs

to communicate with SD slave devices under SD mode.
