# Integration Test Description

## Case Lists
- WiFi Standard cases for only esp32.
- BLE Standard cases for esp32 and esp32c3.

## Trigger
- By bot:
  - `@bot test with label: integration_test`

## Bot Filter
- A case filter could be used in `@bot` message:
- For wifi cases:
  - `@bot test with label: integration_test; with filter module: WIFI MAC, TCPIP, Mesh, System`
- For bt/ble cases:
  - `@bot test with label: integration_test; with filter module: BT Stack, BLUEDROID, BT Profile, NIMBLE`
- use `@bot help` for more details.
