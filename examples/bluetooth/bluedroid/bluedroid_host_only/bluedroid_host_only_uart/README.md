| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

ESP-IDF UART HCI Host
=====================

This is a Bluetooth Host use UART as HCI IO. This require the UART device support RTS/CTS mandatory.

It can do the configuration of UART baudrate by menuconfig.

## Example Layout

This example is modified based on [bt_discovery](../../classic_bt/bt_discovery), and all modifications are listed below:

- Removed all dependencies on controller from `main.c`.

```
#include "esp_bt.h"

...

ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
    ESP_LOGE(GAP_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
    return;
}

if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
    ESP_LOGE(GAP_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
    return;
}
```

- Add support for uart driver: `uart_driver.c` and `uart_driver.h`.

- Initialize UART driver in `main.c`.

```
#include "esp_hci_api.h"
#include "uart_driver.h"

...

/* initialize HCI TRANSPORT first */
hci_uart_open();
/* get HCI driver operations */
esp_bluedroid_hci_driver_operations_t operations = {
    .send = hci_uart_send,
    .check_send_available = hci_check_send_available,
    .register_host_callback = hci_register_host_callback,
};
esp_bluedroid_attach_hci_driver(&operations);
```

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP development board. To connect UART to another board running a Bluetooth controller. For example, [controller_hci_uart_esp32](../../../hci/controller_hci_uart_esp32).

### Configure the project

```
idf.py menuconfig
```

- UART baudrate can be configured in `Example Configuration > UART Baudrate for HCI`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting
