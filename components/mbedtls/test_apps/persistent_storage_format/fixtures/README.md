# Persistent storage format fixtures

A single committed NVS partition image, pre-populated with three persistent eFuse PSA keys — one per driver. Every CI runner flashes the same `.bin`; each runner's consume test only references its driver's key id (RSA-DS / HMAC / ECDSA) and ignores the other two.

Cross-platform format drift is caught by every runner — three independent signals on the same regression.

## File

| File | Storage version | Contains |
|------|-----------------|----------|
| `nvs_efuse_v1.bin` | v1 | 3 persistent eFuse keys: RSA-DS (id `0x1ADA1`), HMAC (id `0x1ADA2`), ECDSA SECP256R1 (id `0x1ADA3`) |

The key ids and the eFuse block/key-id assignments are declared in `../main/test_persistent_format.h`. They align with the existing volatile tests in `mbedtls_ut`, so each runner's already-burned eFuse key serves the persistent path too.

KM-source fixtures are intentionally absent — the per-key `esp_key_mgr_key_recovery_info_t` blob is HUK-wrapped by the deploying chip and meaningless on any other device. KM persistence is verified by the runtime deploy-then-import tests in `mbedtls_ut`.

## Regenerating

You need any chip with all three drivers in the build (typically ESP32-C5 or ESP32-P4 with DS + HMAC + ECDSA enabled).

The capture build needs all three drivers compiled in, which neither `sdkconfig.ci.hmac` nor `sdkconfig.ci.ecdsa` provides on its own. Create your own local overlay file (any path; the example below uses `sdkconfig.capture`) with the following contents, then point `SDKCONFIG_DEFAULTS` at it. Do NOT name it `sdkconfig.ci.*` — that prefix is auto-discovered by the CI app manifest and would add a build target the runners can't use.

```
CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL=y
CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN=y
CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY=y
```

```
cd components/mbedtls/test_apps/persistent_storage_format

# 1. Erase NVS so the capture starts deterministic
esptool --chip <CHIP> --port <PORT> erase_region --force <NVS_OFFSET> <NVS_SIZE>

# 2. Build, flash, and run ONLY the capture test
idf.py set-target <CHIP>
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.capture" reconfigure
idf.py -p <PORT> flash
idf.py -p <PORT> monitor             # send `[fixture_capture]` to Unity, wait for PASS

# 3. Read the partition out
esptool --chip <CHIP> --port <PORT> read_flash <NVS_OFFSET> <NVS_SIZE> fixtures/nvs_efuse_v1.bin

# 4. Commit. If you bumped any storage version, rename to nvs_efuse_v2.bin
#    and keep nvs_efuse_v1.bin around — the v1 consume tests then prove
#    v2 firmware can still read v1 NVS blobs (backward compat).
```

NVS offset/size live in `partitions.csv` — currently `0xA000` / `0x6000` (24 KB).

## When to regenerate

- After bumping any driver's persistent storage struct version. Add a new fixture file (`v2`, `v3`, …) AND keep the older fixtures around with their consume tests, so older-on-disk -> newer-firmware compatibility is proven.
- After an mbedtls upgrade that changes PSA ITS framing. Rare; needs a release note.
- Never just "to refresh." The file is meant to stay frozen so it detects regressions.
