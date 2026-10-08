ESP-IDF UART HCI Controller (moved)
===================================

**This example has been removed in ESP-IDF v6.1.** It is replaced by the unified example [controller_hci_uart](../../hci/controller_hci_uart), which covers every target that supports HCI over UART.

## How to Migrate

1. Use [examples/bluetooth/hci/controller_hci_uart](../../hci/controller_hci_uart) and run `idf.py set-target <target>`. The HCI UART pins and baud rate come from `sdkconfig.defaults.<target>` and match the pins this example used (TX 8, RX 9, 115200 baud, no hardware flow control).
2. The DTM configuration commands of this example are now part of the new example's console, enabled with `CONFIG_ENABLE_BLE_DTM_CONFIGURATION_COMMAND`.

See the [README.md](../../hci/controller_hci_uart/README.md) of the new example for the full description.
