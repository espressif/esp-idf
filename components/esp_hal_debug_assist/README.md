# ESP Hardware Abstraction Layer for Debug Assist Peripherals

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_debug_assist` component provides a **Hardware Abstraction Layer** for various debug and hardware-assisted monitoring peripherals found across ESP targets. It collects low-level register access code and HAL-level sequencing into a single reusable component, making it straightforward for bare-metal users and porting efforts to leverage these debugging features without depending on the full ESP-IDF driver stack.

## Submodules

### 1. assist_debug — Stack Spill & Bus Monitor

The assist_debug (a.k.a. bus monitor) peripheral monitors CPU stack pointer usage and reports stack overflow/underflow conditions.

Key capabilities:

- SP upper/lower bound monitoring with interrupt on overflow
- PC recording on SP overflow (supported targets)
- Debug module active detection (`assist_debug_ll_is_debugger_active`)
- CPU lockup capture with exception cause, tval, and iaddr trace
- Lockup-triggered hardware reset via LP_CLKRST

### 2. debug_probe — Signal Probing (Logic Analyzer)

The debug probe peripheral routes internal digital signals to GPIO pads for real-time observation with a logic analyzer or oscilloscope.

Key capabilities:

- Two independent probe units: HP (high-performance) and LP (low-power)
- Two channels per unit, each routing 32 bits of internal signals
- Configurable signal group selection per byte lane
- 16-bit or 32-bit output to GPIO pads

### 3. riscv_trace — RISC-V Trace Encoder

The RISC-V trace encoder captures instruction trace packets to a reserved memory region.

Key capabilities:

- Programmable memory region (start/end address, wrap or stop modes)
- Configurable trace options: full/delta address, stall-on-full, halt/reset behavior
- Filter unit with dual comparators (address/value match, range, privilege level filtering)
- Interrupt on FIFO overflow or memory-full condition
- Configurable AHB burst and resynchronization parameters

### 4. xtensa_trace_ll — Xtensa Trace Memory Management

Low-level helpers for Xtensa trace memory management.

## Architecture

Each submodule follows the same two-layer design:

1. **HAL Layer** (`include/hal/*_hal.h`, `*_hal.c`): Defines initialization sequences, configuration structures, and operational flow. Not all submodules have a .c file — simple ones are entirely inline.

2. **Low-Level Layer** (`<target>/include/hal/*_ll.h`): Chip-specific register access. One implementation per target that has the peripheral.

File inclusion follows the pattern `<target>/include/hal/` — the build system automatically picks the right LL header for the selected target.

## Dependencies

- `soc`: Chip-specific register definitions and structs
- `hal`: Core hardware abstraction utilities (`hal/assert.h`, `hal/misc.h`)
- `esp_common`: Attribute macros and bit definitions (`esp_attr.h`, `esp_bit_defs.h`)
- `esp_rom` (priv): ROM delay functions used by `riscv_trace_hal.c`
