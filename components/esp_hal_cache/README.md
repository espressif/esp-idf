# ESP Hardware Abstraction Layer for Cache and MMU

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_cache` component provides a **Hardware Abstraction Layer** for the cache and the external-memory MMU across all ESP-IDF supported targets. It serves as a foundation for higher-level drivers (DMA drivers, `esp_mm`, `spi_flash`, etc.) by offering a consistent interface to cache maintenance and external-memory address translation, while hiding the complexities of chip-specific implementations.

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with the cache and MMU, including:
   - Cache initialization, enabling and disabling
   - Cache maintenance operations (writeback, invalidate, writeback + invalidate, sync)
   - Cache freezing and preloading on supported chips
   - MMU mapping operations (virtual-to-physical address translation for flash and PSRAM)

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions (`<target>/include/hal/cache_ll.h` and `<target>/include/hal/mmu_ll.h`)
   - Chip-specific register configurations
   - Hardware feature compatibility

Implementation notes:

- `cache_hal.c` implements the target-independent HAL sequences; ESP32 uses its dedicated ROM-compatible implementation (`esp32/cache_hal_esp32.c`).
- `<target>/cache_periph.c` contains target-specific cache peripheral data.

## Usage

This component is primarily used by ESP-IDF drivers and system components such as `esp_driver_dma`, `esp_mm`, `esp_mspi`, `spi_flash` and `esp_psram`, as well as by the bootloader.

For advanced developers implementing custom DMA or memory-management solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides cache and MMU register definitions and SoC capabilities
- `hal`: Core hardware abstraction utilities and macros
