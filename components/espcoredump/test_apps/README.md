| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP Core Dump Tests

This test app is used to provide built binaries for the test cases under test folders

## Update coredump.64 test data

To update `test/<target>/coredump.64` build a `test_apps` for a target, flash and get a base64 text from `idf.py monitor`

## Update expected_output test data

To update `test/<target>/expected_output` run

```
TARGET=esp32
espcoredump.py --chip $TARGET info_corefile -c ../test/$TARGET/coredump.b64 -t b64 -m ./build/test_core_dump.elf > ../test/$TARGET/expected_output
```

A `test_apps` app should be built for that target.

Do the same for other targets: esp32s2, esp32c3, etc.

## Update ELF test binaries

The ELF test binaries are placed in a different git repository, _idf-coredump-elf_, to avoid putting big binaries in IDF repo.
It is used in _test_espcoredump_ CI job.

See _idf-coredump-elf/README.md_ to use `build_espcoredump.sh` and generate test ELF binaries
