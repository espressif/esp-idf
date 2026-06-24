# ESP Hardware Abstraction Layer for Timer Group Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_timg` component provides a **Hardware Abstraction Layer** for the General Purpose Timer peripherals across all ESP-IDF supported targets. It serves as a foundation for the higher-level timer drivers, offering a consistent interface to interact with timer hardware while hiding the complexities of chip-specific implementations.

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with timer peripherals, including:
   - Initialization and de-initialization
   - Timer control operations (start, stop, reload)
   - Alarm and event handling
   - Counter operations

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility

## Features

- Unified timer interface across all ESP chip families
- Support for different timer counting modes (up/down)
- Alarm functionality with configurable triggers
- Auto-reload capability
- ETM (Event Task Matrix) integration on supported chips
- Multiple clock source options

## Usage

This component is primarily used by ESP-IDF peripheral drivers such as `esp_driver_gptimer`. It is also utilized by system components like `esp_timer`.

For advanced developers implementing custom timer solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
