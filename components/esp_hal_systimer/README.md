# ESP Hardware Abstraction Layer for System Timer Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The ESP Hardware Abstraction Layer for System Timer Peripheral (`esp_hal_systimer`) provides a unified interface to interact with the system timer (SYSTIMER) peripheral across all ESP chip families. This component abstracts the hardware-specific details of different SYSTIMER implementations, enabling consistent usage patterns regardless of the underlying hardware.

The SYSTIMER is a high-resolution timer used for:
- System time tracking
- FreeRTOS tick generation
- High-precision timing operations
- Event timer (ETM) trigger generation

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with the SYSTIMER peripheral, including:
   - Initialization and de-initialization functions
   - Counter value operations
   - Alarm configuration and management
   - Time conversion utilities
   - Clock source configuration

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility
   - Clock and prescaler settings

## Features

- Unified SYSTIMER interface across all ESP chip families
- Support for multiple counters and alarms
- High-resolution timing (typically 16MHz resolution)
- Flexible clock source configuration
- ETM (Event Timer Module) support where available
- Platform-specific optimizations
- IRAM-safe implementation for critical sections
- ROM implementation support for certain chips
- Chip-specific implementations contained in separate directories

## Usage

This component is primarily used by ESP-IDF system services such as:
- **esp_timer**: High-resolution timer implementation
- **FreeRTOS**: System tick generation
- **esp_hw_support**: System time and clock management

For advanced developers implementing custom timing solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
- `esp_rom`: ROM function interfaces (when using ROM implementation)

## ROM Implementation

Some chips support using ROM-based SYSTIMER HAL implementations for reduced code size. This is controlled by the `CONFIG_HAL_SYSTIMER_USE_ROM_IMPL` configuration option. When enabled, the component uses ROM linker scripts to link against ROM functions instead of compiling the HAL implementation.
