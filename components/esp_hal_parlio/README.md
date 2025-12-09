# ESP Hardware Abstraction Layer for Parallel IO Peripheral(s)

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_parlio` component provides a **Hardware Abstraction Layer** for Parallel IO (PARLIO) peripherals across all ESP-IDF supported targets. PARLIO enables high-speed parallel data transfer between the ESP chip and external devices, supporting both transmit (TX) and receive (RX) operations with configurable data widths and timing characteristics.

## Architecture

The PARLIO HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control PARLIO peripherals (e.g., initialization, unit configuration, transfer start/stop).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported PARLIO Units

This HAL supports PARLIO peripherals with the following units depending on the ESP chip:

- **TX Unit**: Transmits parallel data to external devices
  - Configurable data width
  - Clock output or external clock input support
  - Chip select (CS) signal support (on some chips)
  - Valid signal generation and delay control

- **RX Unit**: Receives parallel data from external devices
  - Configurable data width
  - Clock input support
  - Multiple sampling modes (level-controlled, pulse-controlled, software-controlled)
  - Timeout detection support

## Features

### Clock Configuration
- Multiple clock source selection
- Configurable clock divider with integer and fractional support (on some chips)
- Independent clock configuration for TX and RX units

### Data Transfer Control
- Configurable bus width
- Bit packing order configuration (LSB/MSB)
- Sample clock edge selection (rising/falling edge)
- Frame length configuration

### RX Unit Features
- Level-controlled receive mode with active high/low enable signal
- Pulse-controlled receive mode with configurable start/end pulse counting
- Software-controlled receive mode
- RX timeout detection and threshold configuration
- Clock gating support
- Data line as enable signal support

### TX Unit Features
- Data length configuration
- Clock gating support
- Valid signal generation and delay control (on some chips)
- Idle data value configuration
- DMA EOF condition support (on some chips)

### Interrupt and Event Handling
- TX FIFO empty event
- RX FIFO full event
- TX EOF (End of Frame) event
- Interrupt enable/disable and status management

### Power Management
- Sleep retention support (on some chips)
- Register state preservation during sleep

## Usage

The HAL functions primarily serve ESP-IDF peripheral drivers such as `esp_driver_parlio`.

Advanced developers can use these interfaces directly when implementing custom drivers, with the understanding that API stability is not guaranteed.

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
