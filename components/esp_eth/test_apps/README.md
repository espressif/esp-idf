| Supported Targets | ESP32 | ESP32-P4 |
| ----------------- | ----- | -------- |

# Ethernet Test

This test app is used to test Ethernet MAC behavior with different chips.

## Prerequisites
Install third part Python packages:

```bash
pip install scapy
```

## Chip-Specific Configuration

Different Ethernet chips may have deviations from the standard behavior or may not support certain features. To accommodate these chip-specific differences, the test framework provides Kconfig options that allow disabling or modifying specific tests that are incompatible with certain chips.

For example, some chips may not support 10 Mbps loopback mode, or may have known errata that require workarounds. These Kconfig options are defined in `main/Kconfig.projbuild` and can be configured through the ESP-IDF menuconfig system to adjust test behavior for the specific chip under test.

Refer to `main/Kconfig.projbuild` for the complete list of available configuration options and their descriptions.

## Test Framework Overview

The test framework provides common initialization and helper functions through `esp_eth_test_utils.h` and `esp_eth_test_utils.c`. These modules handle Ethernet initialization, event registration, and resource management automatically using Unity's `setUp()` and `tearDown()` functions.

### Unity setUp() and tearDown()

The `setUp()` function automatically performs the following initialization before each test case:

1. **Sets up event handling** (creates event group and default event loop)
2. **Initializes the Ethernet driver** by calling `esp_eth_test_eth_init()` (uses Ethernet init component by default, can be customized - see [Customizing Ethernet Initialization](#customizing-ethernet-initialization) section below)
3. **Optionally sets up esp-netif** if the `[esp-netif]` marker is used (see [Markers](#test-markers) section below)
4. **Registers event handlers**:
   - `eth_test_default_event_handler()` for ETH_EVENT events (ETHERNET_EVENT_START, ETHERNET_EVENT_STOP, ETHERNET_EVENT_CONNECTED, ETHERNET_EVENT_DISCONNECTED)
   - `eth_test_got_ip_event_handler()` for IP_EVENT_ETH_GOT_IP events (only when `[esp-netif]` marker is used)

The `tearDown()` function automatically performs cleanup after each test case:

1. **Frees all allocated memory** tracked by the test framework
2. **Unregisters event handlers**
3. **Stops and deinitializes the Ethernet driver** by calling `esp_eth_test_eth_deinit()` (uses Ethernet init component by default, can be customized - see [Customizing Ethernet Initialization](#customizing-ethernet-initialization) section below)
4. **Destroys the event loop and event group**

### Customizing Ethernet Initialization

By default, the test framework initializes Ethernet using the **Ethernet init component**. This provides initialization of Ethernet hardware based on Kconfig settings.

For test cases that require custom Ethernet initialization (e.g., chip not supported by Ethernet init component, specific MAC/PHY configurations, multiple Ethernet ports, or custom hardware setup), you can override the weak functions:

- **`esp_eth_test_eth_init(esp_eth_handle_t *eth_handle)`**: Override this function to provide custom Ethernet initialization. The function should initialize the Ethernet driver and return the handle via the `eth_handle` parameter.

    > [!IMPORTANT]
    > Initialization function must print DUT PHY identification string as it is expected by `pytest` script. The PHY identification message must be in the following format:
    > ```c
    > printf("DUT PHY: %s", my_phy_str);
    >```

- **`esp_eth_test_eth_deinit(esp_eth_handle_t eth_handle)`**: Override this function to provide custom Ethernet deinitialization. The function should clean up the Ethernet driver resources.

### Test Markers

Test cases can use markers in their Unity test identifiers to control initialization behavior:

#### `[esp-netif]` Marker

When this marker is included in the test case identifier, `setUp()` will additionally initialize TCP/IP stack by:

- Creating an ESP-NETIF interface using `ESP_NETIF_DEFAULT_ETH()` configuration
- Attaching the Ethernet driver to the netif using `esp_eth_new_netif_glue()`
- Registering the `eth_test_got_ip_event_handler()` for IP_EVENT_ETH_GOT_IP events

**Example:**
```c
TEST_CASE("ethernet dhcp test", "[ethernet][esp-netif]")
{
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();
    esp_netif_t *netif = eth_test_get_netif();  // Valid when [esp-netif] is used
    
    // Test code using TCP/IP stack...
}
```

#### `[skip_setup_teardown]` Marker

When this marker is included, `setUp()` and `tearDown()` will skip all automatic initialization and cleanup. This allows the test to perform its own custom initialization and cleanup procedures.

> [!IMPORTANT]
> When using this marker, you must manually:
> - Register event handlers using `eth_test_default_event_handler()` and/or `eth_test_got_ip_event_handler()`
> - Initialize and deinitialize all resources except for memory allocated by `eth_test_alloc()`

> [!NOTE]
> Memory allocated through `eth_test_alloc()` is automatically freed by `tearDown()` even when using this marker.

**Example:**
```c
TEST_CASE("internal emac interrupt priority", "[esp_emac][skip_setup_teardown]")
{
    // Manual initialization
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ESP_OK(esp_event_loop_create_default());
    // Register handlers manually if needed
    // ... custom test code ...
}
```

### Helper Functions

#### Getting Initialized Handles

- **`eth_test_get_eth_handle()`**: Returns the Ethernet handle initialized by `setUp()`. Returns `NULL` if initialization failed or if `[skip_setup_teardown]` marker is used.
- **`eth_test_get_default_event_group()`**: Returns the event group handle created by `setUp()`. Use this to wait for Ethernet events using `xEventGroupWaitBits()` with the predefined bits (ETH_START_BIT, ETH_STOP_BIT, ETH_CONNECT_BIT, ETH_GOT_IP_BIT).
- **`eth_test_get_netif()`**: Returns the ESP-NETIF handle initialized by `setUp()`. **Only valid when `[esp-netif]` marker is used**, otherwise returns `NULL`.

#### PHY Register Manipulation

- **`eth_test_set_phy_reg_bits()`**: Sets specific bits in a PHY register with retry logic. Useful for configuring PHY behavior during tests.
- **`eth_test_clear_phy_reg_bits()`**: Clears specific bits in a PHY register with retry logic.

Both functions include automatic retry logic with configurable maximum attempts and delays between attempts.

#### Memory Management

- **`eth_test_alloc(size)`**: Allocates memory from the heap. The framework automatically tracks all allocations and frees them in `tearDown()`. Maximum of `MAX_HEAP_ALLOCATION_POINTERS` (20) allocations are supported.
- **`eth_test_free(ptr)`**: Frees a specific allocation. Generally not needed as `tearDown()` automatically frees all tracked allocations.
- **`eth_test_free_all()`**: Frees all tracked allocations. Called automatically by `tearDown()`.

#### Event Handlers

- **`eth_test_default_event_handler()`**: Default handler for ETH_EVENT events. Automatically registered by `setUp()` unless `[skip_setup_teardown]` is used. Sets event bits in the event group for:
  - ETHERNET_EVENT_START → ETH_START_BIT
  - ETHERNET_EVENT_STOP → ETH_STOP_BIT
  - ETHERNET_EVENT_CONNECTED → ETH_CONNECT_BIT

- **`eth_test_got_ip_event_handler()`**: Handler for IP_EVENT_ETH_GOT_IP events. Automatically registered by `setUp()` when `[esp-netif]` marker is used. Sets ETH_GOT_IP_BIT in the event group and logs IP configuration.

### Usage Example

```c
TEST_CASE("ethernet basic test", "[ethernet][esp-netif]")
{
    // Get handles initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();
    esp_netif_t *netif = eth_test_get_netif();
    
    EventBits_t bits = 0;
    
    // Start Ethernet driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    
    // Wait for Ethernet to start
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, 
                               pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    
    // Wait for link connection
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, 
                               pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    
    // Wait for IP address (only when [esp-netif] is used)
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, 
                               pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);
    
    // Test code here...
    
    // Stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    
    // Wait for stop event
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, 
                               pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    
    // tearDown() will automatically clean up everything
}
```