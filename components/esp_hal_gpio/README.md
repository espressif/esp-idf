# ESP Hardware Abstraction Layer for GPIO Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_gpio` component provides a **Hardware Abstraction Layer** for General-Purpose Input/Output (GPIO) across all ESP-IDF supported targets.

## Architecture

The GPIO HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to configure and control GPIO pins (e.g., direction, level, pull mode, interrupt enable/disable).
2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Features

- Configure pin direction (input/output)
- Set and get logic levels
- Internal pull-up/pull-down configuration (on supported chips)
- Open-drain/output mode selection (on supported chips)
- Interrupt configuration (edge/level) and handling
- Drive strength configuration (on supported chips)
- Deep sleep and light sleep helpers for pin retention (on supported chips)

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers and system components that need efficient and portable GPIO control.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
