| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- |

# LP Core Custom Linker Host Tests

Build-only host tests for the ULP custom linker feature. No target board is required: every check is resolved at configure or link time on the build host.

`test_lp_core_custom_linker.py` builds this app with different layouts supplied via the `LINKER_LAYOUT` option and asserts on the outcome (both the positive paths and the layouts that must fail the build).

Being a build-time test, it drives `idf.py` itself and is run as plain pytest by the dedicated `test_lp_core_custom_linker_on_host` host-test job, outside the idf-ci flow (see `exclude_dirs` in `.idf_ci.toml`). Run it locally with `pytest_for_ut .` from this directory, or directly with `pytest -p no:idf-ci -p no:pytest_embedded .`.
