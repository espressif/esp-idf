# `esp_hal_sd`

⚠️ This HAL component is still under heavy development at the moment, so we don't guarantee the stability and backward-compatibility among versions.

The `esp_hal_sd` component provides a **Hardware Abstraction Layer** for
- SDMMC Host Controller
- SDIO Slave Controller

for all targets supported by ESP-IDF.

In a broad sense, the HAL layer consists of two sub-layers: HAL (upper) and Low-Level(bottom). The HAL layer defines the steps and data that is required to operate a peripheral (e.g. initialization, parameter settings). The low-level is a translation layer above the register files under the `soc` component, it only covers general conceptions to register configurations.

The functions in this file mainly provide hardware abstraction for IDF peripheral drivers. For advanced developers, the HAL layer functions can also be directly used to assist in implementing their own drivers. However, it needs to be mentioned again that the interfaces here do not guarantee stability.
