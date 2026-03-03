# ESP Hardware Abstraction Layer for USB Peripheral(s)

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_usb` component provides a **Hardware Abstraction Layer** for USB controllers and PHYs across all ESP-IDF supported targets. USB (Universal Serial Bus) enables communication between the ESP device and USB hosts or devices, supporting various USB speeds and transfer types for data exchange.

## Architecture

The USB HAL is structured in two main sub-layers:

1. **HAL Layer (Upper)**: Defines the operational steps and data structures required to control USB peripherals (e.g., initialization, endpoint configuration, transfer management, PHY control).

2. **Low-Level Layer (Bottom)**: Serves as a translation layer between the HAL and the register files defined in the `soc` component, handling target-specific register configurations.

## Supported USB Controllers and PHYs

This HAL supports various USB controller and PHY types depending on the ESP chip:

- **USB-DWC (DesignWare USB Controller)**: The main USB OTG controller supporting USB Host and Device modes

- **USB Serial JTAG**: A special USB peripheral that combines USB CDC-ACM functionality with JTAG debugging capabilities

- **USB WRAP**: A wrapper/peripheral controller that provides additional USB functionality and GPIO matrix integration

- **USB PHY Types**:
  - **FSLS (Full Speed/Low Speed) Internal PHY**: Built-in PHY supporting USB Full Speed and Low Speed operation
  - **FSLS External PHY**: Support for external FSLS PHY via GPIO matrix
  - **UTMI PHY**: UTMI+ compliant PHY interface for High Speed USB support

## Features

- USB controller initialization and configuration
- Endpoint management (IN/OUT endpoints)
- Transfer type support (Control, Bulk, Interrupt, Isochronous)
- USB speed detection and configuration (High Speed, Full Speed, Low Speed)
- FIFO configuration and management
- PHY initialization and control (FSLS and UTMI)
- GPIO matrix signal routing for USB PHY interfaces
- OTG (On-The-Go) signal handling
- Interrupt management and event handling
- Multi-controller support (on chips with multiple USB controllers)

## Usage

The HAL functions primarily serve ESP-IDF USB drivers, primarily the [USB Host stack](https://components.espressif.com/components/espressif/usb).

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros

## Hardware configuration

The USB-DWC2 peripheral has many internal configuration parameters. The configuration is saved `GHWCFGx` registers. Configuration parameters which are often checked in run-time, are cached in USB-HAL context variable `usb_dwc_hal_context_t::constant_config` to limit access to the registers and offload system bus.

### Selected configuration parameters across ESP SoCs

|                    | S2, S3, H4 & P4-OTG1.1 | P4-OTG2.0, version < 3  | P4-OTG2.0, version >= 3 |
|--------------------|------------------------|-------------------------|-------------------------|
| HS PHY interface   | None                   | UTMI+ 16bit             | UTMI+ 16bit             |
| Single point       | Yes                    | No (SPLIT supported)    | No (SPLIT supported)    |
| Number of channels | 8                      | 16                      | 16                      |
| FIFO depth         | 256 lines (1024 bytes) | 1024 lines (4096 bytes) | 1024 lines (4096 bytes) |
| Perio queue depth  | 8                      | 4                       | 16                      |
| NPerio queue depth | 4                      | 4                       | 8                       |
| Transfer cnt width | 16bits (max 65,535)    | 16bits (max 65,535)     | 19bits (max 524,287)    |
| Packet cnt width   | 7bits (max 127)        | 7bits (max 127)         | 10bits (max 1,023)      |
