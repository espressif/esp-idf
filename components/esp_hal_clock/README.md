# ESP Hardware Abstraction Layer for Clock Tree

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_clock` component provides the **Hardware Abstraction Layer** for the SoC clock tree across ESP-IDF supported targets. It exposes helpers to query and configure clock sources, select roots, and manage clock gates in a target-agnostic way while delegating register details to per-target Low-Level (LL) implementations.

## Architecture

The Clock HAL is organized in two layers:

1. **HAL Layer (Upper)**: Target-independent logic and APIs to interact with the clock tree (e.g., querying frequencies, switching roots, enabling/disabling gates).
2. **Low-Level Layer (Bottom)**: Per-target register accessors defined by:
   - `clk_tree_ll.h` — clock source/root select, muxes, dividers, and frequency helpers.
   - `clk_gate_ll.h` — clock gate enable/disable, reset, and configuration of default clock gate status for peripherals.
   - `clkout_channel.h` — clock output channel IDs and related helpers used to route internal clocks to GPIOs.

Per-target HAL sources implement SoC-specific behavior in `clk_tree_hal.c`, using the LL accessors above.

## Features

- Query effective frequencies of common clock domains.
- Select and switch clock roots (when supported).
- Configure dividers/multipliers per domain (target-dependent).
- Gate/ungate peripheral clocks via unified helpers.
- Apply default peripheral clock gate configuration during early boot (target-dependent).
- Configure and map on-chip clocks to GPIOs via clock-out channels (`clkout_channel.h`) on supported targets.

## Usage

This HAL is consumed by ESP-IDF internal components (e.g., `esp_hw_support`, drivers, bootloader code) to perform clock configuration and queries.

Advanced users may interact with the HAL directly when implementing custom bring-up or performance-sensitive flows. API stability is not guaranteed during beta.

## Dependencies

- `soc`: SoC register definitions and clock tree constants
- `hal`: Common HAL utilities and macros


