# ESP Hardware Abstraction Layer for UART Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_uart` component provides a **Hardware Abstraction Layer** for Universal Asynchronous Receiver/Transmitter (UART) peripherals across all ESP-IDF supported targets. It also includes support for UHCI (Universal Host Controller Interface) on chips where it is available and enabled.

## Architecture

The UART HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control UART/UHCI peripherals (e.g., initialization, configuration, data movement, power-management hooks).
2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported Controllers

- **UART**: Standard UART controller available on all supported ESP chips
- **UHCI**: Available on selected chips; built when `CONFIG_SOC_UHCI_SUPPORTED` is enabled

## Features

- Configurable baud rate, data bits, parity, and stop bits
- Hardware and software flow control (on supported chips)
- RX/TX FIFO management
- Interrupt-driven operations
- DMA support on capable chips
- Clock source selection (on supported chips)
- Light-sleep and wakeup helpers (on supported chips)

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_uart`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
