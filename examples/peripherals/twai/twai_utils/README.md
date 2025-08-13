| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# TWAI Console Example

This example demonstrates using the TWAI (Two-Wire Automotive Interface) driver through an interactive console interface. It provides comprehensive TWAI functionality including frame transmission/reception, message filtering, and bus monitoring. The example can be used for both standalone testing via loopback mode and real TWAI network communication.

**Supported Commands:**

| Command | Description | Linux can-utils Equivalent |
|---------|-------------|----------------------------|
| `twai_init <controller> -t <tx> -r <rx> [opts]` | Initialize TWAI controller with GPIO and mode options | `ip link set can0 up type can bitrate 500000` |
| `twai_deinit <controller>` | Deinitialize TWAI controller | `ip link set can0 down` |
| `twai_send <controller> <frame>` | Send TWAI frame (standard/extended/RTR/FD) | `cansend can0 123#DEADBEEF` |
| `twai_dump <controller>[,filter] [-t mode]` / `twai_dump <controller> --stop` | Monitor TWAI traffic with hardware filtering and timestamps | `candump can0` |
| `twai_info <controller>` | Display controller configuration, status | `ip -details link show can0` |
| `twai_recover <controller> [-t <ms>]` | Recover controller from Bus-Off state | N/A |

- Note: `twai_dump` runs continuously in the background. Use `twai_dump <controller> --stop ` to stop monitoring.

## How to Use This Example

### Hardware Required

- Any ESP development board with TWAI support.
- A TWAI transceiver (e.g., SN65HVD230, TJA1050).
- Jumper wires.

### Hardware Setup

Connect the ESP board to a transceiver:
```
ESP32 Pin     Transceiver    TWAI Bus
---------     -----------    --------
GPIO4 (TX) -->  CTX
GPIO5 (RX) <--  CRX
3.3V       -->  VCC
GND        -->  GND
              TWAI_H     <-->  TWAI_H
              TWAI_L     <-->  TWAI_L
```
*Note: The specific GPIO pins for TX and RX must be provided in the `twai_init` command.*

### Quick Start - No Transceiver Mode

For immediate testing without any external hardware, you can use the **No Transceiver Mode** by connecting a single GPIO pin to itself.

```bash
# Connect GPIO4 to itself (or leave it unconnected for self-test)
# Initialize with the same TX/RX GPIO, and enable loopback and self-test modes.
twai> twai_init twai0 -t 4 -r 4 --loopback --self-test

# Send a test frame
twai> twai_send twai0 123#DEADBEEF

# Check controller status
twai> twai_info twai0
```

This mode is ideal for learning the commands, testing application logic, and debugging frame formats without a physical bus.

### Configure the project

```bash
idf.py menuconfig
```

Navigate to **Example Configuration** -> **TWAI Configuration** and configure:

- **Default Arbitration Bitrate**: Default arbitration bitrate in bits per second (bps).
- **Default FD Data Bitrate**: Default data bitrate for TWAI-FD in bits per second (bps).
- **Enable TWAI-FD Support**: Enable TWAI-FD (Flexible Data-rate) support (default: disabled)
- **TX Queue Length**: Length of the transmission queue for TWAI messages (default: 10)

**Note:** For every controller, you must specify the TX and RX pins explicitly with the `-t` and `-r` options when issuing `twai_init`. Failing to do so will make initialization return an error.

### Build and Flash

```bash
idf.py -p PORT flash monitor
```
(To exit the serial monitor, type `Ctrl-]`.)

## Command Reference

### `twai_init`
Initializes and starts the TWAI driver. **TX and RX pins are required.**

**Usage:**
`twai_init <controller> -t <tx_gpio> -r <rx_gpio> [options]`

**Arguments:**
- `<controller>`: Controller ID (`twai0`, `twai1`).
- `-t, --tx`: TX GPIO pin number (required).
- `-r, --rx`: RX GPIO pin number (required).
- `-b, --bitrate`: Arbitration bitrate in bps (default: CONFIG_EXAMPLE_DEFAULT_BITRATE).
- `-B, --fd-bitrate`: Data bitrate for TWAI-FD (FD-capable chips only, default: CONFIG_EXAMPLE_DEFAULT_FD_BITRATE).
- `--loopback`: Enable loopback mode.
- `--self-test`: Enable self-test mode (internal loopback).
- `--listen`: Enable listen-only mode.
- `-c, --clk-out`: Clock output GPIO pin (optional).
- `-o, --bus-off`: Bus-off indicator GPIO pin (optional).

### `twai_deinit`
Stops and de-initializes the TWAI driver.

**Usage:**
`twai_deinit <controller>`

### `twai_send`
Sends a standard, extended, RTR, or TWAI-FD frame.

**Usage:**
`twai_send <controller> <frame_str>`

**Frame Formats:**
- **Standard:** `123#DEADBEEF` (11-bit ID)
- **Extended:** `12345678#CAFEBABE` (29-bit ID)
- **RTR:** `456#R` or `456#R8` (Remote Transmission Request)
- **TWAI-FD:** `123##{flags}{data}` (FD-capable chips only)
  - **flags**: single hex nibble `0..F`
    - bit0 (`0x1`) = BRS (Bit Rate Switch, accelerate data phase)
    - bit1 (`0x2`) = ESI (Error State Indicator)
    - other bits reserved (set to 0)
  - **data**: up to 64 bytes (0..64) of hex pairs, optional `.` separators allowed (e.g. `11.22.33`)
  - example: `123##1DEADBEEF` (BRS enabled, data = DE AD BE EF)

### `twai_dump`
Monitors TWAI bus messages with filtering and candump-style output. This command runs in the background.

**Usage:**
- `twai_dump [-t <mode>] <controller>[,filter...]`
- `twai_dump <controller> --stop`

- **Options:**
- `-t <mode>`: Timestamp mode. Output format is `(seconds.microseconds)` with 6-digit microsecond precision, e.g. `(1640995200.890123)`.
  - `a`: Absolute time (esp_timer microseconds since boot)
  - `d`: Delta time between frames (time since previous frame)
  - `z`: Zero-relative time from start (time since dump started)
  - `n`: No timestamp (default)
- `--stop`: Stop monitoring the specified controller.

**Filter Formats:**
- `id:mask`: Mask filter (e.g., `123:7FF`).
- `low-high`: Range filter (e.g., `100-200`, FD-capable chips only).
- Multiple filters can be combined with commas (e.g., `twai0,123:7FF,100-200`).

### `twai_info`
Displays the TWAI controller's configuration and real-time status.

**Usage:**
`twai_info <controller>`

**Output Includes:**
- Status (Stopped, Running, Bus-Off)
- Node State (Error Active, Error Passive, etc.)
- TX/RX Error Counters
- Bitrate (Arbitration and Data)
- Configured GPIOs and operational modes.

### `twai_recover`
Initiates recovery for a controller that is in the Bus-Off state.

**Usage:**
`twai_recover <controller> [-t <ms>]`

**Options:**
- `-t <ms>`: Recovery timeout.
  - `-1`: Block indefinitely until recovery completes (default).
  - `0`: Asynchronous recovery (returns immediately).
  - `>0`: Timeout in milliseconds.

**Notes:**
- Recovery only works when the controller is in Bus-Off state
- Use `twai_info <controller>` to check current node state
- Recovery may fail if bus conditions are still problematic
- In async mode (timeout=0), use `twai_info` to monitor recovery progress

**Typical Command Sequence:**
1. `twai_init` - Initialize controller
2. `twai_info` - Check status
3. `twai_dump` - Start monitoring (optional)
4. `twai_send` - Send frames
5. `twai_recover` - Recover from errors (if needed)
6. `twai_deinit` - Cleanup

Basic usage example:

```bash
# Initialize controller 0 (bitrate 500 kbps, specify TX/RX pins)
twai> twai_init twai0 -b 500000 -t 4 -r 5

# Display controller information
twai> twai_info twai0
TWAI0 Status: Running
Node State: Error Active
Error Counters: TX=0, RX=0
Bitrate: 500000 bps
GPIOs: TX=GPIO4, RX=GPIO5

# Send standard frame on controller 0
twai> twai_send twai0 123#DEADBEEF

# Start monitoring controller 0 (accept all frames)
twai> twai_dump twai0

# Example received frame display (with default no timestamps)
  twai0  123  [4]  DE AD BE EF
```

### FD-Capable Chips Example

```bash
# Initialize controller 0 with TWAI-FD enabled
twai> twai_init twai0 -b 1000000 -t 4 -r 5 -B 2000000

twai> twai_info twai0
TWAI0 Status: Running
Node State: Error Active
Error Counters: TX=0, RX=0
Bitrate: 1000000 bps (FD: 2000000 bps)
GPIOs: TX=GPIO4, RX=GPIO5

# Send FD frame with BRS on controller 0
twai> twai_send twai0 456##1DEADBEEFCAFEBABE1122334455667788
```

### PC Environment Setup (For Full Testing)

To test bidirectional communication between ESP32 and PC, set up a SocketCAN environment on Ubuntu:

#### Prerequisites

- Ubuntu 18.04 or later
- USB-to-CAN adapter (PEAK PCAN-USB recommended)
- sudo access for network interface configuration

#### Quick Setup

1. **Install CAN utilities:**
```bash
sudo apt update
sudo apt install -y can-utils

# Verify installation
candump --help
cansend --help
```

2. **Configure CAN interface:**
```bash
# Classic CAN setup (500 kbps)
sudo ip link set can0 up type can bitrate 500000

# CAN-FD setup (1M arbitration, 4M data) - requires FD-capable adapter
sudo ip link set can0 up type can bitrate 1000000 dbitrate 4000000 fd on

# Verify interface
ip -details link show can0
```

3. **Test PC setup:**
```bash
# Terminal 1: Monitor
candump can0

# Terminal 2: Send test frame
cansend can0 123#DEADBEEF

# Send FD frame (if FD adapter available)
cansend can0 456##1DEADBEEFCAFEBABE
```

### Bidirectional Testing

Once both PC and ESP32 are set up:

**ESP32 to PC:**
```bash
# PC: Start monitoring
candump can0

# ESP32: Initialize controller 0 and send frame
twai> twai_init twai0 -t 4 -r 5
twai> twai_send twai0 123#DEADBEEF

# PC shows: can0  123   [4]  DE AD BE EF
# ESP32 shows: twai0  123  [4]  DE AD BE EF
```

**PC to ESP32:**
```bash
# ESP32: Start monitoring controller 0
twai> twai_dump twai0

# PC: Send frame
cansend can0 456#CAFEBABE

# ESP32 shows:  twai0  456  [4]  CA FE BA BE

# Stop monitoring
twai> twai_dump twai0 --stop
```

## Advanced Features

### Frame Formats

- **Standard frames:** `123#DEADBEEF` (11-bit ID)
- **Extended frames:** `12345678#CAFEBABE` (29-bit ID)  
- **RTR frames:** `456#R8` (Remote Transmission Request)
- **TWAI-FD frames:** `123##1DEADBEEF` (FD with flags, FD-capable chips only)
- **Data separators:** `123#DE.AD.BE.EF` (dots ignored)


### TWAI-FD Frame Format and Examples (FD-capable chips only)

TWAI-FD frames use two `#` characters: `ID##{flags}{data}`.

- Flags are a single hex nibble (`0..F`). Bit meanings:
  - `0x1` BRS: Enable Bit Rate Switch for the data phase
  - `0x2` ESI: Error State Indicator
  - Other bits are reserved (set to 0)
- Data payload supports up to 64 bytes. The driver maps the payload length to the proper DLC per CAN FD rules automatically.
- Payload hex pairs may include `.` separators for readability (ignored by the parser).

```bash
# FD frame without BRS (flags = 0) on controller 0
twai> twai_send twai0 123##0DEADBEEFCAFEBABE1122334455667788

# FD frame with BRS (flags = 1, higher data speed)
twai> twai_send twai0 456##1DEADBEEFCAFEBABE1122334455667788

# FD frame with ESI (flags = 2)
twai> twai_send twai0 789##2DEADBEEF

# FD frame with BRS + ESI (flags = 3)
twai> twai_send twai0 ABC##3DEADBEEF

# Large FD frame (up to 64 bytes)
twai> twai_send twai0 DEF##1000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F
```

### Filtering and Monitoring

```bash
# Monitor controller 0 (accept all frames)
twai> twai_dump twai0
  twai0  123  [4]  DE AD BE EF
  twai0  456  [2]  CA FE
  twai0  789  [8]  11 22 33 44 55 66 77 88

# Monitor with absolute timestamps
twai> twai_dump -t a twai0
(1640995200.890123)   twai0  123  [4]  DE AD BE EF
(1640995200.895555)   twai0  456  [2]  CA FE
(1640995200.901000)   twai0  789  [8]  11 22 33 44 55 66 77 88

# Monitor with delta timestamps (time between frames)
twai> twai_dump -t d twai0
(0.000000)   twai0  123  [4]  DE AD BE EF
(0.005432)   twai0  456  [2]  CA FE
(0.005445)   twai0  789  [8]  11 22 33 44 55 66 77 88

# Monitor with zero-relative timestamps (from start of monitoring)
twai> twai_dump -t z twai0
(0.000000)   twai0  123  [4]  DE AD BE EF
(0.005432)   twai0  456  [2]  CA FE
(0.010877)   twai0  789  [8]  11 22 33 44 55 66 77 88

# Monitor without timestamps (default)
twai> twai_dump -t n twai0
  twai0  123  [4]  DE AD BE EF
  twai0  456  [2]  CA FE
  twai0  789  [8]  11 22 33 44 55 66 77 88

# Monitor controller 0 with exact ID filter (only receive ID=0x123)
twai> twai_dump twai0,123:7FF
  Mask Filter 0: ID=0x00000123, mask=0x000007FF, STD
   twai0  123  [4]  DE AD BE EF

# Monitor controller 0 with ID range 0x100-0x10F (mask filter approach)
twai> twai_dump twai0,100:7F0
  Mask Filter 0: ID=0x00000100, mask=0x000007F0, STD

# Monitor controller 0 with range filter (0xa to 0x15) - FD-capable chips only
twai> twai_dump twai0,a-15
  Range Filter 0: 0x0000000a - 0x00000015, STD

# Monitor controller 0 with range filter (0x000 to 0x666)
twai> twai_dump twai0,000-666
  Range Filter 0: 0x00000000 - 0x00000666, STD

# Monitor controller 0 with mixed filters (mask + range)
twai> twai_dump twai0,123:7FF,a-15
  Mask Filter 0: ID=0x00000123, mask=0x000007FF, STD
  Range Filter 0: 0x0000000a - 0x00000015, STD

# Monitor controller 0 with dual filters
twai> twai_dump twai0,020:7F0,013:7F8
  Mask Filter 0: ID=0x00000020, mask=0x000007F0, STD
  Mask Filter 1: ID=0x00000013, mask=0x000007F8, STD

# Monitor controller 0 with multiple range filters  
twai> twai_dump twai0,10-20,100-200
  Range Filter 0: 0x00000010 - 0x00000020, STD
  Range Filter 1: 0x00000100 - 0x00000200, STD

# Monitor all frames on controller 0 (no filter)
twai> twai_dump twai0

# Stop monitoring controller 0
twai> twai_dump twai0 --stop
```

**Filter Types (FD-capable chips only):**
- **Mask filters:** `id:mask` format - Uses bitwise matching with configurable mask
- **Range filters:** `low-high` format - Hardware range filtering for ID ranges
- **Mixed filtering:** Combine both types in one command for maximum flexibility

### Testing Modes

**No Transceiver Mode (Testing without external hardware):**
```bash
# Use same GPIO for TX and RX with loopback and self-test
twai> twai_init twai0 -t 4 -r 4 --loopback --self-test
twai> twai_dump twai0
twai> twai_send twai0 123#DEADBEEF
# Frame appears immediately in dump output:
  twai0  123  [4]  DE AD BE EF
twai> twai_dump twai0 --stop
```
**Note:** This mode is perfect for testing TWAI functionality without external transceivers or wiring. The same GPIO is used for both TX and RX, and the combination of `--loopback` and `--self-test` flags ensures frames are properly transmitted and received internally.

**Loopback mode (with external transceiver):**
```bash
twai> twai_init twai0 -t 4 -r 5 --loopback
twai> twai_dump twai0
twai> twai_send twai0 123#54455354
# Frame appears immediately in dump output:
  twai0  123  [4]  54 45 53 54

# Stop monitoring when done
twai> twai_dump twai0 --stop

# FD loopback test (FD-capable chips only)
twai> twai_init twai0 -t 4 -r 5 --loopback -B 2000000
twai> twai_dump twai0
twai> twai_send twai0 456##1DEADBEEFCAFEBABE1122334455667788
  twai0  456  [16]  DE AD BE EF CA FE BA BE 11 22 33 44 55 66 77 88  # FD frame (BRS)

# Stop monitoring
twai> twai_dump twai0 --stop
```

**Listen-only mode:**
```bash
twai> twai_init twai0 -t 4 -r 5 --listen
twai> twai_dump twai0
# Can receive but cannot send frames
# Stop with: twai_dump twai0 --stop
```

### Error Recovery and Diagnostics

**Bus-Off Recovery:**
The TWAI controller can enter a Bus-Off state due to excessive error conditions. Use the `twai_recover` command to initiate recovery:

```bash
# Basic recovery (default: block until complete)
twai> twai_recover twai0
I (1234) cmd_twai_core: Starting recovery from Bus-Off state...
I (1345) cmd_twai_core: Waiting for recovery to complete...
I (1456) cmd_twai_core: Recovery completed successfully in 100 ms

# Recovery with custom timeout
twai> twai_recover twai0 -t 5000
I (1234) cmd_twai_core: Starting recovery from Bus-Off state...
I (1345) cmd_twai_core: Waiting for recovery to complete...
I (1456) cmd_twai_core: Recovery completed successfully in 150 ms

# Asynchronous recovery (return immediately)
twai> twai_recover twai0 -t 0
I (1234) cmd_twai_core: Starting recovery from Bus-Off state...
I (1245) cmd_twai_core: Recovery initiated (async mode)

# If node is not in Bus-Off state
twai> twai_recover twai0
I (1234) cmd_twai_core: Recovery not needed - node is Error Active
```

**Enhanced Status Information:**
The `twai_info` command now displays real-time dynamic status information:

```bash
twai> twai_info twai0
TWAI0 Status: Running
Node State: Error Active
Error Counters: TX=0, RX=0
Bitrate: 500000 bps
GPIOs: TX=GPIO4, RX=GPIO5
```

**Status and Node State Interpretations:**
- **Status: Running**: Driver initialized and operational (not Bus-Off)  
- **Status: Bus-Off**: Controller offline due to excessive errors, requires recovery
- **Status: Stopped**: Driver not initialized
- **Node State: Error Active**: Normal operation, can transmit and receive freely
- **Node State: Error Warning**: Warning level reached (error counters ≥ 96)
- **Node State: Error Passive**: Passive mode (error counters ≥ 128, limited transmission)
- **Node State: Bus Off**: Controller offline (TX error counter ≥ 256, requires recovery)

## Troubleshooting

**Restoring Default Configuration:**
Use `twai_deinit <controller>` followed by `twai_init <controller>` to reset the driver to default settings.

**Bus-Off Recovery Issues:**
- **"Node is not in Bus-Off state"**: Recovery only works when the controller is in Bus-Off state. Use `twai_info` to check current node state.
- **Recovery timeout**: If recovery takes longer than expected, try increasing the timeout with `-t` option (e.g., `-t 15000` for 15 seconds).
- **Recovery fails**: Check physical bus conditions, ensure proper termination and that other nodes are present to acknowledge recovery frames.

**TWAI-FD specific issues:**
- **"TWAI-FD frames not supported"**: Your chip doesn't support FD mode. Use chips like ESP32-C5
- **FD bitrate validation**: Ensure FD data bitrate is higher than arbitration bitrate
- **PC FD compatibility**: Ensure your PC CAN adapter supports FD mode

**PC CAN issues:**
```bash
# Reset PC interface
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 500000

# For FD mode
sudo ip link set can0 up type can bitrate 1000000 dbitrate 4000000 fd on
```

**No communication:**
- Verify bitrates match on both sides
- For FD: Ensure both sides support FD and have compatible transceivers
- Check physical connections and transceiver power
- GPIO pins must be specified in the twai_init command (common: TX=GPIO4, RX=GPIO5)
- For no-transceiver testing, use the same GPIO for both TX and RX with `--loopback --self-test` flags
- Ensure proper CAN bus termination (120Ω resistors)
- Use loopback mode to test ESP32 functionality independently

**Quick diagnostic with no-transceiver mode:**
```bash
# Test if basic TWAI functionality works
twai> twai_init twai0 -t 4 -r 4 --loopback --self-test
twai> twai_send twai0 123#DEADBEEF
# If this fails, check ESP-IDF installation and chip support
```

**Common Error Messages:**
```bash
# Controller ID missing
twai> twai_send 123#DEADBEEF
E (1234) cmd_twai_send: Controller ID is required

# Interface not initialized
twai> twai_send twai0 123#DEADBEEF
E (1234) cmd_twai_send: TWAI0 not initialized

# Invalid frame format
twai> twai_send twai0 123DEADBEEF
E (1456) cmd_twai_send: Frame string is required (format: 123#AABBCC or 12345678#AABBCC)

# Invalid controller ID
twai> twai_init twai5 -t 4 -r 5
E (1678) cmd_twai_core: Invalid controller ID
```