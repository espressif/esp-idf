# ESP Hardware Abstraction Layer for PPA Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_ppa` component provides a **Hardware Abstraction Layer** for the PPA (Pixel Processing Accelerator) peripheral across ESP-IDF supported targets that implement it.

## Architecture

The PPA HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control the PPA peripheral (e.g., initialization, task submission, engine control, and synchronization).
2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Features

- Engine initialization and reset helpers
- Task/command configuration helpers (depending on target capabilities)
- Interrupt and status helpers
- Clock and power-management hooks (on supported chips)

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_ppa`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
