# BLE Log Module

A high-performance, modular Bluetooth logging system that provides real-time log capture and transmission capabilities for the ESP-IDF Bluetooth stack.

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

- **Multi-source Log Collection**: Supports multiple log sources including Link Layer, Host, HCI, etc.
- **High Concurrency Processing**: Uses atomic and spin lock mechanisms for multi-task concurrent writing
- **Real-time Transmission**: Asynchronous transmission mechanism based on FreeRTOS tasks
- **Data Integrity**: Configurable checksum mechanism ensures data integrity
- **Memory Optimization**: Ping-pong buffer design minimizes memory usage

### Advanced Features

- **Timestamp Synchronization**: Supports timestamp synchronization with external devices (optional)
- **Enhanced Statistics**: Detailed logging statistics including loss rate analysis (optional)
- **Link Layer Integration**: Deep integration with ESP-IDF Bluetooth Link Layer
- **Multiple Transmission Methods**: Supports SPI DMA, UART DMA, and Dummy transmission

### Performance Features

- **IRAM Optimization**: Critical path code runs in IRAM ensuring low latency
- **Lock-free Design**: Most operations use atomic operations reducing lock contention
- **Buffer Reuse**: Intelligent buffer management reduces memory allocation overhead

## Quick Start

### 1. Enable Module

Enable the BLE Log module in `menuconfig`:

```
Component config → Bluetooth → Enable BLE Log Module (Experimental)
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
| `CONFIG_BLE_LOG_ENABLED` | n | Enable BLE Log module |
| `CONFIG_BLE_LOG_LBM_TRANS_SIZE` | 512 | Size of each transport buffer |
| `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT` | 2 | Number of atomic lock LBMs for task context |
| `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_ISR_CNT` | 1 | Number of atomic lock LBMs for ISR context |

### Link Layer Configuration

| Configuration | Default | Description |
|---------------|---------|-------------|
| `CONFIG_BLE_LOG_LL_ENABLED` | y | Enable Link Layer logging |
| `CONFIG_BLE_LOG_LBM_LL_TRANS_SIZE` | 1024 | Link Layer transport buffer size |

### Advanced Features

| Configuration | Default | Description |
|---------------|---------|-------------|
| `CONFIG_BLE_LOG_PAYLOAD_CHECKSUM_ENABLED` | y | Enable payload checksum |
| `CONFIG_BLE_LOG_ENH_STAT_ENABLED` | n | Enable enhanced statistics |
| `CONFIG_BLE_LOG_TS_ENABLED` | n | Enable timestamp synchronization |

### Transport Method Configuration

| Transport | Configuration | Description |
|-----------|---------------|-------------|
| Dummy | `CONFIG_BLE_LOG_PRPH_DUMMY` | Debug dummy transport |
| SPI Master DMA | `CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA` | SPI DMA transport |
| UART DMA | `CONFIG_BLE_LOG_PRPH_UART_DMA` | UART DMA transport |

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
    BLE_LOG_SRC_MAX,
} ble_log_src_t;
```

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

Memory usage under default configuration:

```
Total Buffers = (Atomic Task LBMs + Atomic ISR LBMs + Spin LBMs) × 2 × Transport Buffer Size
Default Config = (2 + 1 + 2) × 2 × 512 = 5120 bytes

Additional when Link Layer enabled:
LL Buffers = 2 × 2 × 1024 = 4096 bytes

Additional when Enhanced Statistics enabled:
Statistics Data = Log Source Count × sizeof(ble_log_stat_mgr_t) = 8 × 40 = 320 bytes
```

### Performance Optimization Recommendations

1. **Adjust LBM Count**: Adjust atomic lock LBM count based on concurrency requirements
2. **Buffer Size**: Adjust transport buffer size based on log volume
3. **Transport Method**: Choose optimal transport method based on hardware (SPI DMA typically has best performance)
4. **Checksum**: Consider disabling payload checksum when performance requirements are extremely high

### Real-time Considerations

- Critical code paths are marked with `BLE_LOG_IRAM_ATTR` and run in IRAM
- Atomic operations avoid lock contention
- Ping-pong buffers ensure continuous writing

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
// Enable enhanced statistics to check loss rate
#if CONFIG_BLE_LOG_ENH_STAT_ENABLED
// Statistics will be automatically included in logs
#endif

// Adjust buffer size
// CONFIG_BLE_LOG_LBM_TRANS_SIZE=1024

// Increase atomic lock LBM count
// CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT=4
```

#### 3. Performance Issues

**Symptoms**: System response becomes slow

**Possible Causes**:
- Checksum calculation overhead
- Transmission bottleneck
- Lock contention

**Solutions**:
```c
// Disable payload checksum
// CONFIG_BLE_LOG_PAYLOAD_CHECKSUM_ENABLED=n

// Use faster transmission method
// CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA=y

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

#### 2. Enable Enhanced Statistics

```c
// Enable in menuconfig
// CONFIG_BLE_LOG_ENH_STAT_ENABLED=y
// Statistics will be automatically output to logs
```

#### 3. Monitor Memory Usage

```c
void monitor_memory() {
    printf("Free heap before init: %d\n", esp_get_free_heap_size());
    ble_log_init();
    printf("Free heap after init: %d\n", esp_get_free_heap_size());
}
```
