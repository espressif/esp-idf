| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# OpenThread Radio Co-Processor (RCP) Example

This example demonstrates an [OpenThread Radio Co-Processor](https://openthread.io/platforms/co-processor).

OpenThread RCP doesn't function alone, it needs to work together with a Host and this example covers two common user scenarios:
- Work with a Host Processor to perform a [Thread Border Router](https://openthread.io/guides/border-router).
- Work as a [Thread Sniffer](https://openthread.io/guides/pyspinel/sniffer).

## How to use example

### Hardware Required

To run this example, an ESP32-H2 board is required.

### Configure the project

The default communication interface is port 0 of ESP32-H2 UART running at 115200 baud, change the configuration in [esp_ot_config.h](main/esp_ot_config.h) if you want to use another interface or need different communication parameters.

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
