# ESP Hardware Abstraction Layer for Touch Sensor Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_touch_sens` component provides a **Hardware Abstraction Layer** for Touch Sensor controller supported targets. Touch sensors detect touch events by measuring changes in capacitance when a finger or object approaches the touch pad, enabling capacitive touch interfaces for user interaction.

## Architecture

The Touch Sensor HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control touch sensor peripherals (e.g., initialization, channel configuration, filter setup, measurement control).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported Touch Sensor Controllers

This HAL supports various touch sensor controller versions depending on the ESP chip:

- **Touch Sensor Version 1**: Basic touch sensor functionality (ESP32)
- **Touch Sensor Version 2**: Enhanced features including improved filtering and denoise capabilities (ESP32-S2, ESP32-S3)
- **Touch Sensor Version 3**: Advanced features with frequency hopping, multiple sample configurations, and enhanced waterproof support (ESP32-P4 and newer chips)

## Features

- **Channel Management**: Multi-channel touch pad support with independent configuration
- **Measurement Control**: Configurable charge/discharge cycles, voltage thresholds, and measurement intervals
- **Filtering and Signal Processing**:
  - Benchmark filter (IIR filter, jitter filter)
  - Smooth data filter for noise reduction
  - Debounce and noise threshold configuration
  - Active threshold hysteresis
- **Denoise Function**: Internal denoise channel (T0) to filter out power supply noise and external EMI
- **Waterproof Support**: Guard pad and shield channel configuration for water-resistant applications
- **Proximity Sensing**: Up to three touch channels can be configured as proximity sensors
- **Sleep Channel**: Deep sleep wake-up support with configurable sleep channel
- **FSM Operation**: Hardware timer or software-triggered measurement modes
- **Interrupt Handling**: Multiple interrupt types (active, inactive, done, scan done, timeout, proximity done)
- **Sample Configuration**: Multiple sample configurations with frequency hopping support (Version 3)

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_touch_sens`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
