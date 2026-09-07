| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Introduction

The `coredump` test app contains the coredump-enabled panic coverage that was split out of the `panic_base` app so CI can route it separately with `depends_components`.

The C test logic is shared with the base panic app through [common/main](../common/main). Pytest wrappers live in [pytest_panic_coredump.py](pytest_panic_coredump.py) and reuse the panic test bodies for scenarios that should run with both panic and coredump configurations.

For guidance on where to add new tests, see [panic_base/README.md](../panic_base/README.md#where-to-add-a-new-test).

# Building

For example, to build the coredump panic app with configuration `coredump_flash_default` for ESP32-C3, run:

```bash
idf.py -DIDF_TARGET=esp32c3 -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.coredump_flash_default" build
```
