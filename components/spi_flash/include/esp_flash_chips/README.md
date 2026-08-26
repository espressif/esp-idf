# ESP Flash Chips Directory

## Overview

This directory contains header files for implementing custom SPI flash chip drivers. These headers are **semi-public** - they are intended for expert users who need to implement custom chip drivers for unsupported flash chips, but they are **not considered stable API** and may change without notice.

## Intended Use

These headers are provided for:

- **Expert users** who need to implement custom chip drivers for flash chips not supported by ESP-IDF
- **Advanced developers** who need to extend or modify existing chip driver behavior
- **Internal ESP-IDF development** for implementing official chip drivers

## Important Warnings

⚠️ **These headers are NOT stable API**

- The structure and content of these headers may change in any ESP-IDF release
- No backward compatibility is guaranteed
- These headers are not part of the public API contract
- Changes may break custom chip driver implementations without notice

⚠️ **Use at your own risk**

- Custom chip drivers using these headers may break when upgrading ESP-IDF
- Thorough testing is required after each ESP-IDF version upgrade
- Consider contributing your chip driver to ESP-IDF or [3rd party flash drivers repo](https://github.com/espressif/esp-flash-drivers) instead of maintaining it separately

## Directory Contents

- `spi_flash_chip_driver.h` - Core chip driver interface definitions
- `esp_flash_types.h` - Internal flash type definitions
- `spi_flash_chip_*.h` - Header files for specific chip driver implementations that you may reuse (especially the generic driver)
- `spi_flash_override.h` - Flash override configuration structures for those parts outside the scope of esp_flash driver, usually used in the startup code

## When to Use These Headers

✅ **Appropriate use cases:**

- Implementing a driver for a new flash chip model
- Extending functionality of an existing chip driver
- Debugging chip-specific flash issues

See [example](examples/storage/custom_flash_driver) on how to implement the driver for a custom flash chip.

❌ **Not appropriate for:**

- Regular application development (use public APIs in `esp_flash.h` instead)
- Production code that needs stable API guarantees
