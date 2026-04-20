# BLE Log Module

A high-performance, modular Bluetooth logging system that provides real-time log capture and asynchronous transmission capabilities for the ESP-IDF Bluetooth stack.

## Table of Contents

- [Overview](#overview)
- [Architecture Design](#architecture-design)
- [Features](#features)
- [Quick Start](#quick-start)
- [Configuration Options](#configuration-options)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Performance & Memory Optimization](#performance--memory-optimization)
- [Troubleshooting](#troubleshooting)
- [Important Notes](#important-notes)

## Overview

The BLE Log module is an efficient logging system specifically designed for the ESP-IDF Bluetooth stack, supporting real-time log capture, multi-source log collection, and various transmission methods. This module has been refactored with a modular design, featuring high-concurrency processing capabilities and low-latency characteristics.

### Main Components

- **BLE Log Core** (`ble_log.c`): Module core responsible for initialization and coordination of sub-modules
- **Runtime Manager** (`ble_log_rt.c`): Runtime task management for log transmission scheduling
- **Log Buffer Manager** (`ble_log_lbm.c`): Log buffer management supporting multiple locking mechanisms
- **Peripheral Interface** (`ble_log_prph_*.c`): Peripheral interface abstraction layer supporting various transmission methods
- **Timestamp Sync** (`ble_log_ts.c`): Timestamp synchronization module
- **Utility** (`ble_log_util.c`): Common utility functions

## Features

### Core Functionality

- **Multi-source Log Collection**: Supports multiple log sources including Link Layer, Host, HCI, UART redirection, etc.
- **High Concurrency Processing**: Uses atomic and spin lock mechanisms for multi-task concurrent writing
- **Real-time Transmission**: Asynchronous transmission mechanism based on FreeRTOS tasks
- **Data Integrity**: Checksum mechanism ensures data integrity (always enabled)
- **Multi-buffer Transport**: Each LBM manages multiple transport buffers (default 4) for improved throughput over the legacy ping-pong design
- **Cross-pool Buffer Fallback**: LBM acquire attempts all atomic LBMs before falling back to spinlock LBMs, improving buffer availability under contention

### Advanced Features

- **UART Redirection**: When using UART DMA on PORT 0, UART output (including `esp_rom_printf`) is transparently redirected through the async log pipeline
- **Timestamp Synchronization**: Supports timestamp synchronization with external devices (optional)
- **Enhanced Statistics**: Detailed logging statistics including written/lost frame and byte counts (always enabled)
- **Buffer Utilization Reporting**: Per-LBM buffer utilization and inflight peak tracking for diagnostics
- **Link Layer Integration**: Deep integration with ESP-IDF Bluetooth Link Layer
- **Multiple Transmission Methods**: Supports SPI Master DMA, UART DMA, and Dummy transmission

### Performance Features

- **IRAM Optimization**: Critical path code runs in IRAM ensuring low latency
- **Lock-free Design**: Most operations use atomic operations reducing lock contention
- **Buffer Reuse**: Intelligent buffer management reduces memory allocation overhead

## Quick Start

### 1. Enable Module

Enable the BLE Log module in `menuconfig`:

```
Component config → Bluetooth → Enable BT Log Async Output (Dev Only)
```

### 2. Basic Configuration

```c
#include "ble_log.h"

void app_main() {
    // Initialize BLE Log module
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "Failed to initialize BLE Log");
        return;
    }
    
    // Write log data
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    ble_log_write_hex(BLE_LOG_SRC_CUSTOM, data, sizeof(data));
    
    // Force flush buffers
    ble_log_flush();
    
    // Cleanup resources
    ble_log_deinit();
}
```

### 3. Link Layer Integration

When `CONFIG_BLE_LOG_LL_ENABLED` is enabled, Link Layer logs are automatically integrated:

```c
// Link Layer logs will automatically call this function
void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr,
                          uint32_t len_append, const uint8_t *addr_append, 
                          uint32_t flag);
```

## Configuration Options

### Basic Configuration

| Configuration | Default | Description |
|---------------|---------|-------------|
| `CONFIG_BLE_LOG_ENABLED` | n | Enable BT Log Async Output |
| `CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE` | 2048 | Total buffer memory per common LBM (bytes). Divided equally among `BLE_LOG_TRANS_BUF_CNT` (4) internal transport buffers. |
| `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT` | 2 | Number of atomic lock LBMs for task context |
| `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_ISR_CNT` | 1 | Number of atomic lock LBMs for ISR context |

### Link Layer Configuration

| Configuration | Default | Description |
|---------------|---------|-------------|
| `CONFIG_BLE_LOG_LL_ENABLED` | y | Enable Link Layer logging |
| `CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE` | 2048 | Total buffer memory per Link Layer LBM (bytes). Divided equally among `BLE_LOG_TRANS_BUF_CNT` (4) internal transport buffers. |

### Other Features

| Configuration | Default | Description |
|---------------|---------|-------------|
| `CONFIG_BLE_LOG_TS_ENABLED` | n | Enable timestamp synchronization |
| `CONFIG_BLE_LOG_HOST_HCI_LOG_ENABLED` | n | Enable BLE Host side HCI logging |

> **Note**: Payload checksum and enhanced statistics are now always enabled and no longer have separate Kconfig options.

### Transport Method Configuration

| Transport | Configuration | Description |
|-----------|---------------|-------------|
| Dummy | `CONFIG_BLE_LOG_PRPH_DUMMY` | Debug dummy transport (default unless `SOC_UHCI_SUPPORTED`) |
| SPI Master DMA | `CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA` | SPI DMA transport |
| UART DMA | `CONFIG_BLE_LOG_PRPH_UART_DMA` | UART DMA transport (default when `SOC_UHCI_SUPPORTED`). Default baud rate: 3000000. |

### Deprecated / Removed

| Module | Status | Notes |
|--------|--------|-------|
| Legacy SPI Log Output | Deprecated | Moved to `deprecated/` directory. Use BT Log Async Output instead. A separate Kconfig menu "Legacy SPI Log Output" is available for backward compatibility. |
| UHCI Out | Removed | The standalone UHCI Out module (`ble_log_uhci_out.c`) has been removed. UART DMA transport under the main BLE Log peripheral interface replaces it. |

## API Reference

### Core API

#### `bool ble_log_init(void)`

Initialize the BLE Log module.

**Return Value**: 
- `true`: Initialization successful
- `false`: Initialization failed

**Note**: Must be called before using any other APIs.

#### `void ble_log_deinit(void)`

Cleanup the BLE Log module and release all resources.

**Note**: 
- All pending logs will be lost after calling this function
- Peripheral interface will be cleaned up first to avoid DMA transmission issues during memory release

#### `bool ble_log_write_hex(ble_log_src_t src_code, const uint8_t *addr, size_t len)`

Write hexadecimal log data.

**Parameters**:
- `src_code`: Log source code
- `addr`: Data pointer
- `len`: Data length

**Return Value**:
- `true`: Write successful
- `false`: Write failed (module not initialized or insufficient buffer)

#### `void ble_log_flush(void)`

Force flush all buffers and send pending logs immediately.

**Note**: This operation is blocking and will pause module operation until all buffers are cleared.

#### `void ble_log_dump_to_console(void)`

Output all buffer contents to console in hexadecimal format for debugging.

### Log Source Types

```c
typedef enum {
    BLE_LOG_SRC_INTERNAL = 0,    // Internal system logs
    BLE_LOG_SRC_CUSTOM,          // User-defined logs
    BLE_LOG_SRC_LL_TASK,         // Link Layer task logs
    BLE_LOG_SRC_LL_HCI,          // Link Layer HCI logs
    BLE_LOG_SRC_LL_ISR,          // Link Layer interrupt logs
    BLE_LOG_SRC_HOST,            // Host layer logs
    BLE_LOG_SRC_HCI,             // HCI layer logs
    BLE_LOG_SRC_ENCODE,          // Encoding layer logs
    BLE_LOG_SRC_REDIR,           // UART redirection (PORT 0 only)
    BLE_LOG_SRC_MAX,
} ble_log_src_t;
```

### HCI Log Macro

```c
#define ble_log_write_hci(direction, data, len)
```

Writes an HCI packet with direction encoding. `direction` is `BLE_LOG_HCI_DOWNSTREAM` (0) or `BLE_LOG_HCI_UPSTREAM` (1). Direction is encoded in the MSB of the first byte (HCI type).

### Link Layer API (Conditional Compilation)

#### `void ble_log_write_hex_ll(uint32_t len, const uint8_t *addr, uint32_t len_append, const uint8_t *addr_append, uint32_t flag)`

Link Layer dedicated log writing interface.

**Parameters**:
- `len`: Main data length
- `addr`: Main data pointer
- `len_append`: Append data length  
- `addr_append`: Append data pointer
- `flag`: Log flag bits

**Flag Definitions**:
```c
enum {
    BLE_LOG_LL_FLAG_CONTINUE = 0,
    BLE_LOG_LL_FLAG_END,
    BLE_LOG_LL_FLAG_TASK,
    BLE_LOG_LL_FLAG_ISR,
    BLE_LOG_LL_FLAG_HCI,
    BLE_LOG_LL_FLAG_RAW,
    BLE_LOG_LL_FLAG_OMDATA,
    BLE_LOG_LL_FLAG_HCI_UPSTREAM,
};
```

### Timestamp Synchronization API (Conditional Compilation)

#### `bool ble_log_sync_enable(bool enable)`

Enable or disable timestamp synchronization functionality.

**Parameters**:
- `enable`: true to enable, false to disable

**Return Value**:
- `true`: Operation successful
- `false`: Operation failed (module not initialized)

## Usage Examples

### Example 1: Basic Logging

```c
#include "ble_log.h"

void example_basic_logging() {
    // Initialize
    if (!ble_log_init()) {
        printf("BLE Log init failed\n");
        return;
    }
    
    // Log some example data
    uint8_t hci_cmd[] = {0x01, 0x03, 0x0C, 0x00};  // HCI Reset Command
    ble_log_write_hex(BLE_LOG_SRC_HCI, hci_cmd, sizeof(hci_cmd));
    
    uint8_t host_data[] = {0x02, 0x00, 0x20, 0x0B, 0x00, 0x07, 0x00, 0x04, 0x00, 0x10, 0x01, 0x00, 0xFF, 0xFF, 0x00, 0x28};
    ble_log_write_hex(BLE_LOG_SRC_HOST, host_data, sizeof(host_data));
    
    // Force send
    ble_log_flush();
    
    // Cleanup
    ble_log_deinit();
}
```

### Example 2: ISR Context Logging

```c
void IRAM_ATTR some_isr_handler() {
    uint8_t isr_data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    
    // Safe to write logs in ISR context
    ble_log_write_hex(BLE_LOG_SRC_LL_ISR, isr_data, sizeof(isr_data));
}
```

### Example 3: Logging with Timestamp Synchronization

```c
void example_with_timestamp_sync() {
    if (!ble_log_init()) {
        return;
    }
    
    #if CONFIG_BLE_LOG_TS_ENABLED
    // Enable timestamp synchronization
    ble_log_sync_enable(true);
    #endif
    
    // Log data...
    uint8_t data[] = {0x01, 0x02, 0x03};
    ble_log_write_hex(BLE_LOG_SRC_CUSTOM, data, sizeof(data));
    
    // Timestamp information will be automatically included in logs
    
    ble_log_deinit();
}
```

### Example 4: Performance Testing

```c
void example_performance_test() {
    if (!ble_log_init()) {
        return;
    }
    
    uint8_t test_data[100];
    for (int i = 0; i < 100; i++) {
        test_data[i] = i;
    }
    
    uint32_t start_time = esp_timer_get_time();
    
    // Send 1000 logs
    for (int i = 0; i < 1000; i++) {
        ble_log_write_hex(BLE_LOG_SRC_CUSTOM, test_data, sizeof(test_data));
    }
    
    ble_log_flush();
    uint32_t end_time = esp_timer_get_time();
    
    printf("Time to write 1000 logs: %lu us\n", end_time - start_time);
    
    ble_log_deinit();
}
```

## Performance & Memory Optimization

### Memory Usage Estimation

Each LBM's total buffer memory is configured directly via Kconfig. The configured value is divided equally among `BLE_LOG_TRANS_BUF_CNT` (currently 4) internal transport buffers.

Memory usage under default configuration:

```
Common Pool:
  LBM count = Atomic Task (2) + Atomic ISR (1) + Spinlock (2) = 5
  Total     = 5 × BLE_LOG_LBM_TRANS_BUF_SIZE = 5 × 2048 = 10240 bytes

Link Layer Pool (when CONFIG_BLE_LOG_LL_ENABLED):
  LBM count = 2 (LL task + LL HCI)
  Total     = 2 × BLE_LOG_LBM_LL_TRANS_BUF_SIZE = 2 × 2048 = 4096 bytes

Statistics (always enabled):
  Total     = BLE_LOG_SRC_MAX × sizeof(ble_log_stat_mgr_t) = 9 × 20 = 180 bytes

UART Redirect (when UART DMA on PORT 0):
  Additional BLE_LOG_TRANS_BUF_CNT (4) transport buffers
```

### Performance Optimization Recommendations

1. **Adjust LBM Count**: Adjust atomic lock LBM count based on concurrency requirements
2. **Buffer Size**: Adjust total buffer memory per LBM based on log volume; must be a multiple of `BLE_LOG_TRANS_BUF_CNT` (4)
3. **Transport Method**: Choose optimal transport method based on hardware (UART DMA is default on supported SoCs)

### Real-time Considerations

- Critical code paths are marked with `BLE_LOG_IRAM_ATTR` and run in IRAM
- Atomic operations avoid lock contention
- Multi-buffer transport ensures continuous writing even when some buffers are in-flight

## Troubleshooting

### Common Issues

#### 1. Initialization Failure

**Symptoms**: `ble_log_init()` returns `false`

**Possible Causes**:
- Insufficient memory
- Peripheral configuration error
- Duplicate initialization

**Solutions**:
```c
// Check available memory
printf("Free heap: %d bytes\n", esp_get_free_heap_size());

// Ensure initialization only happens once
static bool initialized = false;
if (!initialized) {
    initialized = ble_log_init();
}
```

#### 2. Log Loss

**Symptoms**: Some logs don't appear in output

**Possible Causes**:
- Buffer overflow
- Transmission speed can't keep up with write speed
- Module not properly initialized

**Solutions**:
```c
// Enhanced statistics are always enabled — check written/lost frame
// and byte counts in the log stream output

// Increase total buffer memory per LBM
// CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE=4096

// Increase atomic lock LBM count
// CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT=4
```

#### 3. Performance Issues

**Symptoms**: System response becomes slow

**Possible Causes**:
- Transmission bottleneck
- Lock contention

**Solutions**:
```c
// Use faster transmission method
// CONFIG_BLE_LOG_PRPH_UART_DMA=y (default on SOC_UHCI_SUPPORTED targets)

// Increase baud rate (default is now 3000000)
// CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE=3000000

// Adjust task priority
#define BLE_LOG_TASK_PRIO configMAX_PRIORITIES-3
```

### Debugging Techniques

#### 1. Use Dummy Transport for Debugging

```c
// Select Dummy transport in menuconfig
// Then use dump function to view buffer contents
ble_log_dump_to_console();
```

#### 2. Check Enhanced Statistics

```c
// Enhanced statistics are always enabled
// Written/lost frame and byte counts are automatically output to logs
```

#### 3. Monitor Memory Usage

```c
void monitor_memory() {
    printf("Free heap before init: %d\n", esp_get_free_heap_size());
    ble_log_init();
    printf("Free heap after init: %d\n", esp_get_free_heap_size());
}
```

## Important Notes

### Buffer Size Constraints

- `CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE` and `CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE` must be multiples of `BLE_LOG_TRANS_BUF_CNT` (currently 4)
- The per-buffer size (total ÷ 4) must be at least large enough to hold one frame overhead (`BLE_LOG_FRAME_OVERHEAD`)
- `BLE_LOG_TRANS_BUF_CNT` must be a power of 2

### Migration from Legacy Modules

- **UHCI Out**: The standalone `ble_log_uhci_out` module has been removed. Use the UART DMA peripheral transport (`CONFIG_BLE_LOG_PRPH_UART_DMA`) instead.
- **SPI Out**: The legacy SPI log output has been moved to `deprecated/`. A separate Kconfig menu "Legacy SPI Log Output (Deprecated)" is available for backward compatibility, but new projects should use BT Log Async Output with the SPI Master DMA peripheral transport.
