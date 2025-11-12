# ESP Hardware Abstraction Layer for Ethernet MAC Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_emac` component provides a **Hardware Abstraction Layer** for the Ethernet MAC (EMAC) peripherals across all ESP-IDF supported targets. It serves as a foundation for the higher-level Ethernet drivers, offering a consistent interface to interact with Ethernet MAC hardware while hiding the complexities of chip-specific implementations.

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with EMAC peripherals, including:
   - Initialization and de-initialization
   - MAC configuration (speed, duplex, promiscuous mode)
   - DMA descriptor management
   - PHY interface management (MII/RMII)
   - Address filtering and frame reception/transmission
   - Flow control and interrupt handling

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility

## Features

- Unified Ethernet MAC interface across all ESP chip families
- Support for MII and RMII PHY interfaces
- DMA-based frame transmission and reception
- MAC address filtering with multiple address support
- Flow control (pause frame) support
- Promiscuous mode and multicast filtering
- Hardware checksum offloading
- IEEE 1588 PTP (Precision Time Protocol) support on compatible chips
- Sleep retention support on compatible chips
- Configurable DMA burst length
- VLAN tag insertion/removal/replacement
- IP checksum insertion for TCP/UDP/ICMP headers

## Usage

This component is primarily used by ESP-IDF peripheral drivers such as `esp_eth`. It is also utilized by system components that require Ethernet functionality.

For advanced developers implementing custom Ethernet solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides chip-specific register definitions and peripheral information
- `hal`: Core hardware abstraction utilities and macros

