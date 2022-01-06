# Integration Test Description

## Case Lists
- WiFi Standard cases for only esp32.
- BLE Standard cases for esp32 and esp32c3.

## Trigger
- By labels:
  - `integration_test`
- By file changes:
  - integration test related files
- By bot:
  - `@bot test with label: integration_test`

## Advanced
- There are labels can be used to run less integration test cases.
  - These labels only take effect when the integration test has been triggered.
  - label: `integration::wifi_only`
    - Only run WiFi cases.
  - label: `integration::ble_only`
    - Only run BLE cases.
