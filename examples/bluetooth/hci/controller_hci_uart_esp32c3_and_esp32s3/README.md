ESP-IDF UART HCI Controller (moved)
===================================

**This example has been removed in ESP-IDF v6.1.** It is replaced by the unified example [controller_hci_uart](../controller_hci_uart), which covers every target that supports HCI over UART, including ESP32-C3 and ESP32-S3.

## How to Migrate

1. Use [examples/bluetooth/hci/controller_hci_uart](../controller_hci_uart) and run `idf.py set-target esp32c3` or `idf.py set-target esp32s3`. The HCI UART pins, baud rate, and hardware flow control come from `sdkconfig.defaults.esp32c3` / `sdkconfig.defaults.esp32s3` and match the pins this example used (TX 4, RX 5, RTS 6, CTS 7, 115200 baud).
2. The UHCI-based HCI UART transport is now part of the Bluetooth controller. Enabling `CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER` makes `esp_bt_controller_init()` install the transport and fill in `esp_bt_controller_config_t::hci_tl_funcs`, so the transport implementation that used to live in this example's `main.c` must be dropped from applications derived from it.

See the [README.md](../controller_hci_uart/README.md) of the new example for the full description.
