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
| `0x30000000` - `0x3FFFFFFF` | Custom NVS namespace (`psa_its_ext`) | Implemented in this example, stores only the raw key bytes — no PSA wrapper, no attributes, no metadata |

Two persistent AES-256 keys are provisioned — one per storage path — and used for AES-CBC encrypt/decrypt to verify correct operation.

### How the custom backend stores keys

PSA Crypto hands the backend a fully-formatted PSA persistent key blob in `set()`. This example unpacks the blob with the upstream helper `psa_parse_key_data_from_storage()` to extract just the inner key bytes and persists *only those bytes* — no PSA wrapper, no attribute trailer, no metadata at all. On `get()`, the backend reads the key bytes back, synthesises a `psa_key_attributes_t` from hardcoded values, and rebuilds the PSA blob with `psa_format_key_data_for_storage()` before returning it to PSA Crypto.

The two upstream helpers (`psa_parse_key_data_from_storage` / `psa_format_key_data_for_storage`) are the canonical way for custom backends to manipulate the PSA blob format without re-implementing it.

This is the minimum on-disk footprint possible — but the trade-off is that the backend commits to a fixed key shape. The attributes it reports on read must match what the application uses when calling `psa_generate_key()` / `psa_import_key()` for keys in the custom UID range. In this example that's AES-256 / CBC / encrypt+decrypt, encoded as a few `#define`s in `custom_nvs_its_backend.c`. A backend that needs to serve heterogeneous keys would persist the attributes too — see the comment block at the top of the backend source for pointers.

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

- **Store the PSA blob verbatim** — minimal code, ~32 bytes of PSA wrapper per entry. Suitable when storage size isn't a concern and the backend should be agnostic to key shape.
- **Unpack-store-repack with hardcoded attributes** (used by this example) — call `psa_parse_key_data_from_storage()` in `set()` to extract just the inner key bytes, persist only those, then call `psa_format_key_data_for_storage()` in `get()` to rebuild the PSA blob using attributes the backend knows in advance. Smallest on-disk footprint; the backend commits to a single key shape.
- **Unpack-store-repack with persisted attributes** — same as above but the backend also stores the attributes (e.g., as a small trailer or sidecar entry). Larger on-disk footprint than hardcoded, but the backend can serve arbitrary key shapes.

If you want to dispatch a single registered custom backend across multiple internal storage targets, do the routing inside your `set`/`get`/`get_info`/`remove` implementations (e.g., switch on `uid` sub-ranges) — the framework only sees one set of callbacks.
