| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# TWAI Bus-Off Recovery Example
This example demonstrates how to recover a TWAI node from a Bus-Off error condition and resume communication. The recovery is triggered by physically inducing bus errors and handled using the ESP TWAI on-chip driver with callback support.

## How It Works
1. Initialize and start the TWAI node using esp_twai_onchip.
2. Transmit a few TWAI frames in normal operation.
3. **Manually** trigger bit errors by either:
   - Briefly disconnecting the TX or RX connection
   - Briefly short-circuiting the TWAI H and TWAI L lines
4. When the node enters the Bus-Off state, an error recovery process will automatically initiated.
5. Once recovered, resume normal transmission.

## Hardware Requirements
- An ESP32xx chip with TWAI peripheral support
- External TWAI transceiver (e.g., TJA1050, MCP2551, or similar)
- TWAI bus setup with proper termination resistors

## Pin Configuration
Modify TX_GPIO_NUM and RX_GPIO_NUM in the `twai_recovery_main.c` if needed. Connect these pins to your TWAI transceiver's TX and RX pins respectively.
```c
#define TX_GPIO_NUM         GPIO_NUM_4
#define RX_GPIO_NUM         GPIO_NUM_5
```

## Manual Error Triggering
To trigger bus errors and test the recovery mechanism, you can use either of these methods:

1. **Connection Disruption**: Briefly disconnect the TX or RX wire between the ESP32 and the TWAI transceiver while frames are being transmitted.

2. **Bus Short Circuit**: Briefly short-circuit the TWAI H and TWAI L lines on the bus. This simulates a bus fault condition.

⚠️ **Important**: The disconnection or short-circuit should be brief enough to trigger bus error messages in the log output. Too long may cause permanent damage to the transceiver or affect other nodes on the bus.

## Build and Flash
```sh
idf.py set-target esp32 build
idf.py -p PORT flash monitor
```

## Example Output
```
install twai success
node started

sending frame 0, please trigger error during sending
sending frame 1, please trigger error during sending

...
sending frame 4, please trigger error during sending
// Manually trigger error here (disconnect TX/RX or short H/L)
bus error: 0x2
state changed: error_active -> error_warning

...
sending frame 9, please trigger error during sending
// Trigger error again
bus error: 0x2
state changed: error_passive -> bus_off

node offline, start recover ...
waiting ... 0
...
state changed: bus_off -> error_active
node recovered! continue

sending frame 0, please trigger error during sending
```

## Troubleshooting

If the example doesn't enter the `error_warning`/`error_passive`/`bus_off` states after manual error triggering:

1. **Check Hardware Connections**: Ensure the TWAI transceiver is properly connected and powered.
2. **Verify Bus Termination**: Make sure the TWAI bus has proper 120Ω termination resistors at both ends.
3. **Timing of Error Injection**: The manual error should be triggered while frames are actively being transmitted.
4. **Transceiver Type**: Different TWAI transceivers may have different fault detection sensitivities.

If errors are not being detected:
- Try holding the disconnection/short-circuit for a slightly longer duration
- Ensure the error is introduced during active frame transmission
- Check that the TWAI H and TWAI L lines are properly connected to the transceiver
