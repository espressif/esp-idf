# ESP Hardware Abstraction Layer for PMU and RTC Control Peripherals

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_pmu` component provides a **Hardware Abstraction Layer** for Power Management Unit (PMU) and legacy RTC Control (RTC_CNTL) peripherals across ESP-IDF supported targets. This HAL enables unified power management operations including sleep mode control, power domain management, wakeup configuration, and retention control across different ESP chip families.

The component consolidates power management functionality from three different peripheral implementations:
- **PMU**: Modern power management unit in newer ESP chips
- **RTC_CNTL**: Legacy RTC control peripheral in older ESP chips
- **PAU**: Power Assist Unit for register retention and backup/restore operations

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with PMU/RTC_CNTL/PAU peripherals, including:
   - Power domain configuration and control
   - Sleep mode management
   - Wakeup source configuration
   - Retention control (CPU, tagmem)
   - Power-up/down wait cycle configuration
   - Backup enable/disable operations
   - REGDMA link configuration and control

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility
   - Power domain and clock control

## Features

### Power Management
- Power domain control (HP/LP system power domains)
- Digital power supply and power-up wait cycle configuration
- Control ready wait cycle configuration (isolate/reset)
- Power mode transitions (active, sleep, modem)

### Sleep and Wakeup
- Sleep mode configuration
- Wakeup source management
- Sleep rejection handling
- Wakeup cause detection

### Retention Control
- CPU retention enable/disable
- Tagmem retention support (on supported chips)
- DMA link buffer configuration for retention

### Backup Operations
- Sleep-to-active backup enable/disable
- Sleep-to-modem backup enable/disable
- Modem-to-active backup enable/disable

### REGDMA (Register DMA) Support
- REGDMA entry link address configuration
- REGDMA modem link control (backup/restore)
- REGDMA system link control
- REGDMA extra link control
- REGDMA timeout configuration
- REGDMA clock configuration

## Usage
This component is primarily used by ESP-IDF system services such as:
- **esp_pm**: Power management framework
- **esp_hw_support**: Hardware support layer for sleep modes, power management, and register retention
- **ulp**: Ultra-low-power coprocessor support

For advanced developers implementing custom power management solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

## Dependencies

- `soc`: Provides chip-specific register definitions and peripheral capabilities
- `hal`: Core hardware abstraction utilities and macros
- `esp_rom`: ROM function interfaces (where applicable)
