# ESP Hardware Abstraction Layer for SPI Peripheral

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_spi` component provides a **Hardware Abstraction Layer** for the General Purpose SPI (GPSPI) peripherals across all ESP-IDF supported targets. It serves as a foundation for the higher-level SPI drivers, offering a consistent interface to interact with SPI hardware while hiding the complexities of chip-specific implementations.

## Architecture

The HAL architecture consists of two primary layers:

1. **HAL Layer (Upper)**: Defines the operational sequences and data structures required to interact with SPI peripherals, including:
   - Initialization and de-initialization
   - Clock configuration and timing calculations
   - Device and transaction setup
   - Master, slave, and slave HD (Half Duplex) mode operations

2. **Low-Level Layer (Bottom)**: Acts as a translation layer between the HAL and the register definitions in the `soc` component, handling:
   - Register access abstractions
   - Chip-specific register configurations
   - Hardware feature compatibility

## Features

- Unified SPI interface across all ESP chip families
- Support for multiple operation modes:
  - **Master mode**: Full-duplex and half-duplex communication
  - **Slave mode**: Standard slave operation
  - **Slave HD mode**: Half Duplex slave mode with segment-based transactions (on supported chips)
- Flexible SPI line configurations (1/2/4-line modes)
- Configurable clock sources and frequency settings
- Support for various transaction formats (command, address, dummy, data phases)

## Usage

This component is primarily used by ESP-IDF peripheral drivers such as `esp_driver_spi`. It provides the low-level hardware abstraction needed for SPI communication with external devices.

For advanced developers implementing custom SPI solutions, the HAL functions can be used directly. However, please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.

### Typical Usage Flow

**Master Mode (without DMA):**
1. Initialize the SPI bus
2. Setup clock speed configuration
3. Call `setup_device` to update parameters for a specific device
4. Call `setup_trans` to update parameters for a specific transaction
5. Prepare data to send into hardware registers
6. Trigger the SPI transaction to start
7. Wait until the transaction is complete
8. Fetch the received data

**Slave Mode (without DMA):**
1. Initialize the SPI bus with `spi_slave_hal_init`
2. Configure device parameters (mode, bit order, etc.) in the HAL context
3. Call `spi_slave_hal_setup_device` to update parameters for the device
4. Prepare data to send and receiving buffer
5. Call `spi_slave_hal_set_trans_bitlen` to set transaction bit length
6. Call `spi_slave_hal_user_start` to trigger the SPI transaction to start
7. Wait until the transaction is done with `spi_slave_hal_usr_is_done`
8. Call `spi_slave_hal_store_result` to store the received data
9. Call `spi_slave_hal_get_rcv_bitlen` to get the received data length

## Dependencies

- `soc`: Provides chip-specific register definitions
- `hal`: Core hardware abstraction utilities and macros
- `esp_hal_gpio`: Required for ESP32 to access GPIO matrix delay information
