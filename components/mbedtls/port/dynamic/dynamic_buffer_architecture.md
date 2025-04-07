# Dynamic Buffer Management in mbedTLS for ESP-IDF

## Executive Summary

ESP-IDF implements a dynamic buffer management system for mbedTLS to optimize memory usage during TLS/SSL connections. This architecture significantly reduces RAM requirements on memory-constrained ESP devices by intelligently allocating buffers only when needed and sizing them according to actual message requirements rather than worst-case scenarios.

## The Problem We're Solving

Standard TLS implementations allocate large static buffers that:
- Reserve memory for the entire connection lifetime
- Are sized for worst-case scenarios (large certificates, messages)
- Remain allocated even when not in use

On memory-constrained IoT devices like ESP32, this traditional approach is inefficient and limits the number of concurrent connections possible.

## Our Solution: The Dynamic Buffer Approach

Instead of static allocation, our system:
1. **Allocates buffers only when needed**
2. **Right-sizes buffers** based on actual message requirements
3. **Releases memory** when it's not needed
4. **Preserves critical state** in small cache buffers

## How It Works: A Conceptual View

### Buffer Lifecycle

1. **Starting state**: Begin with minimal or no buffer allocation
2. **Just before data transmission/reception**: Allocate right-sized buffer
3. **During data processing**: Use the allocated buffer
4. **After processing**: Replace large buffer with small cache buffer
5. **Repeat** as needed during the connection

### Key Concepts Illustrated

#### Transmission (TX) Buffer Handling

```
[Small idle buffer] → [Right-sized TX buffer] → [Back to small buffer]
```

#### Reception (RX) Buffer Handling

```
[No buffer] → [Header buffer] → [Right-sized RX buffer] → [Small cache buffer]
```

### Implementation Strategy

Our implementation follows these steps for each operation:

1. **Before operation**: Check if we need to allocate or resize a buffer
2. **During operation**: Use the standard mbedTLS functions with our buffers
3. **After operation**: Shrink or release buffers that are no longer needed

## Core Components

### 1. Custom Buffer Structure

We use a custom buffer structure that includes metadata:

```c
struct esp_mbedtls_ssl_buf {
    esp_mbedtls_ssl_buf_states state;  // CACHED or NOT_CACHED
    unsigned int len;                  // Buffer size
    unsigned char buf[0];              // Flexible array for actual data
};
```

This structure allows us to:

- Track whether a buffer contains important state
- Store the buffer's size for dynamic resizing
- Use a flexible array member for efficient memory layout

### 2. Buffer States

- **CACHED**: Contains important cryptographic state that must be preserved
- **NOT_CACHED**: Can be safely replaced or released

The state tracking is critical for maintaining TLS protocol security while optimizing memory.

### 3. Critical State Preservation

When replacing large buffers with small cache buffers, we preserve:

- **SSL counter values**: Used for replay protection
- **Initialization vectors**: Required for encryption/decryption

These small amounts of cryptographic state must be maintained between operations to keep the TLS connection secure.

### 4. Memory Management Functions

| Function | Purpose |
|----------|---------|
| `esp_mbedtls_add_tx_buffer()` | Allocates a transmission buffer sized for the outgoing message |
| `esp_mbedtls_free_tx_buffer()` | Replaces TX buffer with small cache buffer after transmission |
| `esp_mbedtls_add_rx_buffer()` | Reads record header and allocates right-sized reception buffer |
| `esp_mbedtls_free_rx_buffer()` | Replaces RX buffer with small cache buffer after processing |

### 5. Function Wrapping

We intercept key mbedTLS functions using GCC's function wrapping:

```c
int __wrap_mbedtls_ssl_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len) {
    // 1. Allocate right-sized buffer
    // 2. Call original function
    // 3. Free buffer when done
}
```

This allows seamless integration without modifying the mbedTLS source code.

## The Handshake Process Design

During TLS handshaking, memory needs change dramatically between steps. Our system tracks the handshake state and manages memory accordingly:

### Client-Side Handshake Memory Management

| Handshake Step | Memory Action |
|----------------|---------------|
| Client Hello | Allocate TX buffer |
| Server Hello | Allocate RX buffer |
| Server Certificate | Allocate RX buffer |
| Certificate Verify | Allocate TX buffer |
| Free certificate resources | Release CA certificates |
| Client Key Exchange | Allocate TX buffer |
| Change Cipher Spec | Small buffer |
| Finished | Small buffers |



## Implementation Design: SSL Read Operation

The dynamic buffer allocation for SSL read operations follows this design:

1. First, read just the TLS record header:

```c
// Read just the header to determine full message size
ssl->in_hdr = msg_head;
ssl->in_len = msg_head + 3;
mbedtls_ssl_fetch_input(ssl, mbedtls_ssl_in_hdr_len(ssl));

// Parse header to get message length
esp_mbedtls_parse_record_header(ssl);
in_msglen = ssl->in_msglen;
```

2. Once we know the exact message size, allocate a buffer that's precisely sized:

```c
// Allocate buffer of right size
buffer_len = in_msglen + overhead; // Add necessary TLS overhead
esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);

// Initialize and set up buffer
esp_mbedtls_init_ssl_buf(esp_buf, buffer_len);
init_rx_buffer(ssl, esp_buf->buf);
```

3. After processing, preserve critical state and free the large buffer:

```c
// Save critical state (counters and IVs)
memcpy(buf, ssl->in_ctr, 8);
memcpy(buf + 8, ssl->in_iv, 8);

// Free large buffer
esp_mbedtls_free_buf(ssl->in_buf);

// Allocate small cache buffer
esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + 16);
esp_mbedtls_init_ssl_buf(esp_buf, 16);

// Restore critical state in small buffer
memcpy(esp_buf->buf, buf, 16);
```

## Configuration Options

The dynamic buffer system includes configurable options:

- `CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT`: Free CA certificates after verification
- `CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA`: Free DHM parameters and key material when no longer needed

These can be enabled in ESP-IDF's menuconfig system.

## Integration Architecture

The implementation uses function wrapping to seamlessly integrate with mbedTLS:

```
Application → mbedTLS API → Our Wrapper Functions → Original mbedTLS Functions
```

Key wrapped functions include:
- `mbedtls_ssl_setup`: Initialize with minimal buffers
- `mbedtls_ssl_read`/`mbedtls_ssl_write`: Dynamic buffer management during I/O 
- `mbedtls_ssl_handshake_client_step`: Handshake-aware memory management
- `mbedtls_ssl_free`: Clean up all allocated memory

## Design Benefits

The dynamic buffer management design provides several key benefits:

1. **Memory Efficiency**: Significantly reduced peak memory usage
2. **Scalability**: Adapts to different TLS message sizes dynamically
3. **Transparency**: Application code doesn't need to be aware of the memory optimization
4. **Compatibility**: Maintains full mbedTLS functionality

## Summary

The dynamic buffer management system in ESP-IDF's mbedTLS port follows a sophisticated architecture that:
1. Allocates only what's needed, when it's needed
2. Preserves critical state in small buffers
3. Is aware of the TLS handshake flow
4. Releases memory as soon as it's no longer required

This architecture enables more efficient use of RAM on memory-constrained devices while maintaining the security guarantees of the TLS protocol.
