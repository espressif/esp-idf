# ESP Hardware Abstraction Layer for Analog Comparator Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_ana_cmpr` component provides a **Hardware Abstraction Layer** for Analog Comparator peripherals across supported ESP-IDF targets. Analog comparators compare two analog voltage signals and generate digital outputs based on the comparison result, enabling threshold detection, zero-crossing detection, and analog signal monitoring applications.

## Architecture

The Analog Comparator HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control analog comparator peripherals (e.g., initialization, reference voltage configuration, cross detection setup, interrupt handling).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported Controllers

This HAL supports analog comparator controllers on the following ESP chips:

- **ESP32-P4**: Dual analog comparator units (ANA_CMPR_U0, ANA_CMPR_U1)
- **Others**: Single analog comparator unit

## Features

### Reference Voltage Configuration

- **Internal Reference Voltage**: Configurable internal reference voltage from 0% to 70% VDD in 10% steps (0.0V, 0.1VDD, 0.2VDD, ..., 0.7VDD)
- **External Reference Voltage**: Support for external reference voltage via dedicated GPIO pad (typically GPIO10)
- **Reference Source Selection**: Switch between internal and external reference sources

### Cross Detection

- **Positive Cross Detection**: Detects when the input analog signal crosses the reference voltage from low to high
- **Negative Cross Detection**: Detects when the input analog signal crosses the reference voltage from high to low
- **Any Cross Detection**: Detects both positive and negative crossings
- **Debounce Filtering**: Configurable debounce cycle to ensure stable cross detection and filter out noise

### Interrupt Handling

- **Multiple Interrupt Types**: Support for positive cross, negative cross, and any cross interrupts
- **Interrupt Masking**: Flexible interrupt enable/disable control per interrupt type
- **Interrupt Status**: Read interrupt status and clear interrupt flags
- **Interrupt Register Access**: Direct access to interrupt status register for advanced use cases

### Signal Processing

- **Debounce Cycle**: Configurable debounce cycle count to filter out glitches and ensure stable detection
- **Hardware Filtering**: Built-in hardware filtering for noise reduction

### Event Task Matrix (ETM) Support

- **ETM Event Source**: Analog comparator cross events can be used as ETM event sources for precise timing and synchronization
- **Multiple Event Types**: Support for positive and negative cross events as separate ETM sources

### Peripheral Configuration

- **GPIO Mapping**: Each comparator unit has dedicated source GPIO and external reference GPIO pins
- **Module Identification**: Each comparator unit has a unique module name for identification
- **Interrupt Source**: Each comparator unit has a dedicated interrupt source

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_ana_cmpr`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
