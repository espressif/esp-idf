### SDIO Channel Test Example

## Introduction

This example pair turns the SDIO host/slave examples into a channel test app
for lab work.

Use it when you want to:

- check whether a board pair can communicate over SDIO,
- compare 1-line and 4-line transfer modes,
- compare default-speed and high-speed clock settings,
- generate repeated traffic that is easy to capture with a scope or logic
  analyzer,
- validate that the received data still matches a known fixed pattern.

The `host` app provides an interactive serial menu:

1. Select 1-line or 4-line mode and select default-speed or high-speed clock.
2. Start link initialization.
3. Enter transmit mode or receive mode.
4. Run one transfer per second until `z` is pressed to return to the menu.

The `slave` app is a passive endpoint. Once initialized, it keeps enough
buffers ready so the host can repeatedly send or receive a fixed 16-byte test
frame without an extra handshake before each round.

The fixed test frame is:

`FF 00 FF 00 FF 00 FF 00 FF 00 FF 00 FF 00 FF 00`

Both sides validate received data against this exact sequence and print a
success or failure log for each completed transfer.

## Wiring

Connect the host and slave boards exactly as described in the SDIO programming
guide and the basic SDIO example for the selected targets.

For this signal-integrity test, connect all SDIO signal lines between the two
boards and always provide a solid common ground. Keep the wires short and avoid
loose return paths.

If link initialization fails, the host deinitializes the bus and returns to the
setup menu so the user can choose a different width/speed combination.

## Signal Integrity Notes

This example can exercise either a single active data lane or four active data
lanes. The waveforms below are conceptual. They show the fixed `FF 00`
payload pattern used by this example and intentionally omit the exact CRC bits.

### 1-line mode

```text
CLK	: _|-|_|-|_|-|_|-|_|-|_|-|_|-|_
CMD	: -----------------------------
DAT0	: _|--------________--------________...
DAT1	: -----------------------------
DAT2	: -----------------------------
DAT3	: -----------------------------
	  ^start	0xFF	0x00	0xFF	0x00
```

### 4-line mode

```text
CLK	: _|-|_|-|_|-|_|-|_|-|_|-|_|-|_
CMD	: -----------------------------
DAT0	: _|--__--__--__--__--__--__...
DAT1	: _|--__--__--__--__--__--__...
DAT2	: _|--__--__--__--__--__--__...
DAT3	: _|--__--__--__--__--__--__...
	  ^start	FF	00	FF	00	...
```

In 4-line mode, the repeated `FF 00` frame makes all four data lines switch
at the same time during the `0xF -> 0x0` and `0x0 -> 0xF` nibble
transitions. This is useful when checking simultaneous switching noise and
grounding quality.

To confirm the wiring before analyzing signal quality:

1. If CMD and CLK are correct, the host should complete the command-only
   initialization path and print `SDIO link initialized successfully`. In this
   example, both `sdmmc_card_init()` and `enable_slave_function()` still use
   only CMD-based transactions before the first data transfer starts.
2. If DAT0 is correct, 1-line mode should work. This example does have a real
   1-line-only data phase when you choose `1-line`: the first data-line
   transfer is the first CMD53 transaction after initialization succeeds.
3. If DAT1-DAT3 are connected but 4-line mode still fails, use a logic analyzer
   or oscilloscope to check whether the slave is responding on DAT1-DAT3 during
   4-line reads. The 4-line flow does not first send payload in 1-line mode, so
   a 4-line failure after a passing 1-line test points to DAT1-DAT3. No
   response usually indicates wiring or pull-up issues;
   response with corrupted data usually indicates signal-quality issues.

Use a logic analyzer or oscilloscope when checking these nodes, and make sure
the pull-ups and board-specific SDIO requirements are correct. For more
signal-quality analysis and mitigation guidance, see the SDIO programming guide.

## How To Use

## 1. Build And Flash

Flash the `host` app to the host-capable board and the `slave` app to the
slave-capable board.

For example, in a manual setup with ESP32 host and ESP32-C6 slave:

- flash `examples/peripherals/sdio/hw_test/host` to the ESP32 host board
- flash `examples/peripherals/sdio/hw_test/slave` to the ESP32-C6 slave board

## 2. Open The Host Console

After reset, the host prints a setup menu. Use it to select:

- line width: 1-line or 4-line
- speed: default-speed or high-speed

Then start SDIO initialization.

If initialization fails, the host prints the error and returns to the setup
menu.

## 3. Choose The Traffic Direction

Once the link is up, use the second menu:

- transmit mode: host sends the fixed 16-byte pattern once per second
- receive mode: host reads the fixed 16-byte pattern once per second
- `z`: stop the current loop and return to the second menu

No extra synchronization is performed before each round. The slave is expected
to be ready already.

## 4. Adjust Drive Strength (Optional)

If you want to compare how drive strength affects the observed waveforms or
error rate, change it before starting link initialization:

- on the host, use the setup menu keys `k`, `m`, and `g` to configure CLK,
  CMD, and DATA drive strength
- on the slave, edit `SLAVE_CLK_DRIVE_CAP`, `SLAVE_CMD_DRIVE_CAP`, and
  `SLAVE_DATA_DRIVE_CAP` in `slave/main/app_main.c`, then rebuild and flash

Both sides print the active drive-strength settings during startup.

## 5. Check The Logs

Watch for:

- initialization success or failure,
- transfer success or driver error codes,
- pattern validation success or failure on both boards.

The transfer interval is intentionally slow so repeated waveforms are easy to
capture.

## Pytest Coverage

`pytest_sdio_hw_test.py` drives the same host serial menu that a user sees.

The automated pytest cases exercise these width/speed combinations on the
available SDIO multi-device runners:

- 1-line + default-speed
- 1-line + high-speed
- 4-line + default-speed
- 4-line + high-speed

For each combination, the test enters both host transmit mode and host receive
mode, waits for 5 successful transfers, and fails immediately if any transfer
or pattern-validation error log appears. Other SDIO-capable board pairs can
still use this example manually if their wiring and pin assignments follow the
SDIO programming guide.
