# ESP Hardware Abstraction Layer for RMT Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_rmt` component provides a **Hardware Abstraction Layer** for the RMT (Remote Control Transceiver) peripherals across all ESP-IDF supported targets. It serves as a foundation for the higher-level RMT drivers, offering a consistent interface to interact with RMT hardware while hiding the complexities of chip-specific implementations.

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with RMT peripherals, including:
   - Initialization and de-initialization
   - TX/RX channel control operations
   - Memory and DMA management
   - Carrier and modulation configuration

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility

## Features

- Unified RMT interface across all ESP chip families
- Support for both transmit and receive channels
- Flexible pulse timing and encoding capabilities
- Carrier wave modulation for IR remote control
- Loop transmission mode for repeated patterns
- DMA support on capable chips for large data transfers
- Multiple clock source options
- Memory block allocation and management

## Usage

This component is primarily used by ESP-IDF peripheral drivers such as `esp_driver_rmt`. 

For advanced developers implementing custom RMT solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
