| Supported Targets | ESP32 |
| ----------------- | ----- |

# PSA ITS Custom Storage Backend

## Overview

This example demonstrates the PSA ITS (Internal Trusted Storage) custom storage backend feature, which lets the application route persistent PSA Crypto keys to a user-defined storage implementation based on the key ID range.

By default, all persistent PSA keys are stored in NVS (Non-Volatile Storage) under the framework-owned `psa_its` namespace. With the custom storage backend enabled, a configurable range of key IDs is routed to user-registered callbacks where the application controls where and how those keys are stored.

This example registers a single custom backend that handles one key ID range:

| Key ID Range | Storage Backend | Description |
|---|---|---|
| `0x00000001` - `0x2FFFFFFF` | Default NVS (`psa_its`) | Handled by the framework, no custom code needed |
| `0x30000000` - `0x3FFFFFFF` | Custom NVS namespace (`psa_its_ext`) | Implemented in this example, stores the PSA blob verbatim |

Two persistent AES-256 keys are provisioned — one per storage path — and used for AES-CBC encrypt/decrypt to verify correct operation.

### How the custom backend stores keys

PSA Crypto hands the backend a fully-formatted PSA persistent key blob in `set()`. This example persists that blob verbatim and returns it unchanged on `get()`. The backend treats the blob as opaque bytes — no parsing, no attribute knowledge, no commitment to a particular key shape. This is the simplest possible backend; the PSA layer owns the format and the backend is purely a byte store.

Backends that need to optimise on-disk size, expose pre-provisioned hardware keys, or synthesise blobs on read can call `esp_psa_key_file_unpack()` / `esp_psa_key_file_pack()` (declared in `esp_psa_key_file.h`) to convert between PSA blobs and (attributes + key material). See the "Adding a New Storage Backend" section below for the trade-offs and an example.

> Note: `PSA_STORAGE_FLAG_WRITE_ONCE` is intentionally not tracked in this minimal example.

## How to use the example

### Hardware Required

This example can be executed on any ESP32 development board.

### Configure the project

Set the correct chip target:

```
idf.py set-target <chip_name>
```

The default configuration in `sdkconfig.defaults` enables the custom storage backend with the key ID range `0x30000000` - `0x3FFFFFFF`. These can be adjusted via `idf.py menuconfig`:

- `Component Config` > `mbedTLS` > `Enable custom storage backend for PSA ITS`
- `Component Config` > `mbedTLS` > `Minimum UID for custom backend range`
- `Component Config` > `mbedTLS` > `Maximum UID for custom backend range`

### Build and Flash

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

### Example Output

First boot (keys generated):

```log
I (286) example: === PSA ITS Custom Storage Backend ===
I (306) example: Custom ITS backend registered
I (306) example: --- Provisioning keys ---
I (416) example: [NVS] Generated persistent key 0x1
I (436) example: [CUSTOM] Generated persistent key 0x30000001
I (436) example: --- Testing encryption/decryption ---
I (446) example: [NVS] Encrypt/decrypt OK with key 0x1
I (456) example: [CUSTOM] Encrypt/decrypt OK with key 0x30000001
I (456) example: === All tests passed ===
```

Subsequent boots (keys loaded from storage):

```log
I (286) example: === PSA ITS Custom Storage Backend ===
I (306) example: Custom ITS backend registered
I (306) example: --- Provisioning keys ---
I (306) example: [NVS] Key 0x1 already exists
I (316) example: [CUSTOM] Key 0x30000001 already exists
I (316) example: --- Testing encryption/decryption ---
I (326) example: [NVS] Encrypt/decrypt OK with key 0x1
I (336) example: [CUSTOM] Encrypt/decrypt OK with key 0x30000001
I (336) example: === All tests passed ===
```

## Project Structure

```
main/
  app_main.c                 - Application entry point, registers the custom backend
  custom_nvs_its_backend.c/h - Custom NVS namespace storage implementation
partitions_example.csv       - Partition table
```

## Adding a New Storage Backend

To replace or extend the custom backend with another storage (e.g., SPIFFS, a secure element, a filesystem on an external chip):

1. Implement the four storage primitives (`set`, `get`, `get_info`, `remove`) so their signatures match `esp_psa_its_custom_ops_t`.
2. Bind them in `app_main.c` when filling the `esp_psa_its_custom_ops_t` instance.
3. Pass that instance to `esp_psa_its_register_custom_backend()`.

The framework imposes no on-disk format on custom backends. A few common patterns:

- **Store the PSA blob verbatim** (used by this example) — minimal code, 36 bytes of PSA wrapper per entry plus the key material. Suitable when storage size isn't a concern and the backend should be agnostic to key shape.
- **Unpack-store-repack with hardcoded attributes** — call `esp_psa_key_file_unpack()` in `set()` to extract just the inner key bytes, persist only those, then call `esp_psa_key_file_pack()` in `get()` to rebuild the PSA blob using attributes the backend knows in advance. Smallest on-disk footprint; the backend commits to a single key shape. The `esp_secure_cert_mgr` custom-backend example demonstrates this pattern with a pre-provisioned RSA-DS key synthesised from an eFuse-bound peripheral.
- **Unpack-store-repack with persisted attributes** — same as above but the backend also stores the attributes (e.g., as a small trailer or sidecar entry). Larger on-disk footprint than hardcoded, but the backend can serve arbitrary key shapes.

If you want to dispatch a single registered custom backend across multiple internal storage targets, do the routing inside your `set`/`get`/`get_info`/`remove` implementations (e.g., switch on `uid` sub-ranges) — the framework only sees one set of callbacks.
