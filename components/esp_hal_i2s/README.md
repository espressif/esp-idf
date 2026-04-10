# ESP Hardware Abstraction Layer for I2S Peripheral(s)

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_i2s` component provides a **Hardware Abstraction Layer** for Inter-IC Sound (I2S) controllers across all ESP-IDF supported targets. I2S is a serial bus interface standard used for connecting digital audio devices, enabling high-quality audio data transmission between integrated circuits.

## Architecture

The I2S HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control I2S peripherals (e.g., initialization, clock configuration, STD/TDM/PDM slot configuration, channel enable/disable).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported I2S Controllers

This HAL supports two I2S controller types depending on the ESP chip:

- **Standard I2S**: Traditional I2S interface supporting master/slave modes. And the different hardware versions are supported regarding different chips.
- **Low-Power I2S (LP I2S)**: Power-efficient I2S controller for low-power applications (on supported chips)

## Features

- **Multiple Audio Formats**:
  - Standard I2S (STD) mode
  - Pulse Density Modulation (PDM) mode
  - Time Division Multiplexing (TDM) mode
- **Master/Slave Mode Support**: Configurable as master or slave device
- **Bidirectional Communication**: Independent TX and RX channel configuration
- **Clock Configuration**: Precise master clock (MCLK) and bit clock (BCLK) division
- **Slot Configuration**: Mono/stereo mode, slot selection, bit width configuration
- **PDM/PCM Converter Features**: PCM-to-PDM and PDM-to-PCM conversion, high-pass filtering (on supported chips)
- **TDM Features**: Multi-slot TDM support with configurable slot masks
- **Signal Connection Management**: GPIO signal mapping and interrupt configuration

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_i2s`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
