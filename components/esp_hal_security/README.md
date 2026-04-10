# `esp_hal_security` (G1 component)

The `esp_hal_security` component provides a **Hardware Abstraction Layer** for security-related peripherals across all targets supported by ESP-IDF.

This component contains HAL implementations for the following security peripherals:

- **AES** (Advanced Encryption Standard)
- **SHA** (Secure Hash Algorithm)
- **HMAC** (Hash-based Message Authentication Code)
- **MPI** (Modular Polynomial Integer - RSA operations)
- **ECC** (Elliptic Curve Cryptography)
- **ECDSA** (Elliptic Curve Digital Signature Algorithm)
- **DS** (Digital Signature)
- **Key Manager** (Hardware key management)
- **HUK** (Hardware Unique Key)
- **APM** (Access Permission Manager)
- **MPU** (Memory Protection Unit)

## Structure

Similar to the main `hal` component, this component follows the same structure:

- **HAL layer** (`include/esp_hal_security/<periph>_hal.h`): High-level abstraction for peripheral operations
- **LL layer** (`<target>/include/esp_hal_security/<periph>_ll.h`): Low-level register access functions
- **Types** (`include/esp_hal_security/<periph>_types.h`): Type definitions and constants shared across layers

## Usage

This component is automatically included when you depend on components that use security peripherals, such as:
- `esp_security`
- `mbedtls`
- `bootloader_support` (for secure boot and flash encryption)

You typically don't need to explicitly add this component to your `CMakeLists.txt` unless you're directly using security HAL APIs.

## Note

This component was split from the main `hal` component to better organize security-related functionality and manage dependencies. Components that previously depended on `hal` for security features should now depend on `esp_hal_security` instead.

