# ESP Hardware Abstraction Layer for DMA Peripheral(s)

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_dma` component provides a **Hardware Abstraction Layer** for Direct Memory Access (DMA) controllers across all ESP-IDF supported targets. DMA allows peripherals to access memory directly without CPU intervention, significantly improving data transfer efficiency and reducing CPU load.

## Architecture

The DMA HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control DMA peripherals (e.g., initialization, channel configuration, transfer start/stop).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported DMA Controllers

This HAL supports various DMA controller types depending on the ESP chip:

- **GDMA (General DMA)**: Available in multiple versions:
  - AHB GDMA (Version 1 & 2)
  - AXI GDMA
- **DW GDMA**: DesignWare DMA controller
- **DMA2D**: 2D DMA controller for efficient image processing operations
- **CP DMA**: A dedicated DMA controller designed for highly efficient memory copy

## Features

- Channel allocation and management
- Transfer direction control (TX/RX)
- Trigger peripheral selection (M2M, SPI, I2S, etc.)
- Priority configuration for channels
- DMA descriptor handling
- CRC calculation support (on supported chips)
- ETM (Event Task Matrix) event handling

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_private/gdma.h`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
