| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# TWAI Network Example

This example demonstrates TWAI (Two-Wire Automotive Interface) network communication using the ESP-IDF TWAI driver. It consists of two programs that showcase different aspects of TWAI bus communication.

## Overview

### Programs

- **twai_sender**: Sends periodic heartbeat messages and large data packets
- **twai_listen_only**: Monitors specific message ID using a filter

### Key Features

- Event-driven message handling with callbacks
- Message filtering using acceptance filters in listen-only mode
- Single/Burst data transmission and reception
- Real-time bus error and node status reporting

## Hardware Setup

### Wiring

For multi-device testing:
1. Connect TWAI transceivers to each ESP32xx
2. Wire TWAI_H and TWAI_L between all devices using line topology
3. Add 120Ω termination resistors at both ends of the bus

For single-device testing, enable self_test mode in the sender.

### GPIO Configuration

The GPIO pins can be configured using menuconfig:

```bash
idf.py menuconfig
```

Navigate to: `Example Configuration` → Configure the following:
- **TWAI TX GPIO Num**: GPIO pin for TWAI TX  
- **TWAI RX GPIO Num**: GPIO pin for TWAI RX

**Default configuration:**
```c
#define TWAI_TX_GPIO    GPIO_NUM_4
#define TWAI_RX_GPIO    GPIO_NUM_5  
#define TWAI_BITRATE    1000000     // 1 Mbps
```

## Message Types

| ID | Type | Frequency | Size | Description |
|----|------|-----------|------|-------------|
| 0x7FF | Heartbeat | 1 Hz | 8 bytes | Timestamp data |
| 0x100 | Data | Every 10s | 1000 bytes | Test data (125 frames) |

## Building and Running

### Build each program (sender, listen_only):

**Enter the sub-application directory and run:**

```bash
idf.py set-target esp32 build flash monitor
```

## Expected Output

### Sender
```
===================TWAI Sender Example Starting...===================
I (xxx) twai_sender: TWAI Sender started successfully
I (xxx) twai_sender: Sending messages on IDs: 0x100 (data), 0x7FF (heartbeat)
I (xxx) twai_sender: Sending heartbeat message: 1234567890
I (xxx) twai_sender: Sending packet of 1000 bytes in 125 frames
```

### Listen-Only Monitor
```
===================TWAI Listen Only Example Starting...===================
I (xxx) twai_listen: Buffer initialized: 200 slots for burst data
I (xxx) twai_listen: TWAI node created
I (xxx) twai_listen: Filter enabled for ID: 0x100 Mask: 0x7F0
I (xxx) twai_listen: TWAI start listening...
I (xxx) twai_listen: RX: 100 [8] 0 0 0 0 0 0 0 0
I (xxx) twai_listen: RX: 100 [8] 1 1 1 1 1 1 1 1
```

## Implementation Details

### Message Buffering

Each program uses a buffer pool to handle incoming messages efficiently:

- **Sender**: Small buffer for transmission completion tracking
- **Listen-Only**: 100-slot buffer for monitoring filtered traffic

### Operating Modes

- **Normal Mode** (Sender): Participates in bus communication, sends ACK frames
- **Listen-Only Mode** (Monitor): Receives filtered messages without transmitting anything

### Message Filtering

The listen-only monitor uses hardware acceptance filters to receive only specific message IDs:

```c
twai_mask_filter_config_t data_filter = {
    .id = TWAI_DATA_ID,
    .mask = 0x7F0,      // Match high 7 bits of the ID, ignore low 4 bits
    .is_ext = false,    // Receive only standard ID
};
```

### Error Handling

- Bus error logging and status monitoring

## Configuration

### Customizing Buffer Sizes

Adjust buffer sizes in each program as needed:
```c
#define POLL_DEPTH  200  // Listen-only buffer size
```

### Changing Message IDs

Update the message ID definitions:
```c
#define TWAI_DATA_ID       0x100
#define TWAI_HEARTBEAT_ID  0x7FF
```

## Use Cases

This example is suitable for:

- Learning TWAI bus communication
- Testing TWAI network setups
- Developing custom TWAI protocols
- Bus monitoring and debugging
- Prototyping automotive communication systems

## Troubleshooting

### No Communication
- Check GPIO pin connections
- Verify bitrate settings match between devices
- Ensure proper bus termination

### Buffer Overflows
- Increase buffer size (`POLL_DEPTH`)
- Reduce bus message transmission rate
- Optimize message processing code

### Bus Errors
- Check physical bus wiring
- Verify termination resistors (120Ω at each end)
- Monitor error counters with `twai_node_get_info()`
