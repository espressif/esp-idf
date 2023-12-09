# SDMMC Protocol Layer Driver

## SD Host Side Related Component Architecture

### Architecture Graph

                            ┌─────────────────────────────────────────────┐
                            │                                             │
                            │  SD Protocol Layer Driver: components/sdmmc │
                            │                                             │
                            └──────────▲────────────────────▲─────────────┘
                                       │                    │
                                       │                    │
                            Implements │                    │ Implements
                                       │                    │
    ┌──────────────────────────────────┴─────┐         ┌────┴───────────────────────────────────┐
    │                                        │         │                                        │
    │Host Driver: components/esp_driver_sdmmc│         │Host Driver: components/esp_driver_sdspi│
    │                                        │         │                                        │
    └────────────────────────────────────────┘         └────────────────────────────────────────┘

### Components

- `sdmmc`: SD protocol layer driver, it provides SD protocol related definitions and interfaces. With corresponding implementation drivers, `sdmmc` APIs can help you:
  - send commands to slave devices
  - send and receive data
  - handle error conditions within the bus
- `esp_driver_sdmmc`: SDMMC Host driver for ESP SDMMC hardware, it implements the `sdmmc` protocol interfaces.
- `esp_driver_sdspi`: SDSPI Host driver for ESP GPSPI hardware, it implements the `sdmmc` protocol interfaces.

### Dependency

- `esp_driver_sdmmc` is in driver layer (G2), it relies on `sdmmc`
- `esp_driver_sdspi` is in driver layer (G2), it relies on `sdmmc`
- `sdmmc` does not and should not rely on `esp_driver_sdmmc` or `esp_driver_sdspi`. Though `sdmmc` is independent, it still stays in G2.
