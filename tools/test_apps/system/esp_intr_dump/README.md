| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Test for esp_intr_dump

This test app serves two purposes:
1. Sanity-checking `esp_intr_dump` function. These tests run in QEMU and make sure that `esp_intr_dump` produces expected output when e.g. a shared interrupt is allocated.
2. Making unintended changes to the default interrupt allocations more visible in MRs. The way this works is, the output of `esp_intr_dump` is compared to the expected output, for example [expected_output/esp32.txt](expected_output/esp32.txt). If you change IDF startup code so that it allocates an additional interrupt, you will need to update the expected output file. MR reviewers will see the modification of the expected output file and will evaluate the impact of the change.

## When adding support for a new chip target

1. Build the test app for the new target, flash it to the board.
2. Enter `intr_dump` command in the console.
3. Copy the output and save it in `expected_output/<target>.txt`.
