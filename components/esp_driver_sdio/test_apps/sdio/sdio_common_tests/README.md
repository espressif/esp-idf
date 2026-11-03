# SDIO Cross Chips Test Apps

This folder contains SDIO related tests. This folder contains two sub-folders:
- host_sdmmc: this fodler contains the ESP32 SDMMC test app which works as the host to test the SDIO Slave peripheral
- sdio: this fodler contains the ESP SDIO Slave test apps

Tests in this folder are dual-board tests, except for the `[sdio_standalone]` cases of the sdio app, which run on the slave board alone.
