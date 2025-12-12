# ESP Hardware Abstraction Layer for LEDC Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_ledc` component provides a **Hardware Abstraction Layer** for the LEDC (LED PWM Controller) peripheral across all ESP-IDF supported targets.

## Architecture

The LEDC HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control LEDC (e.g., timer/channel configuration, duty updates, fade and low-power helpers).
2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Features

- Multi-timer and multi-channel PWM generation
- High-speed and low-speed modes (on supported chips)
- Duty cycle setting and smooth fading
- Timer resolution and frequency configuration
- Interrupt-driven operations
- Low-power helpers (on supported chips)

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_ledc`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
