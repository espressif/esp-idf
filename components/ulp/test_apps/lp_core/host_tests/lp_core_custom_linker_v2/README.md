| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- |

# LP Core Custom Linker Host Tests (CMake v2 full subproject)

Build-only host tests for the ULP custom linker feature under the **CMake v2** build system. The parent app is v2-native (its `CMakeLists.txt` includes `tools/cmakev2/idf.cmake`) and the ULP program is built as a full subproject via `ulp_project_default()`, so these tests exercise the buildv2 linker-script path (`components/ulp/CMakeLists_v2.txt`) rather than the CMake v1 one covered by `../lp_core_custom_linker`.

No target board is required: every check is resolved at build time on the host.

`test_lp_core_custom_linker_v2.py` builds this app with layouts supplied via the `LINKER_LAYOUT` option of `ulp_add_project` and asserts on the linked ULP image — that a custom layout is applied, and that switching the layout on an incremental build relinks against the new one.

Being a build-time test, it drives `idf.py` itself and is run as plain pytest by the dedicated `test_lp_core_custom_linker_buildv2_on_host` host-test job, outside the idf-ci flow (see `exclude_dirs` in `.idf_ci.toml`). Run it locally with `pytest_for_ut .` from this directory, or directly with `pytest -p no:idf-ci -p no:pytest_embedded .`.
