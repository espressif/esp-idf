# ESP Hardware Abstraction Layer for PCNT Peripherals

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_pcnt` component provides a **Hardware Abstraction Layer** for PCNT (Pulse Counter) peripherals across ESP-IDF supported targets. This HAL enables efficient counting of external pulses and signals, with support for quadrature encoders, frequency measurement, and position tracking applications.

## Architecture

The PCNT HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control PCNT peripherals (e.g., initialization).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Features

### Core Counting Features
- Bidirectional pulse counting (increment/decrement)
- Configurable counting range with high/low limits
- Counter value read operations
- Counter clear and reset functionality
- Start/stop control for counting operations

### Channel Configuration
- Edge action configuration:
  - Hold: Keep current count value
  - Increase: Increment count value
  - Decrease: Decrement count value
- Level action configuration:
  - Keep: Maintain current count mode
  - Inverse: Invert count direction
  - Hold: Freeze count value

### Event Detection
- Threshold events
- High/low limit events
- Zero-crossing events
- Watch-point events
- Step events

### Signal Processing
- Glitch filtering with configurable filter width
  
## Usage

The HAL functions primarily serve ESP-IDF PCNT peripheral drivers such as `esp_driver_pcnt` component.

Advanced developers can use these interfaces directly when implementing custom pulse counting applications, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions and peripheral capabilities
- `hal`: Core hardware abstraction utilities and macros
