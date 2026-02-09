| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H4 |
| ----------------- | -------- | -------- | -------- | -------- |

# OpenThread Radio Co-Processor (RCP) Example

This example demonstrates an [OpenThread Radio Co-Processor](https://openthread.io/platforms/co-processor).

OpenThread RCP doesn't function alone, it needs to work together with a Host and this example covers two common user scenarios:
- Work with a Host Processor to perform a [Thread Border Router](https://openthread.io/guides/border-router).
- Work as a [Thread Sniffer](https://openthread.io/guides/pyspinel/sniffer).

## How to use example

### Hardware Required

To run this example, a board with IEEE 802.15.4 module (for example ESP32-H2) is required.

### Configure the project

The default communication interface is port 0 of ESP32-H2 UART running at 460800 baud, change the configuration in [esp_ot_config.h](main/esp_ot_config.h) if you want to use another interface or need different communication parameters.

### RCP Size Optimization Configuration

To optimize the size of the RCP firmware, the following configurations are enabled by default:

```
CONFIG_COMPILER_OPTIMIZATION_SIZE=y
CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT=y
CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT=y
CONFIG_COMPILER_SAVE_RESTORE_LIBCALLS=y
CONFIG_ESP_ERR_TO_NAME_LOOKUP=n
CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT=y
CONFIG_LOG_DEFAULT_LEVEL_NONE=y
CONFIG_LIBC_NEWLIB_NANO_FORMAT=y
CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC=n
CONFIG_OPENTHREAD_LOG_LEVEL_NONE=y
```
Configure them via `idf.py menuconfig` if you need.

The firmware size are as follows (reference value):

```
                                Before Optimization     After Optimization

esp_ot_rcp.bin for esp32h2          314KB                   184KB

esp_ot_rcp.bin for esp32c6          299KB                   208kb
```

### Build and Flash

Build the project and flash it to the board:

```
idf.py -p <PORT> build flash
```

Now you'll get an OpenThread RCP, you can try the following use cases:

#### Thread Border Router

Please refer to [ot_br](../ot_br) example for the setup steps.

#### Thread Sniffer

Please refer to [Thread Sniffer](https://openthread.io/guides/pyspinel/sniffer) for the detailed instructions.

### Debug Feature: Turn on RCP console

The RCP console feature allows you to send console commands to the RCP firmware via the Spinel protocol. This is useful for debugging IEEE 802.15.4 PHY register issues and accessing debug statistics.

To enable the RCP console, you can either:

**Option 1: Use the debug configuration file**

Use the debug configuration file and rebuild:

```
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.debug" set-target esp32h2
idf.py -p <PORT> build flash
```

This enables the following commands:

- `phyreg -g` - Get IEEE 802.15.4 PHY register debug information, including PBUS registers, PHY calibration data, I2C checks, and register validation
- `help` - List all available console commands

**Option 2: Configure manually via menuconfig**

To enable other IEEE 802.15.4 debug commands (detailed in `examples/ieee802154/components/cmd_ieee802154_debug/README.md`), run `idf.py menuconfig` and enable the following options:

```
Component config → OpenThread → Thread Core Features → Thread Radio Co-Processor Feature → Enable RCP console via Spinel
Component config → IEEE 802.15.4 → Enable IEEE802154 Debug
Component config → IEEE 802.15.4 → IEEE 802.15.4 Debug Parameters → **Select relevant debug features as required**
Component config → Log → Log Level → Default log verbosity → Info
Component config → ESP System Settings → Task watchdog timeout period (seconds) → 10
```

**Note:** It's recommended to increase the watchdog timer timeout to 10 seconds when enabling debug features, as large debug prints may take longer to complete.

Then rebuild and flash:

```
idf.py -p <PORT> build flash
```
