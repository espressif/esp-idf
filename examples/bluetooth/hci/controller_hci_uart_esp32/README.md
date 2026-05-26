ESP-IDF UART HCI Controller (moved)
===================================

**This example has been removed in ESP-IDF v6.1.** It is replaced by the unified example [controller_hci_uart](../controller_hci_uart), which covers every target that supports HCI over UART, including ESP32.

## How to migrate

1. Use [examples/bluetooth/hci/controller_hci_uart](../controller_hci_uart) and run `idf.py set-target esp32`. The HCI UART pins, baud rate, and hardware flow control come from `sdkconfig.defaults.esp32` and match the pins this example used (TX 5, RX 18, RTS 19, CTS 23, 921600 baud).
2. The application no longer configures the HCI UART itself. `esp_bt_controller_init()` enables the UART and UHCI bus clocks and routes the pins when `CONFIG_BTDM_CTRL_HCI_UART_INIT_BY_CONTROLLER` is enabled, so the `uart_ll_*` / `uhci_ll_*` calls that used to live in `app_main()` must be removed from applications derived from this example.

See the [README.md](../controller_hci_uart/README.md) of the new example for the full description.
