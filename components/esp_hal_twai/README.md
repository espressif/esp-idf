# ESP Hardware Abstraction Layer for TWAI Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_twai` component provides a **Hardware Abstraction Layer** for the TWAI (Two-Wire Automotive Interface) peripheral across all ESP-IDF supported targets. TWAI is Espressif's implementation of the CAN (Controller Area Network) protocol, widely used in automotive and industrial applications for reliable communication between microcontrollers and devices.

## Architecture

The TWAI HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control the TWAI peripheral (e.g., initialization, message transmission/reception, error handling, bus recovery).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported TWAI Controllers

This HAL supports two versions of TWAI controllers depending on the ESP chip:

- **TWAI Version 1**: Standard CAN 2.0 controller supporting standard and extended frame formats
  - Implementation: `twai_hal_v1.c`

- **TWAI Version 2**: Enhanced controller with TWAI-FD (CAN-FD) support, enabling flexible data-rate for higher bandwidth communication
  - Implementation: `twai_hal_v2.c`

## Features

- Message transmission and reception with configurable filtering
- Standard (11-bit) and extended (29-bit) identifier support
- Configurable bit timing and baud rates
- Error detection and error state management
- Bus-off recovery mechanisms
- Interrupt-driven operations
- Self-test mode support
- Acceptance filters for selective message reception
- CAN-FD support with flexible data-rate (on supported chips)

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as the `esp_driver_twai` component.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
