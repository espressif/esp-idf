# PSRAM MSPI Interrupt Handling

## Overview

This file (`system_layer/esp_psram_mspi.c`) handles PSRAM-specific MSPI interrupt registration.

| Chip Type    | `MSPI_LL_INTR_SHARED` | Behavior                                             |
|--------------|-----------------------|------------------------------------------------------|
| Shared IRQ   | 1                     | Register to shared MSPI dispatcher                   |
| Separate IRQ | 0                     | Register standalone PSRAM ISR via `esp_intr_alloc()` |

## Documentation

For detailed architecture, flow diagrams, and API reference, see:

**[esp_hw_support/README.md - MSPI Interrupt Logic](../esp_hw_support/README.md#mspi-interrupt-logic---chip-differences-analysis)**
