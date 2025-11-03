# ESP Hardware Abstraction Layer for Watch Dog Peripheral(s)

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The ESP Hardware Abstraction Layer for Watch Dog Peripherals (`esp_hal_wdt`) provides a unified interface to interact with various watchdog timer (WDT) peripherals across all ESP chip families. This component abstracts the hardware-specific details of different watchdog implementations, enabling consistent usage patterns regardless of the underlying hardware.

The component supports multiple types of watchdog timers:
- RTC Watchdog Timer (RWDT)
- Main System Watchdog Timer (MWDT) of Timer Group(s)
- XTAL32K Watchdog Timer (where supported)

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with watch dog peripherals, including:
   - Initialization and de-initialization functions
   - Timer configuration and stage management
   - Feeding (resetting) the watchdog timer
   - Interrupt handling
   - Enable/disable functionality

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility
   - Clock and prescaler settings

## Features

- Unified watch dog interface across all ESP chip families
- Support for multiple watchdog stages with configurable timeout actions
- Flexible reset signal configuration (length and type)
- Interrupt management
- Platform-specific optimizations
- IRAM-safe implementation for critical sections
- Chip-specific implementations contained in separate directories

## Usage

This component is primarily used by ESP-IDF system services such as **interrupt watchdog** and **task watchdog**.

For advanced developers implementing custom watch dog solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
- `esp_hal_timg`: Timer group HAL for shared resources with the general purpose timer in the same group
