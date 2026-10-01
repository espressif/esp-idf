# ESP-TEE Code Hash

## `esp_tee_code_hash` tool

```
$ python esp_tee_code_hash.py --help

 Usage: esp_tee_code_hash.py [OPTIONS] IMAGE

 Compute the code hash of an ESP-TEE firmware image.

Options:
  -q, --quiet    print only the code hash
  --json         print the result as JSON
  --expect HEX   exit with status 1 unless the code hash equals HEX
  --header FILE  path to esp_tee_code_hash.h (default: from this checkout or IDF_PATH)
  -h, --help     Show this message and exit.
```

The code hash is the SHA-256 of the TEE image segments (headers and data) alone, without the app descriptor. It identifies the compiled code alone: the same TEE source, built reproducibly, yields the same code hash regardless of flash geometry, revision bounds or version labels. Image authenticity is the job of Secure Boot; the code hash is the reproducibility measure of a TEE build.

```bash
python esp_tee_code_hash.py build/esp_tee/esp_tee.bin
python esp_tee_code_hash.py -q build/esp_tee/esp_tee.bin                       # bare hash
python esp_tee_code_hash.py --expect <calc_code_hash> build/esp_tee/esp_tee.bin  # verify an attestation claim
python esp_tee_code_hash.py --json build/esp_tee/esp_tee.bin                   # for tooling
```

The tool verifies the image before hashing: the esptool XOR checksum and the appended SHA-256 must match, and the app descriptor must identify a TEE image. Damaged, truncated or non-TEE images are rejected.

> [!NOTE]
> The tool only hashes the image itself; the padding and Secure Boot V2 signature block that follow it are ignored. A signed `esp_tee.bin` and its `esp_tee-unsigned.bin` thus yield the same code hash, even though their plain file hashes differ (the signatures are salted).

## Excluded fields

The definition is versioned once in `components/esp_tee/include/esp_tee_code_hash.h` (`ESP_TEE_IMG_CODE_HASH_VER`) and parsed by this tool, so a device-side implementation can share it without drifting. Only the image segments are hashed; the parts below carry packaging metadata and are left out entirely.

| Image part | Field | Input behind it |
|---|---|---|
| Image header (`esp_image_header_t`) | `spi_mode`, `spi_speed` / `spi_size` | `ESPTOOLPY_FLASHMODE`, `ESPTOOLPY_FLASHFREQ` / `ESPTOOLPY_FLASHSIZE` |
| | `wp_pin`, `spi_pin_drv` | board flash wiring / pin drive |
| | `chip_id`, `entry_addr` | target, `_start` |
| | `min_chip_rev_full` / `max_chip_rev_full` | `ESP_REV_MIN_FULL` / `ESP_REV_MAX_FULL` |
| App descriptor (`esp_app_desc_t`, leading the first segment) | `secure_version` | `SECURE_TEE_SECURE_VERSION` |
| | `version` | `ESP_TEE_VERSION` (`version.cmake`) + IDF release line suffix |
| | `time` / `date` | always empty (reproducible build) |
| | `idf_ver` | ESP-IDF checkout state (tag / `git describe`) |
| | `app_elf_sha256` | the full ELF, including debug info |
| | `min/max_efuse_blk_rev_full`, `mmu_page_size` | `ESP_EFUSE_BLOCK_REV_*`, `MMU_PAGE_SIZE` |
| After the last segment | esptool XOR checksum byte (with its padding) | covers the app descriptor, i.e. every field above |
| Last 32 bytes | appended SHA-256 digest | derived from the image |

Deliberately **not** excluded: all segment headers (load addresses and sizes are memory layout, i.e. code identity) and all segment data past the app descriptor.

On the device, the segment positions come from the image metadata (`esp_image_get_metadata()`), i.e. the same walk that verifies the image digest.

> [!NOTE]
> An equal code hash does not imply deployment equivalence: the excluded chip/eFuse revision bounds are silicon-errata policy, and anything not listed above (e.g., a different `sdkconfig`) changes the code itself.
