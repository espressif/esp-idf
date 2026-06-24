# ESP Hardware Abstraction Layer for Analog I2C Master (REGI2C)

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_regi2c` component provides the **Hardware Abstraction Layer** for the analog I2C master peripheral (commonly referred to as REGI2C) across ESP-IDF supported targets. This peripheral is used to access internal analog registers for calibration, PLL configuration, and other low-level analog operations.

## Architecture

The REGI2C HAL is organized in two layers:

1. **Implementation Layer (Upper)**: Per-target implementation sources (`regi2c_impl.c`) provides the interface to read and write to the analog i2c **device** registers.
2. **Low-Level Layer (Bottom)**: Per-target `regi2c_ctrl_ll.h` contains functions that configure analog I2C power, clock gating, reset, bus access.

## Usage

This HAL is consumed by ESP-IDF internal components to perform analog register access for calibration and configuration.

Developers may interact with the HAL directly: `regi2c_impl_write`, `regi2c_impl_write_mask`, `regi2c_impl_read`, and `regi2c_impl_read_mask` are supposed to be the only interfaces to do analog i2c register read and write operations. Atomicity and analog i2c clock needs to be taken care separately.

Or `esp_hw_support/regi2c_ctrl.h` provides fully wrapped macros `REGI2C_WRITE`, `REGI2C_WRITE_MASK`, `REGI2C_READ`, and `REGI2C_READ_MASK` that can be used directly.

API stability is not guaranteed during beta.

## Dependencies

- `soc`: SoC register definitions and analog I2C master constants
- `hal`: Common HAL utilities and macros

